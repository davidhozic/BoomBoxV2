

template <typename tip>
class Vozlisce
{
private:

    class vozlisce_data_obj_t
    {
    public:
        friend class Vozlisce<tip>;

    private:
        vozlisce_data_obj_t *naslednji;
        vozlisce_data_obj_t *prejsnji;
        tip podatek;
    };

    vozlisce_data_obj_t *glava = NULL;
    unsigned short count = 0;
    
    inline void pojdi_zacetek()
    {
        while (glava != nullptr && glava->prejsnji != nullptr)
        {
            glava = glava->prejsnji;
        }
    }

    inline void pojdi_konec()
    {
        while (glava != nullptr && glava->naslednji != nullptr)
        {
            glava = glava->naslednji;
        }
    }

public:

    inline unsigned short length()
    {
        return count;
    }

    void dodaj_zacetek(tip vrednost)
    {
        vozlisce_data_obj_t *nov = (vozlisce_data_obj_t *) pvPortMalloc(sizeof(vozlisce_data_obj_t));
        pojdi_zacetek();

        if (glava != nullptr)
        {
            glava->prejsnji = nov;
        }
        nov->prejsnji = nullptr;
        nov->naslednji = glava;
        nov->podatek = vrednost;
        glava = nov;
        count++;
    }

    void dodaj_konec(tip vrednost)
    {
        vozlisce_data_obj_t *nov = (vozlisce_data_obj_t *) pvPortMalloc(sizeof(vozlisce_data_obj_t));

        pojdi_konec();
        if (glava != nullptr)
        {
            glava->naslednji = nov;
        }
        nov->prejsnji = glava;
        nov->naslednji = nullptr;
        nov->podatek = vrednost;
        glava = nov;
        count++;
    }

    tip &operator[](unsigned short index)
    {
		pojdi_zacetek();
        for (unsigned short ind = 0; ind < index; ind++)
        {
            glava = glava->naslednji;
        }
        return (glava->podatek);
    }
};