#include "global.hpp"
#include "libs/EEPROM/EEPROM.hh"
#include "includes/audio.hh"
#include "libs/outputs_inputs/outputs_inputs.hh"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/



void class_AUDIO_SYS::updateSTRIP()
{
	writePWM(strip_red_pin, strip_port,		m_audio_system.current_color[RED]	*	m_audio_system.current_brightness/255.00);
	writePWM(strip_green_pin, strip_port,	m_audio_system.current_color[GREEN] *	m_audio_system.current_brightness/255.00);
	writePWM(strip_blue_pin, strip_port,	m_audio_system.current_color[BLUE]	*	m_audio_system.current_brightness/255.00);
}

void class_AUDIO_SYS::flashSTRIP() //Utripanje (Izhod iz STATE_SCROLL stata / menjava mikrofona)
{
	select_strip_color(BELA);
	for (uint8_t i = 0; i < 5; i++)
	{
		current_brightness = 0;
		updateSTRIP();
		delay_FreeRTOS_ms(125);
		current_brightness = 255;
		updateSTRIP();
		delay_FreeRTOS_ms(125);
	}
}

void class_AUDIO_SYS::colorSHIFT(uint8_t BARVA, unsigned short animation_time)
{
	char smer[3];
	m_audio_system.current_brightness = 255;
	while ( m_audio_system.current_color[RED]	 != m_mozne_barve.barvni_ptr[BARVA][RED] ||
			m_audio_system.current_color[GREEN]  != m_mozne_barve.barvni_ptr[BARVA][GREEN] ||
			m_audio_system.current_color[BLUE]	 != m_mozne_barve.barvni_ptr[BARVA][BLUE]		)	// While current colors different from wanted
	{
		m_mozne_barve.barvni_ptr[BARVA][RED] >= m_audio_system.current_color[RED]	  ? smer[RED]	= 1 : smer[RED]	  = -1;
		m_mozne_barve.barvni_ptr[BARVA][GREEN] >= m_audio_system.current_color[GREEN] ? smer[GREEN] = 1 : smer[GREEN] = -1;
		m_mozne_barve.barvni_ptr[BARVA][BLUE] >= m_audio_system.current_color[BLUE]  ? smer[BLUE]  = 1 : smer[BLUE]  = -1;

		m_audio_system.current_color[RED]	+=	(AUVSYS_CONFIG_BRIGHTNESS_CHANGE * smer[RED]);
		m_audio_system.current_color[GREEN]	+=	(AUVSYS_CONFIG_BRIGHTNESS_CHANGE * smer[GREEN]);
		m_audio_system.current_color[BLUE]	+=	(AUVSYS_CONFIG_BRIGHTNESS_CHANGE * smer[BLUE]);

		//Preveri prenihaj:

		m_audio_system.current_color[RED] = (smer[RED] == 1 && m_audio_system.current_color[RED] > m_mozne_barve.barvni_ptr[BARVA][RED]) ||
		(smer[RED] == -1 && m_audio_system.current_color[RED] < m_mozne_barve.barvni_ptr[BARVA][RED]) ? m_mozne_barve.barvni_ptr[BARVA][RED]: m_audio_system.current_color[RED] ;	//Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
		
		m_audio_system.current_color[GREEN] = (smer[GREEN] == 1 && m_audio_system.current_color[GREEN] > m_mozne_barve.barvni_ptr[BARVA][GREEN]) ||
		(smer[GREEN] == -1 && m_audio_system.current_color[GREEN] < m_mozne_barve.barvni_ptr[BARVA][GREEN]) ? m_mozne_barve.barvni_ptr[BARVA][GREEN]: m_audio_system.current_color[GREEN] ;
		
		m_audio_system.current_color[BLUE] = (smer[BLUE] == 1 && m_audio_system.current_color[BLUE] > m_mozne_barve.barvni_ptr[BARVA][BLUE]) ||
		(smer[BLUE] == -1 && m_audio_system.current_color[BLUE] < m_mozne_barve.barvni_ptr[BARVA][BLUE]) ? m_mozne_barve.barvni_ptr[BARVA][BLUE]: m_audio_system.current_color[BLUE] ;

		updateSTRIP();
		
		delay_FreeRTOS_ms(  (animation_time*AUVSYS_CONFIG_BRIGHTNESS_CHANGE)/255  );
	}
}

void class_AUDIO_SYS::brightnessFADE(char smer, unsigned short animation_time)
{
	while (smer > 0 ? m_audio_system.current_brightness < 255 : m_audio_system.current_brightness > 0)
	{
		m_audio_system.current_brightness += AUVSYS_CONFIG_BRIGHTNESS_CHANGE * smer;
		m_audio_system.current_brightness = m_audio_system.current_brightness < 0 ? 0 : m_audio_system.current_brightness;
		m_audio_system.current_brightness = m_audio_system.current_brightness > 255 ? 255 : m_audio_system.current_brightness;
		updateSTRIP();
		delay_FreeRTOS_ms(  (animation_time*AUVSYS_CONFIG_BRIGHTNESS_CHANGE)/255  );
	}
}


void class_AUDIO_SYS::stripOFF()
{
	strip_mode = STRIP_OFF;
	deleteTASK(&handle_audio_meass);
	deleteTASK(&handle_active_strip_mode);
	delay_FreeRTOS_ms(10);
	m_audio_system.animation_time = AUVSYS_CONFIG_SLOW_ANIMATION_TIME_MS;
	brightDOWN(AUVSYS_CONFIG_SLOW_ANIMATION_TIME_MS);
}

void class_AUDIO_SYS::stripON()
{
	xTaskCreate(signal_measure, "avg_vol", 128, NULL, 3, &handle_audio_meass);
	strip_mode = EEPROM.beri(EEPROM_ADDRESS_STRIP_MODE);
	
	/* EEPROM address is empty */
	if (strip_mode == 0xFF)
	{
		strip_mode = 0;
	}   

	m_audio_system.animation_time = AUVSYS_CONFIG_NORMAL_ANIMATION_TIME_MS;
	delay_FreeRTOS_ms(10);
}