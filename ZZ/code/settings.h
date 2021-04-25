
#include <avr/io.h>


#ifndef SETTINGS_HH
#define SETTINGS_HH
/************************************************************************/
/*								EEPROM IDs                              */
/************************************************************************/
/**/    #define battery_eeprom_addr			0						  /**/
/**/    #define strip_mode_addr				1						  /**/
/************************************************************************/


/************************************************************************/
/*					        VOLTAGE CONSTANTS					        */
/************************************************************************/

#define adc_milivolt_ref				   5000
#define adc_volt_ref					   5.00f

/************************************************************************/                             
/*							  ADC PIN MACROS               		        */
/************************************************************************/    
/**/	#define mic_pin						0	//ADC				  /**/
/**/    #define vDIV_pin					2	//ADC				  /**/
/**/	#define mic_ref_pin					3	//ADC				  /**/
/************************************************************************/


/************************************************************************/
/*						  DIGITAL PINS & PORTS                          */
/************************************************************************/
    
	#define BAT_LCD_pin						0
	#define BAT_LCD_port				   'E'

	#define _21VPSU_pin						1
	#define  _21VPSU_port				   'K'

    #define main_mosfet_pin					3			
	#define main_mosfet_port			   'H'	

    #define menjalnik_pin					4	
	#define menjalnik_port			       'H'	
	
	#define bat_charge_pin					5			
	#define bat_charge_port				   'H'	
	
	#define red_button_pin					5
	#define red_button_port				   'G'

	#define _12V_line_pin					7			
	#define  _12V_line_port				   'B'
	
	#define strip_red_pin					4 
	#define strip_green_pin					5
	#define strip_blue_pin					6
	#define strip_port					   'B'




/************************************************************************/
/*								OTHER                                   */
/************************************************************************/
    #define sleep_voltage				  3100   
	#define F_CPU					   16000000UL  

 
#endif