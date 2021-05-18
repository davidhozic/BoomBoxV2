

#ifndef SETTINGS_H
#define SETTINGS_H


/************************************************************************/
/*					 	   AUDIO SYSTEM SETTINGS                       */
/************************************************************************/
	#define  NORMAL_ANIMATION_TIME_MS		500		// Animation time for mic detection light up
	#define	 SLOW_ANIMATION_TIME_MS			1500	// Animation time for exit animations and settings ui showcase
/************************************************************************/
/*								EEPROM IDs                              */
/************************************************************************/
    #define EEPROM_ADDRESS_BATTERY_CHARGING_STATUS		0x0		  
    #define EEPROM_ADDRESS_STRIP_MODE					0x1		  
/************************************************************************/


/************************************************************************/
/*					        VOLTAGE SETTINGS					        */
/************************************************************************/
	#define adc_milivolt_ref							5000	  
	#define adc_volt_ref								5.00	  
	#define min_battery_voltage							3000L	  
	#define max_battery_voltage							4150L	  
	#define polnilna_histereza							10		  // When charging finishes, the speaker needs to discharge for atleast this percent
/************************************************************************/

/************************************************************************/                             
/*							  ADC PIN MACROS               		        */
/************************************************************************/    
	#define mic_pin						0	//ADC				  
    #define vDIV_pin					2	//ADC				  
/************************************************************************/


/************************************************************************/
/*						  DIGITAL PINS & PORTS                          */
/************************************************************************/
	#define BAT_LCD_pin						0
	#define BAT_LCD_port				   'E'

	#define _21VPSU_pin						0
	#define  _21VPSU_port				   'K'
	
	#define	main_power_switch_pin		    1
	#define main_power_switch_port		   'K'
		
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
	#define	 F_CPU						 16000000UL  
#endif