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

struct mozne_barve_t
{
    uint8_t BELA[3] = {255, 255, 255};
    uint8_t ZELENA[3] = {0, 255, 0};
    uint8_t RDECA[3] = {255, 0, 0};
    uint8_t MODRA[3] = {0, 0, 255};
    uint8_t RUMENA[3] = {255, 255, 0};
    uint8_t SVETLO_MODRA[3] = {0, 255, 255};
    uint8_t VIJOLICNA[3] = {255, 0, 255};
    uint8_t ROZA[3] = {255, 20, 147};
    uint8_t *barvni_ptr[8] = {BELA, ZELENA, RDECA, MODRA, RUMENA, SVETLO_MODRA, VIJOLICNA, ROZA};
};
extern mozne_barve_t mozne_barve;

#endif