
#include <avr/io.h>


#ifndef SETTINGS_HH
#define SETTINGS_HH
/************************************************************************/
/*								EEPROM IDs                              */
/************************************************************************/
/**/    #define battery_eeprom_addr			0						  /**/
/**/    #define audiomode_eeprom_addr	    1						  /**/
/************************************************************************/



/************************************************************************/                             
/*							  ADC PIN MACROS               		        */
/************************************************************************/    
/**/	#define mic_pin						0	//ADC				  /**/
/**/    #define Temp_sensor_pin				1	//ADC				  /**/	
/**/    #define vDIV_pin					2	//ADC				  /**/
/**/	#define mic_ref_pin					3	//ADC				  /**/
/************************************************************************/


/************************************************************************/
/*						  DIGITAL PINS & PORTS                          */
/************************************************************************/    

    #define main_mosfet_pin					3			
	#define main_mosfet_port			   'H'	

    #define menjalnik_pin					4	
	#define menjalnik_port			       'H'	
	
	#define bat_charge_pin					5			
	#define bat_charge_port				   'H'	

	#define _12V_line_pin					7			
	#define  _12V_line_port				   'B'
	
	#define BAT_LCD_pin						0		
	#define BAT_LCD_port				   'E'

	#define _21VPSU_pin						1			
	#define  _21VPSU_port				   'K'

	#define red_button_pin					5
	#define red_button_port				   'G'

	#define r_trak 4 
	#define z_trak 5
	#define m_trak 6
	#define strip_port					   'B'

/************************************************************************/
/*								OTHER                                   */
/************************************************************************/
    #define save_strip_mode				     1    
    #define sleep_voltage				   3200   
	#define F_CPU						16000000UL  

 
#endif