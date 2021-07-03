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
    unsigned long count = 0;
    unsigned long glava_index = 0;

    void pojdi_zacetek();
    void pojdi_konec();
    void head_to_index(uint32_t index);
   
public:

    unsigned short length();
	void add_front(tip vrednost);
    void add_end(tip vrednost);
    tip pop_end();
    
    ~LIST_t<tip>()
    {
        splice(0, length());
    }

#if (INCLUDE_SORT == 1)
    void sort(int (*comparator_fnct)(tip , tip));
#endif


#if (INCLUDE_IOSTREAM == 1)
    void print_console();
#endif

    void remove_by_index(uint32_t index);
    void splice(uint32_t index, uint32_t num_to_remove);


    /*************************************************/
#if (USE_OPERATORS == 1)

    tip &operator[](unsigned long index);
   
    LIST_t<tip> operator+(tip pod);
    
    void operator+=(tip pod);

	
#endif

};
    


#include "llist_funct.hpp"

#endif

