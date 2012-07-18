#ifndef CFBYTEARRAY_H
#define CFBYTEARRAY_H

#include "cfrefcount.h"

struct CFByteArray {
    char *m_array;
    unsigned int m_length;
    unsigned int m_used;
    CFRefCount *m_refcount;
};
typedef struct CFByteArray CFByteArray;

void CFByteArrayInit(CFByteArray **array);
void CFByteArrayDestroy(CFByteArray **array);
int CFByteArrayCopy(CFByteArray *source, CFByteArray **destination);
bool CFByteArrayEqual(CFByteArray *array1, CFByteArray *array2);
int CFByteArraySet(CFByteArray *array, void *bytes, unsigned int length);
int CFByteArrayAppend(CFByteArray *array, void *bytes, unsigned int length);
int CFByteArrayClear(CFByteArray *array);

#endif // CFBYTEARRAY_H
