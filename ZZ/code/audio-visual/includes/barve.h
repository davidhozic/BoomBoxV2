#pragma once

	enum enum_BARVE
	{
		BELA=0,
		ZELENA,
		RDECA,
		MODRA,
		RUMENA,
		SVETLO_MODRA,
		VIJOLICNA,
		ROZA,
		barve_end
	};

	struct struct_MOZNE_BARVE
	{
		unsigned char BELA[3]	 =		{255, 255, 255};
		unsigned char ZELENA[3]	 =		{0, 255, 0};
		unsigned char RDECA[3]	 =		{255, 0, 0};
		unsigned char MODRA[3]   =		{0, 0, 255};
		unsigned char RUMENA[3]  =		{255, 255, 0};
		unsigned char SVETLO_MODRA[3] =	{0, 255, 255};
		unsigned char VIJOLICNA[3] =	{255, 0, 255};
		unsigned char ROZA[3] =			{255, 20, 147};
		unsigned char *barvni_ptr[8]  = {BELA, ZELENA, RDECA, MODRA, RUMENA, SVETLO_MODRA, VIJOLICNA, ROZA};
	};
extern struct_MOZNE_BARVE mozne_barve;
	

