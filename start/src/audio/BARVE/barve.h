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
    int BELA[3] = {255, 255, 255};
    int ZELENA[3] = {0, 255, 0};
    int RDECA[3] = {255, 0, 0};
    int MODRA[3] = {0, 0, 255};
    int RUMENA[3] = {255, 255, 0};
    int SVETLO_MODRA[3] = {0, 255, 255};
    int VIJOLICNA[3] = {255, 0, 255};
    int ROZA[3] = {255, 20, 147};
    int *barvni_ptr[8] = {BELA, ZELENA, RDECA, MODRA, RUMENA, SVETLO_MODRA, VIJOLICNA, ROZA};
};
extern c mozne_barve;