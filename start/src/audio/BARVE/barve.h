#ifndef barvna_lista
#define barvna_list
enum barve
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

struct c
{
    byte BELA[3] = {255, 255, 255};
    byte ZELENA[3] = {0, 255, 0};
    byte RDECA[3] = {255, 0, 0};
    byte MODRA[3] = {0, 0, 255};
    byte RUMENA[3] = {255, 255, 0};
    byte SVETLO_MODRA[3] = {0, 255, 255};
    byte VIJOLICNA[3] = {255, 0, 255};
    byte ROZA[3] = {255, 20, 147};
    byte *barvni_ptr[8] = {BELA, ZELENA, RDECA, MODRA, RUMENA, SVETLO_MODRA, VIJOLICNA, ROZA};
};
extern c mozne_barve;

#endif