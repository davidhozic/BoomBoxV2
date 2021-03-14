
#include <avr/io.h>


#ifndef SETTINGS_HH
#define SETTINGS_HH
/******************************************************                                   
 *                  EEPROM ID                         *
******************************************************/
/**/ #define battery_eeprom_addr   1               /**/
/**/ #define audiomode_eeprom_addr 0               /**/
/*****************************************************/



/*********************************************************                                  
*                        SPLOSNO                         *
*********************************************************/
/**/     #define SHRANI_AUDVS_MODE   1                /**/
/**/     #define vDIV_pin            2               /**/
         #define Temp_sensor_pin     1   
/**/     #define mic_pin             0               /**/
		 #define mic_ref_pin      	 3
/**/     #define sleep_voltage      3300              /**/
         #define main_mosfet_pin     3 //PH
         #define menjalnik_pin       4 //PH
         #define btr_chr_pin         5 //PH
         #define _12V_line           7 //PB
         #define BAT_LCD_pin         0 //PE
/********************************************************/




#endif