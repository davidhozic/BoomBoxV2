/************************************************************/
					/*		 SETTINGS		*/

/* Timer will wait for AUVS_CFG_MEASS_FILTER_TIME_MS befor updating the input state after the state has changed (if the state goes back to unchanged, the timer auto resets) */
#define USE_FILTERING									1    
#define AUVS_CFG_MEASS_FILTER_TIME_MS									1

/* Included object methods */
#define INCLUDE_risen_edge                              1
#define INCLUDE_fallen_edge                             1
/************************************************************/