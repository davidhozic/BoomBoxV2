#include "global.hpp"
#include "libs/EEPROM/EEPROM.hpp"
#include "includes/audio.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/




/**********************************************************************
 *  FUNCTION:    CREATE_ANIMATION
 *  PARAMETERS:  uint8_t task_index, uint8_t color
 *  DESCRIPTION: Deletes previously active task, sets strip color and 
 *               creates a strip task                            
 **********************************************************************/
void AUVS::CREATE_ANIMATION(uint8_t task_index, uint8_t color)                
{
    set_strip_color(color);
    deleteTASK(&handle_active_strip_mode);
    xTaskCreate(AUVS::strip_animations[task_index].f_ptr, "str", 256, NULL, 4, &handle_active_strip_mode);
}


/**********************************************************************
 *  FUNCTION:    update_strip
 *  PARAMETERS:  void
 *  DESCRIPTION: updates strip with current color and brightness                         
 **********************************************************************/
void AUVS::update_strip()
{
	writePWM(strip_red_pin, strip_port,		current_color[STRIP_RED]	*	 current_brightness / 255.00);
	writePWM(strip_green_pin, strip_port,	current_color[STRIP_GREEN]  *	 current_brightness / 255.00);
	writePWM(strip_blue_pin, strip_port,	current_color[STRIP_BLUE]	*    current_brightness / 255.00);
}


/**********************************************************************
 *  FUNCTION:    flash_strip
 *  PARAMETERS:  void
 *  DESCRIPTION: flashes the strip (used in settings_ui in state
 *               transition)                         
 **********************************************************************/
void AUVS::flash_strip() //Utripanje (Izhod iz STATE_SCROLL stata / menjava mikrofona)
{
	set_strip_color(COLOR_WHITE);
	for (uint8_t i = 0; i < 5; i++)
	{
		set_strip_brightness(0);
        update_strip();
		delay_FreeRTOS_ms(125);
		set_strip_brightness(255);
        update_strip();
		delay_FreeRTOS_ms(125);
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
		strip_colors[BARVA].color_data[STRIP_RED] >= current_color[STRIP_RED]	  ? smer[STRIP_RED]	= 1 : smer[STRIP_RED]	  = -1;
		strip_colors[BARVA].color_data[STRIP_GREEN] >= current_color[STRIP_GREEN] ? smer[STRIP_GREEN] = 1 : smer[STRIP_GREEN] = -1;
		strip_colors[BARVA].color_data[STRIP_BLUE] >= current_color[STRIP_BLUE]  ? smer[STRIP_BLUE]  = 1 : smer[STRIP_BLUE]  = -1;

		current_color[STRIP_RED]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_RED]);
		current_color[STRIP_GREEN]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_GREEN]);
		current_color[STRIP_BLUE]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_BLUE]);

		/* Check over color for seperate color indexes */
		if (smer[STRIP_RED] == 1 && current_color[STRIP_RED] > strip_colors[BARVA].color_data[STRIP_RED] || smer[STRIP_RED] == -1 && current_color[STRIP_RED] < strip_colors[BARVA].color_data[STRIP_RED] )
		{
			current_color[STRIP_RED] = strip_colors[BARVA].color_data[STRIP_RED];
		}
	
		if (smer[STRIP_GREEN] == 1 && current_color[STRIP_GREEN] > strip_colors[BARVA].color_data[STRIP_GREEN] 	|| smer[STRIP_GREEN] == -1 && current_color[STRIP_GREEN] < strip_colors[BARVA].color_data[STRIP_GREEN] )
		{
			current_color[STRIP_GREEN] = strip_colors[BARVA].color_data[STRIP_GREEN];
		}

		if (smer[STRIP_BLUE] == 1 && current_color[STRIP_BLUE] > strip_colors[BARVA].color_data[STRIP_BLUE] || smer[STRIP_BLUE] == -1 && current_color[STRIP_BLUE] < strip_colors[BARVA].color_data[STRIP_BLUE] )
		{
			current_color[STRIP_BLUE] = strip_colors[BARVA].color_data[STRIP_BLUE];
		}


		update_strip();
		
		delay_FreeRTOS_ms(  (animation_time*AUVS_CONFIG_BRIGHTNESS_CHANGE) / 255  );

	}while ( current_color[STRIP_RED]	  != strip_colors[BARVA].color_data[STRIP_RED]   ||
			 current_color[STRIP_GREEN]   != strip_colors[BARVA].color_data[STRIP_GREEN] ||
		 	 current_color[STRIP_BLUE]	  != strip_colors[BARVA].color_data[STRIP_BLUE]	);	// While current colors different from wanted
}

/**********************************************************************
 *  FUNCTION:    brightness_fade
 *  PARAMETERS:  int8_t direction, uint16_t animation time
 *  DESCRIPTION: Fades strip brightness into direction to max/min value
 **********************************************************************/
void AUVS::brightness_fade(char smer, unsigned short animation_time)
{
	do
	{
		current_brightness += AUVS_CONFIG_BRIGHTNESS_CHANGE * smer;
		current_brightness = current_brightness < 0 ? 0 :    current_brightness;
		current_brightness = current_brightness > 255 ? 255 : current_brightness;
		update_strip();
		delay_FreeRTOS_ms(  (animation_time*AUVS_CONFIG_BRIGHTNESS_CHANGE)/255  );
	}while (smer > 0 ? current_brightness < 255 : current_brightness > 0);
}

/**********************************************************************
 *  FUNCTION:    strip_off
 *  PARAMETERS:  void
 *  DESCRIPTION: Sets strip mode to OFF, deletes auvs task and changes
 *               animation time to slow for use in settings ui                         
 **********************************************************************/
void AUVS::strip_off()
{
	strip_mode = AUVS_AN_STRIP_OFF;
	deleteTASK(&handle_audio_meass);
	deleteTASK(&handle_active_strip_mode);
	delay_FreeRTOS_ms(10);
	animation_time = AUVS_CONFIG_SLOW_ANIMATION_TIME_MS;
	brightDOWN(AUVS_CONFIG_SLOW_ANIMATION_TIME_MS);
}

/**********************************************************************
 *  FUNCTION:    strip_on
 *  PARAMETERS:  void
 *  DESCRIPTION: Creates meassuring tasks, reads strip mode from eeprom,
 *               changes the animation time back to normal                         
 **********************************************************************/
void AUVS::strip_on()
{
	xTaskCreate(signal_measure, "avg_vol", 128, NULL, 1, &handle_audio_meass);
	strip_mode = EEPROM.beri(EEPROM_ADDRESS_STRIP_MODE);
	
	/* EEPROM address is empty */
	if (strip_mode == 0xFF)
	{
		strip_mode = 0;
	}   

	animation_time = AUVS_CONFIG_NORMAL_ANIMATION_TIME_MS;
	delay_FreeRTOS_ms(10);
}

/**********************************************************************
 *  FUNCTION:    set_strip_mode
 *  PARAMETERS:  uint8_t mode
 *  DESCRIPTION: Sets strip mode index to wanted mode and updates
 *               EEPROM                         
 **********************************************************************/
void AUVS::set_strip_mode(uint8_t mode)
{
	strip_mode = mode;
	EEPROM.pisi(strip_mode,EEPROM_ADDRESS_STRIP_MODE);
}

/**********************************************************************
 *  FUNCTION:    set_strip_color
 *  PARAMETERS:  uint8_t color_index
 *  DESCRIPTION: Sets the color to wanted index      
 **********************************************************************/
void AUVS::set_strip_color(unsigned char barva_index)
{
		current_color[STRIP_RED]   = strip_colors[barva_index].color_data[STRIP_RED];
		current_color[STRIP_GREEN] = strip_colors[barva_index].color_data[STRIP_GREEN];
		current_color[STRIP_BLUE]  = strip_colors[barva_index].color_data[STRIP_BLUE];
}


/**********************************************************************
 *  FUNCTION:    set_strip_brightness 
 *  PARAMETERS:  uint8_t brightness
 *  DESCRIPTION: Sets strip brightness to wanted brightness and updates
 *               strip       
 **********************************************************************/
void AUVS::set_strip_brightness(uint8_t value)
{
	current_brightness = value;
    update_strip();
}