#if (USE_FREERTOS == 1)
    #include "task.h"
#endif


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

/**********************************************************************
 *  FUNCTION:    head_to_index()   
 *  PARAMETERS:  uint32_index
 *  DESCRIPTION: Moves the head of list to a specific index 
 *  RETURN:      void                                        
 **********************************************************************/

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


/**********************************************************************
 *  FUNCTION:    length()
 *  PARAMETERS:  void
 *  DESCRIPTION: Returns the number of elements in the list     
 *  RETURN:      void                                   
 **********************************************************************/
template <typename tip>
unsigned short LIST_t<tip>::length()
{
    return count;
}

/**********************************************************************
 *  FUNCTION:    add_front()
 *  PARAMETERS:  custom_type value
 *  DESCRIPTION: adds element to the front of the list  
 *  RETURN:      void               
 **********************************************************************/
template <typename tip>
void LIST_t<tip>::add_front(tip value)
{
	vpdt *nov;

	#if USE_FREERTOS == 1
	    nov = (vpdt *)pvPortMalloc(sizeof(vpdt));
	#else
	    nov = (vpdt *)malloc(sizeof(vpdt));
	#endif
	
    pojdi_zacetek();

    if (glava != NULL)
    {
        glava->prejsnji = nov;
    }
    nov->prejsnji = NULL;
    nov->naslednji = glava;
    nov->podatek = value;
    glava = nov;
    count++;
    glava_index = 0;
}


/**********************************************************************
 *  FUNCTION:    add_end()
 *  PARAMETERS:  custom_type value
 *  DESCRIPTION: adds element to the end of the list        
 *  RETURN:      void         
 **********************************************************************/
template <typename tip>
void LIST_t<tip>::add_end(tip value)
{
	vpdt *nov;

	#if USE_FREERTOS == 1
	    nov = (vpdt *)pvPortMalloc(sizeof(vpdt));
	#else
	    nov = (vpdt *)malloc(sizeof(vpdt));
	#endif
		
	
    pojdi_konec();
    if (glava != NULL)
    {
        glava->naslednji = nov;
    }
    nov->prejsnji = glava;
    nov->naslednji = NULL;
    nov->podatek = value;
    glava = nov;
    count++;
    glava_index = count - 1;

}


/**********************************************************************
 *  FUNCTION:    pop_end()
 *  PARAMETERS:  void
 *  DESCRIPTION: removes the last element and returns it
 *  RETURN:      Custom type element at the end of the list                     
 **********************************************************************/
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


/**********************************************************************
 *  FUNCTION:    sort()
 *  PARAMETERS:  int function_comparator_function
 *               (must return < 0 if element 1 is before element 2)
 *  DESCRIPTION: Sorts elements in the list                         
 *  RETURN:      void
 **********************************************************************/
#if (INCLUDE_SORT == 1)
template <typename tip>
void LIST_t<tip>::sort(int (*comparator_fnct)(tip, tip))
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


/**********************************************************************
 *  FUNCTION:    print_console()
 *  PARAMETERS:  void
 *  DESCRIPTION: Prints the contents of the list to the console 
 *               ( elements must be a string or a number )                          
 *  RETURN:      void
 **********************************************************************/
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


/**********************************************************************
 *  FUNCTION:    remove_by_index ()
 *  PARAMETERS:  uint32_t index of element to remove
 *  DESCRIPTION: Removes the element by index,  moves the contents on
 *               the right to the left                        
 *  RETURN:      void
 **********************************************************************/
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

    this->glava->podatek.~tip();  // Call the deconstructor in case data is another list 
    free(glava);


    glava = new_head;
    count--;
}


/**********************************************************************
 *  FUNCTION:    splice ()
 *  PARAMETERS:  uint32 start_index, uint32 number of elements to remove
 *  DESCRIPTION: Removes the specified number of elements, starting at                       
 *               start index  
 *  RETURN:      void
 **********************************************************************/
template <typename tip>
void LIST_t<tip>::splice(uint32_t start_index, uint32_t num_to_remove)
{
    while (num_to_remove > 0)
    {
        remove_by_index(start_index);
        num_to_remove--;
    }
}



/********************************************************************************************/
/*                                       OPERATORS                                          */    
/********************************************************************************************/

/**********************************************************************
 *  OPERATOR:    [] 
 *  PARAMETERS:  unsigned long index
 *  DESCRIPTION: returns the element at specific index by reference                    
 *  RETURN:      Reference return of custom type data at specific index
 **********************************************************************/

#if (USE_OPERATORS == 1)
template <typename tip>
tip &LIST_t<tip>::operator[](unsigned long index)
{
    
    head_to_index(index);
    
    return (glava->podatek);
}
/**********************************************************************
 *  OPERATOR:    +=
 *  PARAMETERS:  custom data
 *  DESCRIPTION: Adds data to the back of the list                   
 *  RETURN:      void
 **********************************************************************/
template <typename tip>
void LIST_t<tip>::operator+=(tip pod)
{
    this->add_end(pod);
}



#endif
