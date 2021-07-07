

/********************************************************************************************/
/*                                      STRUCTURES                                          */
/********************************************************************************************/
struct SETTINGS_UI_MENU_LIST
{
    uint8_t index;
    uint8_t state;
};

/********************************************************************************************/
/*                                         ENUMS                                            */
/********************************************************************************************/

enum SETTINGS_UI_KEY_EVENT
{
	SU_KEY_CLEAR = 0,
	SU_KEY_SHORT_PRESS,
	SU_KEY_LONG_PRESS,
	SU_KEY_IGNORE_KEY
};

/*********************************************************************************************/
/*											PROTOS											 */
/*********************************************************************************************/
void zaslon();
void settings_UI();
void exit_scroll();
inline void showSEEK(SETTINGS_UI_MENU_LIST element);
void enter_scroll();
/************************/
/*         MACROS       */
/************************/

#define SU_NEXT(index, menu_table)  index + 1 >= sizeof(menu_table)/sizeof(menu_table[0])  ?  index = 0 :  index++



