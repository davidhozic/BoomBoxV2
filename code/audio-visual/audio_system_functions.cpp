#include "global.hpp"
#include "libs/EEPROM/EEPROM.hpp"
#include "includes/audio.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/



void AUVS::update_strip()
{
	writePWM(strip_red_pin, strip_port,		m_audio_system.current_color[STRIP_RED]	*	m_audio_system.current_brightness/255.00);
	writePWM(strip_green_pin, strip_port,	m_audio_system.current_color[STRIP_GREEN] *	m_audio_system.current_brightness/255.00);
	writePWM(strip_blue_pin, strip_port,	m_audio_system.current_color[STRIP_BLUE]	*	m_audio_system.current_brightness/255.00);
}

void AUVS::flash_strip() //Utripanje (Izhod iz STATE_SCROLL stata / menjava mikrofona)
{
	set_strip_color(COLOR_WHITE);
	for (uint8_t i = 0; i < 5; i++)
	{
		set_strip_brightness(0);
		delay_FreeRTOS_ms(125);
		set_strip_brightness(255);
		delay_FreeRTOS_ms(125);
	}
}

void AUVS::color_shift(uint8_t BARVA, unsigned short animation_time)
{
	char smer[3];
	while ( m_audio_system.current_color[STRIP_RED]	 != AUVS::strip_colors[BARVA].color_data[STRIP_RED] ||
			m_audio_system.current_color[STRIP_GREEN]  != AUVS::strip_colors[BARVA].color_data[STRIP_GREEN] ||
			m_audio_system.current_color[STRIP_BLUE]	 != AUVS::strip_colors[BARVA].color_data[STRIP_BLUE]		)	// While current colors different from wanted
	{
		AUVS::strip_colors[BARVA].color_data[STRIP_RED] >= m_audio_system.current_color[STRIP_RED]	  ? smer[STRIP_RED]	= 1 : smer[STRIP_RED]	  = -1;
		AUVS::strip_colors[BARVA].color_data[STRIP_GREEN] >= m_audio_system.current_color[STRIP_GREEN] ? smer[STRIP_GREEN] = 1 : smer[STRIP_GREEN] = -1;
		AUVS::strip_colors[BARVA].color_data[STRIP_BLUE] >= m_audio_system.current_color[STRIP_BLUE]  ? smer[STRIP_BLUE]  = 1 : smer[STRIP_BLUE]  = -1;

		m_audio_system.current_color[STRIP_RED]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_RED]);
		m_audio_system.current_color[STRIP_GREEN]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_GREEN]);
		m_audio_system.current_color[STRIP_BLUE]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_BLUE]);

		//Preveri prenihaj:

		m_audio_system.current_color[STRIP_RED] = (smer[STRIP_RED] == 1 && m_audio_system.current_color[STRIP_RED] > AUVS::strip_colors[BARVA].color_data[STRIP_RED]) ||
		(smer[STRIP_RED] == -1 && m_audio_system.current_color[STRIP_RED] < AUVS::strip_colors[BARVA].color_data[STRIP_RED]) ? AUVS::strip_colors[BARVA].color_data[STRIP_RED]: m_audio_system.current_color[STRIP_RED] ;	//Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
		
		m_audio_system.current_color[STRIP_GREEN] = (smer[STRIP_GREEN] == 1 && m_audio_system.current_color[STRIP_GREEN] > AUVS::strip_colors[BARVA].color_data[STRIP_GREEN]) ||
		(smer[STRIP_GREEN] == -1 && m_audio_system.current_color[STRIP_GREEN] < AUVS::strip_colors[BARVA].color_data[STRIP_GREEN]) ? AUVS::strip_colors[BARVA].color_data[STRIP_GREEN]: m_audio_system.current_color[STRIP_GREEN] ;
		
		m_audio_system.current_color[STRIP_BLUE] = (smer[STRIP_BLUE] == 1 && m_audio_system.current_color[STRIP_BLUE] > AUVS::strip_colors[BARVA].color_data[STRIP_BLUE]) ||
		(smer[STRIP_BLUE] == -1 && m_audio_system.current_color[STRIP_BLUE] < AUVS::strip_colors[BARVA].color_data[STRIP_BLUE]) ? AUVS::strip_colors[BARVA].color_data[STRIP_BLUE]: m_audio_system.current_color[STRIP_BLUE] ;

		update_strip();
		
		delay_FreeRTOS_ms(  (animation_time*AUVS_CONFIG_BRIGHTNESS_CHANGE)/255  );
	}
}

void AUVS::brightness_fade(char smer, unsigned short animation_time)
{
	while (smer > 0 ? m_audio_system.current_brightness < 255 : m_audio_system.current_brightness > 0)
	{
		m_audio_system.current_brightness += AUVS_CONFIG_BRIGHTNESS_CHANGE * smer;
		m_audio_system.current_brightness = m_audio_system.current_brightness < 0 ? 0 : m_audio_system.current_brightness;
		m_audio_system.current_brightness = m_audio_system.current_brightness > 255 ? 255 : m_audio_system.current_brightness;
		update_strip();
		delay_FreeRTOS_ms(  (animation_time*AUVS_CONFIG_BRIGHTNESS_CHANGE)/255  );
	}
}


void AUVS::strip_off()
{
	strip_mode = AUVS_AN_STRIP_OFF;
	deleteTASK(&handle_audio_meass);
	deleteTASK(&handle_active_strip_mode);
	delay_FreeRTOS_ms(10);
	m_audio_system.animation_time = AUVS_CONFIG_SLOW_ANIMATION_TIME_MS;
	brightDOWN(AUVS_CONFIG_SLOW_ANIMATION_TIME_MS);
}

void AUVS::strip_on()
{
	xTaskCreate(signal_measure, "avg_vol", 128, NULL, 3, &handle_audio_meass);
	strip_mode = EEPROM.beri(EEPROM_ADDRESS_STRIP_MODE);
	
	/* EEPROM address is empty */
	if (strip_mode == 0xFF)
	{
		strip_mode = 0;
	}   

	m_audio_system.animation_time = AUVS_CONFIG_NORMAL_ANIMATION_TIME_MS;
	delay_FreeRTOS_ms(10);
}

void AUVS::set_strip_mode(uint8_t mode)
{
	strip_mode = mode;
	EEPROM.pisi(strip_mode,EEPROM_ADDRESS_STRIP_MODE);
}

void AUVS::set_strip_color(unsigned char barva_index)
{
		current_color[STRIP_RED]   = AUVS::strip_colors[barva_index].color_data[STRIP_RED];
		current_color[STRIP_GREEN] = AUVS::strip_colors[barva_index].color_data[STRIP_GREEN];
		current_color[STRIP_BLUE]  = AUVS::strip_colors[barva_index].color_data[STRIP_BLUE];
		update_strip();
}

void AUVS::set_strip_brightness(uint8_t value)
{
	current_brightness = value;
	update_strip();
}