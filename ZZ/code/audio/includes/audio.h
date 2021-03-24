
#ifndef AUDIO_H
#define AUDIO_H
#include "../BARVE/barve.h"
#include "FreeRTOS.h"
#include "task.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "settings.h"
/*********************************/
/*             Makro             */
/*********************************/

#define tr_r Audio_vars.TR_BARVA[0]
#define tr_z Audio_vars.TR_BARVA[1]
#define tr_m Audio_vars.TR_BARVA[2]
#define tr_bright Audio_vars.tr_svetlost
#define trenutni_audio_mode Audio_vars.STRIP_MODE
#define brightUP(cas_na_krog) svetlost_mod_funct(1, cas_na_krog);
#define brightDOWN(cas_na_krog) svetlost_mod_funct(-1, cas_na_krog);
#define colorSHIFT(index_barve) color_fade_funct((uint8_t *)index_barve);
#define cr_fade_tsk(funct, name, barv, control) \
    deleteTask(control);                        \
    xTaskCreate(funct, name, 128, &barv, 3, &control);

#define turnOFFstrip()              \
    holdTASK(audio_system_control); \
    deleteALL_subAUDIO_tasks();     \
    brightDOWN(15);
#define nastavi_barve(x)                               \
    tr_r = mozne_barve.barvni_ptr[*((uint8_t *)x)][0]; \
    tr_z = mozne_barve.barvni_ptr[*((uint8_t *)x)][1]; \
    tr_m = mozne_barve.barvni_ptr[*((uint8_t *)x)][2];
/********************************************************************/


/*********************************************/
/*			  TASK CONTROL FREERTOS          */
/*********************************************/

extern TaskHandle_t fade_control;
extern TaskHandle_t color_fade_control;
extern TaskHandle_t Breathe_control;

/*********************************************/
/*			 ENUM,STRUCT DEFINICIJE          */
/*********************************************/

enum strip_mode_enum_t
{
	NORMAL_FADE,
	COLOR_FADE,
	Fade_Breathe,
	LENGTH_2,
	OFF_A
};

enum mic_mode_enum_t{
	AVG_VOL,
	POTENCIOMETER,
	mic_enum_len
};

 typedef struct 
{
	unsigned short STRIP_MODE;
	short TR_BARVA[3];
	short tr_svetlost;
	unsigned short MIC_MODE;
	unsigned char zrebana_barva;
	unsigned short Average_vol;
}audio_t;

extern audio_t Audio_vars;


/*********************************************/
/*         Prototipi pomoznih funkcij        */
/*********************************************/
void holdALL_tasks();
void writeTRAK();
void color_fade_funct(uint8_t *BARVA);
void svetlost_mod_funct(char smer, uint8_t cas_krog);
void mic_mode_change();
void strip_mode_chg(char *ch);
void deleteALL_subAUDIO_tasks();
void flash_strip();
/*********************************************/

/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void fade_task(void *B);
void Color_Fade_task(void *B);
void Fade_Breathe_task(void *B);
void avg_vol_task(void* param);
/*********************************************/



#endif
