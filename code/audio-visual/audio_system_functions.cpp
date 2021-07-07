#include "common.hpp"
#include "libs/EEPROM/EEPROM.hpp"
#include "includes/audio.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"

/**********************************************************************
 *  FUNCTION:    create_animation
 *  PARAMETERS:  uint8_t task_index, uint8_t color
 *  DESCRIPTION: Deletes previously active task, sets strip color and 
 *               creates a strip task                            
 **********************************************************************/
void AUVS::create_animation(uint8_t task_index, uint8_t color)                
{
    /* If terminator is sent in, don't change color (assume task will take care of that) */
    if (color < COLOR_TERMINATOR)
        set_strip_color(color);
    
    if (strip.strip_mode == AUVS_AN_STRIP_OFF)                  
        strip.animation_time = AUVS_CFG_SLOW_ANIMATION_TIME_MS;   // Strip light up by user ui (for showcase)
    else
        strip.animation_time = AUVS_CFG_NORMAL_ANIMATION_TIME_MS;

    deleteTASK(&handle_active_strip_mode);
    xTaskCreate(strip.strip_animations[task_index].f_ptr, "str", TASK_CFG_TASK_DEFAULT_STACK, NULL, 4, &handle_active_strip_mode);
}


/**********************************************************************
 *  FUNCTION:    update_strip
 *  PARAMETERS:  void
 *  DESCRIPTION: updates strip with current color and brightness                         
 **********************************************************************/
void AUVS::update_strip()
{
	writePWM(GLOBAL_CFG_PIN_LED_STRIP_R, GLOBAL_CFG_PORT_LED_STRIP,		strip.current_color[STRIP_RED]	*	 (strip.current_brightness / 255.00));
	writePWM(GLOBAL_CFG_PIN_LED_STRIP_G, GLOBAL_CFG_PORT_LED_STRIP, 	strip.current_color[STRIP_GREEN]  *	 (strip.current_brightness / 255.00));
	writePWM(GLOBAL_CFG_PIN_LED_STRIP_B, GLOBAL_CFG_PORT_LED_STRIP,	    strip.current_color[STRIP_BLUE]	*    (strip.current_brightness / 255.00));
}


/**********************************************************************
 *  FUNCTION:    flash_strip
 *  PARAMETERS:  void
 *  DESCRIPTION: flashes the strip (used in settings_ui in state
 *               transition)                         
 **********************************************************************/
void AUVS::flash_strip() //Utripanje (Izhod iz STATE_SCROLL stata / menjava mikrofona)
{
	for (uint8_t i = 0; i < 5; i++)
	{
        delay_FreeRTOS_ms(125);
        set_strip_brightness(255);
        update_strip();
        delay_FreeRTOS_ms(125);
        set_strip_brightness(0);
        update_strip();
	}
}

/**********************************************************************
 *  FUNCTION:    color_shift
 *  PARAMETERS:  uint8_t color_index, uint16_t animation_time
 *  DESCRIPTION: fades strip from the current color to the wanted color                         
 **********************************************************************/
void AUVS::color_shift(uint8_t BARVA, unsigned short animation_time)
{
	char smer[3];
    do
	{
		strip.strip_colors[BARVA].color_data[STRIP_RED]   >= strip.current_color[STRIP_RED]	   ?  smer[STRIP_RED]   = 1 : smer[STRIP_RED]	  = -1;
		strip.strip_colors[BARVA].color_data[STRIP_GREEN] >= strip.current_color[STRIP_GREEN]  ?  smer[STRIP_GREEN] = 1 : smer[STRIP_GREEN] = -1;
		strip.strip_colors[BARVA].color_data[STRIP_BLUE]  >= strip.current_color[STRIP_BLUE]   ?  smer[STRIP_BLUE]  = 1 : smer[STRIP_BLUE]  = -1;

		strip.current_color[STRIP_RED]	 +=	 (AUVS_CFG_COLOR_CHANGE * smer[STRIP_RED]);
		strip.current_color[STRIP_GREEN] +=(AUVS_CFG_COLOR_CHANGE * smer[STRIP_GREEN]);
		strip.current_color[STRIP_BLUE]	 +=	(AUVS_CFG_COLOR_CHANGE * smer[STRIP_BLUE]);

		/* Check over color for seperate color indexes */
		if (smer[STRIP_RED] == 1  && strip.current_color[STRIP_RED] > strip.strip_colors[BARVA].color_data[STRIP_RED] ||
            smer[STRIP_RED] == -1 && strip.current_color[STRIP_RED] < strip.strip_colors[BARVA].color_data[STRIP_RED] )
		{
			strip.current_color[STRIP_RED] = strip.strip_colors[BARVA].color_data[STRIP_RED];
		}
	
		if (smer[STRIP_GREEN] ==  1 && strip.current_color[STRIP_GREEN] > strip.strip_colors[BARVA].color_data[STRIP_GREEN] || 
            smer[STRIP_GREEN] == -1 && strip.current_color[STRIP_GREEN] < strip.strip_colors[BARVA].color_data[STRIP_GREEN] )
		{
			strip.current_color[STRIP_GREEN] = strip.strip_colors[BARVA].color_data[STRIP_GREEN];
		}

		if (smer[STRIP_BLUE] ==  1 && strip.current_color[STRIP_BLUE] > strip.strip_colors[BARVA].color_data[STRIP_BLUE] ||
            smer[STRIP_BLUE] == -1 && strip.current_color[STRIP_BLUE] < strip.strip_colors[BARVA].color_data[STRIP_BLUE] )
		{
			strip.current_color[STRIP_BLUE] = strip.strip_colors[BARVA].color_data[STRIP_BLUE];
		}


		update_strip();
		
		delay_FreeRTOS_ms(  (animation_time*AUVS_CFG_COLOR_CHANGE) / 255  );

	}while ( strip.current_color[STRIP_RED]	  != strip.strip_colors[BARVA].color_data[STRIP_RED]   ||
			 strip.current_color[STRIP_GREEN]   != strip.strip_colors[BARVA].color_data[STRIP_GREEN] ||
		 	 strip.current_color[STRIP_BLUE]	  != strip.strip_colors[BARVA].color_data[STRIP_BLUE]	);	// While current colors different from wanted
}

/**********************************************************************
 *  FUNCTION:    brightness_fade
 *  PARAMETERS:  int8_t direction, uint16_t animation time
 *  DESCRIPTION: Fades strip brightness into direction to max/min value
 **********************************************************************/
void AUVS::brightness_fade(char smer, unsigned short animation_time)
{
    update_strip();
	do
	{
		strip.current_brightness += AUVS_CFG_BRIGHTNESS_CHANGE * smer;
		strip.current_brightness = strip.current_brightness < 0 ? 0 :    strip.current_brightness;
		strip.current_brightness = strip.current_brightness > 255 ? 255 : strip.current_brightness;
		update_strip();
		delay_FreeRTOS_ms(  (animation_time*AUVS_CFG_BRIGHTNESS_CHANGE)/255 );
	}while (smer > 0 ? strip.current_brightness < 255 : strip.current_brightness > 0);
}

/**********************************************************************
 *  FUNCTION:    strip_off
 *  PARAMETERS:  void
 *  DESCRIPTION: Sets strip mode to OFF, deletes auvs task and changes
 *               animation time to slow for use in settings ui                         
 **********************************************************************/
void AUVS::strip_off()
{
	strip.strip_mode = AUVS_AN_STRIP_OFF;
	holdTASK(&handle_audio_system);
	deleteTASK(&handle_active_strip_mode);
	brightDOWN(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
}

/**********************************************************************
 *  FUNCTION:    strip_on
 *  PARAMETERS:  void
 *  DESCRIPTION: Creates meassuring tasks, reads strip mode from eeprom,
 *               changes the animation time back to normal                         
 **********************************************************************/
void AUVS::strip_on()
{
	strip.strip_mode = EEPROM.beri(GLOBAL_CFG_EEPROM_ADDR_STRIP_MODE);
	
	/* EEPROM address is empty */
	if (strip.strip_mode == 0xFF)
	{
		strip.strip_mode = 0;
	}   

    if (strip.strip_mode != AUVS_AN_STRIP_OFF)
        resumeTASK(&m_audio_system.handle_audio_system);

}

/**********************************************************************
 *  FUNCTION:    set_strip_mode
 *  PARAMETERS:  uint8_t mode
 *  DESCRIPTION: Sets strip mode index to wanted mode and updates
 *               EEPROM                         
 **********************************************************************/
void AUVS::set_strip_mode(uint8_t mode)
{
	strip.strip_mode = mode;
	EEPROM.pisi(strip.strip_mode,GLOBAL_CFG_EEPROM_ADDR_STRIP_MODE);
}

/**********************************************************************
 *  FUNCTION:    set_strip_color
 *  PARAMETERS:  uint8_t color_index
 *  DESCRIPTION: Sets the color to wanted index      
 **********************************************************************/
void AUVS::set_strip_color(unsigned char barva_index)
{
    strip.current_color[STRIP_RED]   = strip.strip_colors[barva_index].color_data[STRIP_RED];
    strip.current_color[STRIP_GREEN] = strip.strip_colors[barva_index].color_data[STRIP_GREEN];
    strip.current_color[STRIP_BLUE]  = strip.strip_colors[barva_index].color_data[STRIP_BLUE];
}

/**********************************************************************
 *  FUNCTION:    set_strip_brightness 
 *  PARAMETERS:  uint8_t brightness
 *  DESCRIPTION: Sets strip brightness to wanted brightness       
 **********************************************************************/
void AUVS::set_strip_brightness(uint8_t value)
{
	strip.current_brightness = value;
}