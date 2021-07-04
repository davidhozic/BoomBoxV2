

#ifndef SETTINGS_H
#define SETTINGS_H



/************************************************************************/
/*								EEPROM IDs                              */
/************************************************************************/
    #define GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS		0x0		  
    #define GLOBAL_CFG_EEPROM_ADDR_STRIP_MODE					0x1		  
/************************************************************************/


/************************************************************************/
/*					        POWER SETTINGS  					        */
/************************************************************************/
	#define POWER_CFG_ADC_REF_mV							        5000	  
	#define POWER_CFG_MIN_BATTERY_VOLTAGE							3200	  
	#define POWER_CFG_MAX_BATTERY_VOLTAGE							4150	  
	#define POWER_CFG_CHARGE_HYSTERESIS_PERCENT 					5		// When charging finishes, the speaker needs to discharge for atleast this percent
    #define POWER_CFG_SLEEP_DELAY_MS                                2000
    #define POWER_CFG_VOLTAGE_READ_PERIOD_MS                        500
/************************************************************************/

/************************************************************************/
/*					       USER UI SETTINGS  					        */
/************************************************************************/
    #define SU_CFG_AUTO_EXIT_SCROLL_PERIOD			                15000
    #define SU_CFG_LONG_PRESS_PERIOD				                1000
    #define SU_CFG_SHORT_PRESS_PERIOD				                500
/************************************************************************/


/************************************************************************/
/*					  AUDIO VISUAL SYSTEM SETTINGS  				    */
/************************************************************************/
/*  STRIP ANIMATION_CONFIG  */
#define AUVS_CFG_NORMAL_ANIMATION_TIME_MS		                    500		// Animation time for mic detection light up
#define AUVS_CFG_FAST_ANIMATION_TIME_MS				                250		// Animation time used in flash strip
#define	AUVS_CFG_SLOW_ANIMATION_TIME_MS				                1000	// Animation time for exit animations and settings ui showcase
#define AUVS_CFG_BRIGHTNESS_CHANGE					                5
#define AUVS_CFG_COLOR_CHANGE                                       5
/*   MEASSUREMENT CONFIG   */          
#define	AUVS_CFG_MEASS_MAX_READINGS	                                50
#define AUVS_CFG_MEASS_LOG_PERIOD_MS	                            5 
#define	AUVS_CFG_MEASS_MIC_TRIGGER_PERCENT			                0.20
#define AUVS_CFG_MEASS_FILTER_TIME_MS                               340
/************************************************************************/


/************************************************************************/                             
/*							  ADC PIN MACROS               		        */
/************************************************************************/    
	#define GLOBAL_CFG_PIN_MICROPHONE				0	//ADC				  
    #define GLOBAL_CFG_PIN_VOLTAGE_DIV				2	//ADC				  
/************************************************************************/


/************************************************************************/
/*						  DIGITAL PINS & PORTS                          */
/************************************************************************/
	#define GLOBAL_CFG_PIN_BATTERY_LCD						0
	#define GLOBAL_CFG_PORT_BATTERY_LCD				       'E'

	#define GLOBAL_CFG_PIN_21V_PSU						    0
	#define GLOBAL_CFG_PORT_21V_PSU				           'K'
	
	#define	GLOBAL_CFG_PIN_MAIN_POWER_SWITCH			    1
	#define GLOBAL_CFG_PORT_MAIN_POWER_SWITCH		       'K'
		
	#define GLOBAL_CFG_PIN_OUTPUT_MOSFET					3			
	#define GLOBAL_CFG_PORT_OUTPUT_MOSFET			       'H'	

	#define GLOBAL_CFG_PIN_MENJALNIK				       	4	
	#define GLOBAL_CFG_PORT_MENJALNIK			           'H'	
	
	#define GLOBAL_CFG_PIN_CHARGE					        5			
	#define GLOBAL_CFG_PORT_CHARGE			        	   'H'	
	
	#define GLOBAL_CFG_PIN_SU_SWITCH				        5
	#define GLOBAL_CFG_PORT_SU_SWITCH				       'G'

	#define  GLOBAL_CFG_PIN_12V_LINE				        7			
	#define  GLOBAL_CFG_PORT_12V_LINE				       'B'
	
	#define GLOBAL_CFG_PIN_LED_STRIP_R				    	4 
	#define GLOBAL_CFG_PIN_LED_STRIP_G				    	5
	#define GLOBAL_CFG_PIN_LED_STRIP_B					    6
	#define GLOBAL_CFG_PORT_LED_STRIP					   'B'
/************************************************************************/
/*								TASK RELATED                            */
/************************************************************************/

    #define TASK_CFG_TASK_DEFAULT_STACK                   256

#endif
