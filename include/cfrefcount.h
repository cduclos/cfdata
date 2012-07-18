#ifndef CFREFCOUNT_H
#define CFREFCOUNT_H

#include "cfrefcount_p.h"

/*
 * Normally userCount will be one, since the structure will not be
 * shared. The moment we start sharing the structure the count becomes
 * two. If we try to write and the count is not one, then we need to
 * detach.
 */
struct CFRefCount {
    int m_userCount;
    CFRefCountNode *m_users;
    CFRefCountNode *m_last;
};
typedef struct CFRefCount CFRefCount;

void CFRefCountInit(CFRefCount ** ref);
void CFRefCountDestroy(CFRefCount ** ref);
int CFRefCountAttach(CFRefCount *ref, void *owner);
int CFRefCountDetach(CFRefCount *ref, void *owner);
bool CFRefCountisShared(CFRefCount *ref);
bool CFRefCountisEqual(CFRefCount *a, CFRefCount *b);

#endif //CFREFCOUNT
