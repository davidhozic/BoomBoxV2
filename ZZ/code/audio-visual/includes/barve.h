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
		unsigned char BELA[3];
		unsigned char ZELENA[3];
		unsigned char RDECA[3];
		unsigned char MODRA[3];
		unsigned char RUMENA[3];
		unsigned char SVETLO_MODRA[3];
		unsigned char VIJOLICNA[3];
		unsigned char ROZA[3];
		unsigned char *barvni_ptr[8];
	};
extern struct_MOZNE_BARVE mozne_barve;
	

