#include <stdlib.h>
#include "cfdatadefs.h"
#include "cfbytearray.h"
#include "cfrefcount.h"

void CFByteArrayInit(CFByteArray **array)
{
    if (!array)
        return;
    *array = (CFByteArray *)malloc(sizeof(CFByteArray));
    (*array)->m_array = (char *)malloc(1024);
    (*array)->m_length = 1024;
    (*array)->m_used = 0;
    CFRefCountInit(&(*array)->m_refcount);
    CFRefCountAttach((*array)->m_refcount, (*array));
}

void CFByteArrayDestroy(CFByteArray **array)
{
    int elements = 0;
    if (!array)
        return;
    elements = CFRefCountDetach((*array)->m_refcount, (*array));
    if (elements == 0) {
        // If we are the only ones holding the array we can delete it
        free((*array)->m_array);
        // The refcount element is not needed anymore
        CFRefCountDestroy(&(*array)->m_refcount);
    }
    free(*array);
    *array = NULL;
}

int CFByteArrayCopy(CFByteArray *source, CFByteArray **destination)
{
    // Basically we copy the link to the array and mark the attachment
    if (!source || !destination)
        return -1;
    *destination = (CFByteArray *)malloc(sizeof(CFByteArray));
    (*destination)->m_array = source->m_array;
    (*destination)->m_length = source->m_length;
    (*destination)->m_used = source->m_used;
    int elements = 0;
    elements = CFRefCountAttach(source->m_refcount, (*destination));
    if (elements < 0)
        return -1;
    (*destination)->m_refcount = source->m_refcount;
    return 0;
}

bool CFByteArrayEqual(CFByteArray *array1, CFByteArray *array2)
{
    if (!array1 || !array2)
        return false;
    /*
     * Rules for comparison:
     * 1. First check the refcount elements, if they are the same
     *  then the elements are the same.
     * 2. Check at the content.
     * Notice that the length of the array does not mean much, since
     * the arrays might be of different size but equal content.
     */
    if (CFRefCountisEqual(array1->m_refcount, array2->m_refcount))
        return true;
    if (array1->m_used == array2->m_used){
        char *p = NULL;
        char *q = NULL;
        unsigned int count = 0;
        for (p = array1->m_array, q = array2->m_array; p == q; p++, q++, count++)
            ;
        if ((count == array1->m_used) && (count == array2->m_used))
            return true;
    }
    return false;
}

int CFByteArraySet(CFByteArray *array, void *bytes, unsigned int length)
{
    if (!array || !bytes || (length <= 0))
        return -1;
    if (array->m_used + length >= array->m_length)
        return -1;
    if (CFRefCountisShared(array->m_refcount)) {
        CFRefCountDetach(array->m_refcount, array);
        array->m_used = 0;
        array->m_array = (char *)malloc(1024);
        array->m_length = 1024;
        array->m_refcount = NULL;
        CFRefCountInit(&array->m_refcount);
        CFRefCountAttach(array->m_refcount, array);
    }
    unsigned int i = 0;
    char *p = (char *)bytes;
    for (; i < length; i++) {
        array->m_array[i] = p[i];
    }
    array->m_used = length;
    return 0;
}

int CFByteArrayAppend(CFByteArray *array, void *bytes, unsigned int length)
{
    if (!array || !bytes)
        return -1;
    if (array->m_used + length >= array->m_length)
        return -1;
    if (CFRefCountisShared(array->m_refcount)) {
        CFRefCountDetach(array->m_refcount, array);
        CFRefCountInit(&array->m_refcount);
        CFRefCountAttach(array->m_refcount, array);
    }
    unsigned int i = 0;
    char *p = (char *)bytes;
    for (; i < length; i++) {
        array->m_array[i + array->m_used] = p[i];
    }
    array->m_used += length;
    return 0;
}

int CFByteArrayClear(CFByteArray *array)
{
    if (!array)
        return -1;
    if (CFRefCountisShared(array->m_refcount)) {
        CFRefCountDetach(array->m_refcount, array);
        CFRefCountInit(&array->m_refcount);
        CFRefCountAttach(array->m_refcount, array);
    }
    array->m_used = 0;
    return 0;
}

