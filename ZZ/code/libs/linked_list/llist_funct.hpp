#include "task.h"


template <typename tip>
void LIST_t<tip>::pojdi_zacetek()
{
    while (glava != NULL && glava->prejsnji != NULL)
    {
        glava = glava->prejsnji;
    }
    glava_index = 0;
}
template <typename tip>
void LIST_t<tip>::pojdi_konec()
{
    while (glava != NULL && glava->naslednji != NULL)
    {
        glava = glava->naslednji;
    }
    if (count > 0)
        glava_index = count - 1;
}
template <typename tip>
void LIST_t<tip>::head_to_index(uint32_t index)
{
    while (glava_index > index)
    {
        glava = glava->prejsnji;
        glava_index--;
    }

    while (glava_index < index)
    {
        glava = glava->naslednji;
        glava_index++;
    }
}
template <typename tip>
unsigned short LIST_t<tip>::length()
{
    return count;
}
template <typename tip>
void LIST_t<tip>::add_front(tip vrednost)
{
	#if USE_FREERTOS == 1
		vpdt *nov = (vpdt *)pvPortMalloc(sizeof(vpdt));
	#else	
		vpdt *nov = (vpdt *)malloc(sizeof(vpdt));
	#endif
    pojdi_zacetek();

    if (glava != NULL)
    {
        glava->prejsnji = nov;
    }
    nov->prejsnji = NULL;
    nov->naslednji = glava;
    nov->podatek = vrednost;
    glava = nov;
    count++;
    glava_index = 0;
}
template <typename tip>
void LIST_t<tip>::add_end(tip vrednost)
{

	#if USE_FREERTOS == 1
		vpdt *nov = (vpdt *)pvPortMalloc(sizeof(vpdt));
	#else
		vpdt *nov = (vpdt *)malloc(sizeof(vpdt));
	#endif
	
    pojdi_konec();
    if (glava != NULL)
    {
        glava->naslednji = nov;
    }
    nov->prejsnji = glava;
    nov->naslednji = NULL;
    nov->podatek = vrednost;
    glava = nov;
    count++;
    glava_index = count - 1;

}
template <typename tip>
tip LIST_t<tip>::pop_end()
{
    tip return_data;
    pojdi_konec();
    return_data = glava->podatek;
    vpdt *prev = glava->prejsnji;
    if (prev != NULL)
        prev->naslednji = NULL;

	#if USE_FREERTOS == 1
		pvPortFree(glava);
	#else
	    free(glava);
	#endif

    glava = prev;
    count--;
    glava_index--;
    
    return return_data;
}

#if (INCLUDE_SORT == 1)
template <typename tip>
void LIST_t<tip>::sort(tip (*comparator_fnct)(tip, tip))
{
    tip temp;
    for (uint32_t i = 0; i < count - 1;)
    {
        if (comparator_fnct((*this)[i], (*this)[i + 1]) > 0)
        {
            temp = (*this)[i];
            (*this)[i] = (*this)[i + 1];
            (*this)[i + 1] = temp;
            if (i > 0)
                i--;
        }
        else
            i++;
    }
}
#endif

#if (INCLUDE_IOSTREAM == 1)
template <typename tip>
void LIST_t<tip>::print_console()
{
    pojdi_zacetek();
    for (uint32_t i = 0; i < count; i++)
    {
        using namespace std;
        cout << (*this)[i] << endl;
    }
}
#endif
template <typename tip>
void LIST_t<tip>::remove_by_index(uint32_t index)
{
    head_to_index(index);
    vpdt *new_head;

    if (glava->naslednji != NULL)
        glava->naslednji->prejsnji = glava->prejsnji;

    if (glava->prejsnji != NULL)
        glava->prejsnji->naslednji = glava->naslednji;

    if (glava->naslednji != NULL)
    {
        new_head = glava->naslednji;
    }
    else
    {
        new_head = glava->prejsnji;
        glava_index--;
    }


    free(glava);


    glava = new_head;
    count--;
}
template <typename tip>
void LIST_t<tip>::splice(uint32_t index, uint32_t num_to_remove)
{
    while (num_to_remove > 0)
    {
        remove_by_index(index);
        num_to_remove--;
    }
}




/*************************************************/
#if (USE_OPERATORS == 1)
template <typename tip>
tip &LIST_t<tip>::operator[](unsigned long index)
{
    
    head_to_index(index);
    
    return (glava->podatek);
}

template <typename tip, class cl>
LIST_t<tip> operator+(tip pod, cl obj)
{
    obj.add_front(pod);
    return obj;
}

template <typename tip>
LIST_t<tip> LIST_t<tip>::operator+(tip pod)
{
    this->add_end(pod);
    return (*this);
}

template <typename tip>
void LIST_t<tip>::operator+=(tip pod)
{
    this->add_end(pod);
}



#endif
