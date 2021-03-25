#ifndef barvna_lista
#define barvna_lista

enum barve_t
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

	struct mozne_barve_t
	{
		unsigned char BELA[3] = {255, 255, 255};
		unsigned char ZELENA[3] = {0, 255, 0};
		unsigned char RDECA[3] = {255, 0, 0};
		unsigned char MODRA[3] = {0, 0, 255};
		unsigned char RUMENA[3] = {255, 255, 0};
		unsigned char SVETLO_MODRA[3] = {0, 255, 255};
		unsigned char VIJOLICNA[3] = {255, 0, 255};
		unsigned char ROZA[3] = {255, 20, 147};
		unsigned char *barvni_ptr[8] = {this->BELA, this->ZELENA, this->RDECA, this->MODRA, this->RUMENA, this->SVETLO_MODRA, this->VIJOLICNA, this->ROZA};
	};
extern mozne_barve_t mozne_barve;
	
#endif
