#ifndef SEZNAM_INC
#define SEZNAM_INC
#include <stdlib.h>
#include <stdint.h>
#include "list_settings.hpp"

#if (USE_FREERTOS == 1)
	#include "FreeRTOS.h"
#endif

#if (INCLUDE_IOSTREAM == 1)
    #include <iostream>
#endif


template <typename tip>
class LIST_t
{
private:

    struct vpdt
    {
        friend class LIST_t<tip>;
    private:
        vpdt *naslednji;
        vpdt *prejsnji;
        tip podatek;
        
    };

    vpdt *glava = NULL;
    uint32_t count = 0;
    uint32_t glava_index = 0;

    inline void pojdi_zacetek();
    inline void pojdi_konec();
    inline void head_to_index(uint32_t index);
    inline void menjaj_glava_naslednji();

public:

    uint32_t length();
	void add_front(tip vrednost);
    void add_end(tip vrednost);
    tip pop_end();

#if (INCLUDE_SORT == 1)
    void sort(int (*comparator_fnct)(tip , tip));
#endif


#if (INCLUDE_IOSTREAM == 1)
    void print_console();
#endif

    inline void remove_by_index(uint32_t index);
    void splice(uint32_t index, uint32_t num_to_remove);
    tip peek();
    void clone(LIST_t<tip> &src);
    tip pop(uint32_t index);
    /*************************************************/
#if (USE_OPERATORS == 1)

    tip &operator[](unsigned long index);
    void operator +=(tip pod);
#endif

};
    


#include "llist_funct.hpp"

#endif

