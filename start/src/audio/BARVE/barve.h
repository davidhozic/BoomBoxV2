enum barve
{
    BELA,
    ZELENA,
    RDECA,
    MODRA,
    RUMENA,
    MORSKO_MODRA,
    VIJOLICNA,
    ROZA,
    LENGHT
};

struct c
{
    static int BELA[3];
    static int ZELENA[3];
    static int RDECA[3];
    static int MODRA[3];
    static int RUMENA[3];
    static int MORSKO_MODRA[3];
    static int VIJOLICNA[3];
    static int ROZA[3];
    int *barvni_ptr[8] = {BELA, ZELENA, RDECA, MODRA, RUMENA, MORSKO_MODRA, VIJOLICNA, ROZA};
};
extern c mozne_barve;