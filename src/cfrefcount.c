#include <stdlib.h>
#include "cfdatadefs.h"
#include "cfrefcount.h"
#include "cfrefcount_p.h"

void CFRefCountInit(CFRefCount **ref)
{
    if (!ref)
        return;
    *ref = (CFRefCount *)malloc(sizeof(CFRefCount));
    (*ref)->m_userCount = 0;
    (*ref)->m_users = NULL;
    (*ref)->m_last = NULL;
}

void CFRefCountDestroy(CFRefCount **ref)
{
    if (ref && *ref) {
        // Destroy the list of users!
        free(*ref);
        *ref = NULL;
    }
}

int CFRefCountAttach(CFRefCount *ref, void *owner)
{
    if (!ref || !owner)
        return -1;
    ref->m_userCount++;
    CFRefCountNode *node = (CFRefCountNode *)malloc(sizeof(CFRefCountNode));
    node->m_next = NULL;
    node->m_user = owner;
    if (ref->m_last) {
        ref->m_last->m_next = node;
        node->m_previous = ref->m_last;
    } else {
        // First node
        ref->m_users = node;
        node->m_previous = NULL;
    }
    ref->m_last = node;
    return ref->m_userCount;
}

int CFRefCountDetach(CFRefCount *ref, void *owner)
{
    if (!ref || !owner)
        return -1;
    /*
     * Detaching is a bit more complicated than attaching.
     * First we need to find the node and then fix the users list.
     */
    CFRefCountNode *p = NULL;
    bool found = false;
    for (p = ref->m_users; p; p = p->m_next)
        if (p->m_user == owner) {
            found = true;
            if (p->m_previous && p->m_next) {
                p->m_previous->m_next = p->m_next;
                p->m_next->m_previous = p->m_previous;
            } else if (p->m_previous && !p->m_next) {
                // Last one
                p->m_previous->m_next = NULL;
                ref->m_last = p->m_previous;
            } else if (!p->m_previous && p->m_next) {
                // First one
                ref->m_users = p->m_next;
                p->m_next->m_previous = NULL;
            } else {
                // Only one, we do not detach but return 0
                return 0;
            }
            free(p);
            break;
        }
    if (!found)
        return -1;
    ref->m_userCount--;
    return ref->m_userCount;
}

bool CFRefCountisShared(CFRefCount *ref)
{
    if (!ref)
        return false;
    return (ref->m_userCount != 1);
}

bool CFRefCountisEqual(CFRefCount *a, CFRefCount *b)
{
    if (a != b)
        return false;
    return true;
}
