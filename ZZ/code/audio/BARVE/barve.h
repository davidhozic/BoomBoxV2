#ifndef barvna_lista
#define barvna_lista

enum barve_t
{
	BELA,
	ZELENA,
	RDECA,
	MODRA,
	RUMENA,
	SVETLO_MODRA,
	VIJOLICNA,
	ROZA,
	barve_end
};

	typedef struct 
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
	}mozne_barve_t;
	

	extern mozne_barve_t mozne_barve;


#endif
