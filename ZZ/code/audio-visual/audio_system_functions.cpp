#include "global.hh"
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
	for (uint8_t i = 0; i < 5; i++)
	{
		brightDOWN (FAST_ANIMATION_TIME_MS);
		brightUP   (FAST_ANIMATION_TIME_MS);
	}
}

void class_AUDIO_SYS::colorSHIFT(uint8_t BARVA, unsigned short animation_time)
{
	char smer[3];
	while ( m_audio_system.current_color[RED]	 != m_mozne_barve.barvni_ptr[BARVA][RED] ||
			m_audio_system.current_color[GREEN]  != m_mozne_barve.barvni_ptr[BARVA][GREEN] ||
			m_audio_system.current_color[BLUE]	 != m_mozne_barve.barvni_ptr[BARVA][BLUE]		)	// While current colors different from wanted
	{
		m_mozne_barve.barvni_ptr[BARVA][RED] >= m_audio_system.current_color[RED]	  ? smer[RED]	= 1 : smer[RED]	  = -1;
		m_mozne_barve.barvni_ptr[BARVA][GREEN] >= m_audio_system.current_color[GREEN] ? smer[GREEN] = 1 : smer[GREEN] = -1;
		m_mozne_barve.barvni_ptr[BARVA][BLUE] >= m_audio_system.current_color[BLUE]  ? smer[BLUE]  = 1 : smer[BLUE]  = -1;

		m_audio_system.current_color[RED]	+=	(6 * smer[RED]);
		m_audio_system.current_color[GREEN]	+=	(6 * smer[GREEN]);
		m_audio_system.current_color[BLUE]	+=	(6 * smer[BLUE]);

		//Preveri prenihaj:

		m_audio_system.current_color[RED] = (smer[RED] == 1 && m_audio_system.current_color[RED] > m_mozne_barve.barvni_ptr[BARVA][RED]) ||
		(smer[RED] == -1 && m_audio_system.current_color[RED] < m_mozne_barve.barvni_ptr[BARVA][RED]) ? m_mozne_barve.barvni_ptr[BARVA][RED]: m_audio_system.current_color[RED] ;	//Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
		
		m_audio_system.current_color[GREEN] = (smer[GREEN] == 1 && m_audio_system.current_color[GREEN] > m_mozne_barve.barvni_ptr[BARVA][GREEN]) ||
		(smer[GREEN] == -1 && m_audio_system.current_color[GREEN] < m_mozne_barve.barvni_ptr[BARVA][GREEN]) ? m_mozne_barve.barvni_ptr[BARVA][GREEN]: m_audio_system.current_color[GREEN] ;
		
		m_audio_system.current_color[BLUE] = (smer[BLUE] == 1 && m_audio_system.current_color[BLUE] > m_mozne_barve.barvni_ptr[BARVA][BLUE]) ||
		(smer[BLUE] == -1 && m_audio_system.current_color[BLUE] < m_mozne_barve.barvni_ptr[BARVA][BLUE]) ? m_mozne_barve.barvni_ptr[BARVA][BLUE]: m_audio_system.current_color[BLUE] ;

		updateSTRIP();
		
		delayFREERTOS(  (animation_time*6)/255  );
	}
}

void class_AUDIO_SYS::brightnessFADE(char smer, unsigned short animation_time)
{
	while (smer > 0 ? m_audio_system.current_brightness < 255 : m_audio_system.current_brightness > 0)
	{
		m_audio_system.current_brightness += 6 * smer;
		m_audio_system.current_brightness = m_audio_system.current_brightness < 0 ? 0 : m_audio_system.current_brightness;
		m_audio_system.current_brightness = m_audio_system.current_brightness > 255 ? 255 : m_audio_system.current_brightness;
		updateSTRIP();
		delayFREERTOS(  (animation_time*6)/255  );
	}
}


void class_AUDIO_SYS::stripOFF()
{
	strip_mode = STRIP_OFF;
	deleteTASK(&handle_average_volume);
	deleteTASK(&handle_active_strip_mode);
	delayFREERTOS(10);
	m_audio_system.animation_time = SLOW_ANIMATION_TIME_MS;
	brightDOWN(SLOW_ANIMATION_TIME_MS);
}

void class_AUDIO_SYS::stripON()
{
	xTaskCreate(signal_measure, "avg_vol", 128, NULL, 3, &handle_average_volume);
	strip_mode = EEPROM.beri(EEPROM_ADDRESS_STRIP_MODE);
	
	/* EEPROM address is empty */
	if (strip_mode == 0xFF)
	{
		strip_mode = 0;
	}   

	m_audio_system.animation_time = NORMAL_ANIMATION_TIME_MS;
	delayFREERTOS(10);
}