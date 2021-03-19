#ifndef barvna_lista
#define barvna_list
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
	LENGHT
};

/*struct mozne_barve_t
{
const uint8_t BELA[3] = {255, 255, 255};
const uint8_t ZELENA[3] = {0, 255, 0};
const uint8_t RDECA[3] = {255, 0, 0};
const uint8_t MODRA[3] = {0, 0, 255};
const uint8_t RUMENA[3] = {255, 255, 0};
const uint8_t SVETLO_MODRA[3] = {0, 255, 255};
const uint8_t VIJOLICNA[3] = {255, 0, 255};
const uint8_t ROZA[3] = {255, 20, 147};
const uint8_t *barvni_ptr[8] = {BELA, ZELENA, RDECA, MODRA, RUMENA, SVETLO_MODRA, VIJOLICNA, ROZA};
};
extern mozne_barve_t mozne_barve;*/







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
