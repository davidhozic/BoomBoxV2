

#ifndef SETTINGS_H
#define SETTINGS_H



/************************************************************************/
/*								EEPROM IDs                              */
/************************************************************************/
    #define GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS		0x0		  
    #define GLOBAL_CFG_EEPROM_ADDR_STRIP_MODE					0x1		  
/************************************************************************/


/************************************************************************/
/*					        VOLTAGE SETTINGS					        */
/************************************************************************/
	#define GLOBAL_CFG_ADC_REF_mV							        5000	  
	#define GLOBAL_CFG_MIN_BATTERY_VOLTAGE							3200	  
	#define GLOBAL_CFG_MAX_BATTERY_VOLTAGE							4150	  
	#define GLOBAL_CFG_CHARGE_HYSTERESIS 							5		// When charging finishes, the speaker needs to discharge for atleast this percent
    #define GLOBAL_CFG_SLEEP_DELAY_MS                               2000
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

    #define GLOBAL_CFG_TASK_DEFAULT_STACK                   128


#endif
