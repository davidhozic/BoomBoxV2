#include "audio.hpp"

STRIP_COLOR_t AUVS::strip_colors[] =
{
	{ COLOR_WHITE, 	    {255, 255, 255}  },
	{ COLOR_ZELENA,     {0, 255, 0}	     },
	{ COLOR_RED, 	    {255, 0, 0} 	 },
	{ COLOR_BLUE,	    {0,0, 255} 	     },
	{ COLOR_YELLOW ,    {255, 255, 0}    },
	{ COLOR_LBLUE,      {0, 255, 255}	 },
	{ COLOR_VIOLET,     {255, 0, 255} 	 },
	{ COLOR_PINK, 	    {255, 20, 100}	 },
    { COLOR_TERMINATOR, {0, 0, 0}        }
};


/* Index of array must match the animation index(inside structure) */
STRIP_ANIMATION_t AUVS::strip_animations[] = 
{ 
	{ AUVS_AN_NORMAL_FADE,    normal_fade_task   },
 	{ AUVS_AN_INVERTED_FADE,  inverted_fade_task },
 	{ AUVS_AN_BREATHE_FADE,   breathe_fade_task  },
};