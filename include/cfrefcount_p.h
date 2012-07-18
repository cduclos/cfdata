#ifndef CFREFCOUNT_P_H
#define CFREFCOUNT_P_H

/*
 * WARNING
 *
 * This file exists as an implementation detail and can be changed at anytime.
 * You should not be using it or referencing it.
 * If your code breaks because I changed it, that is your fault.
 * You have been warned!
 */

struct CFRefCountNode {
    struct CFRefCountNode *m_next;
    struct CFRefCountNode *m_previous;
    void *m_user;
};
typedef struct CFRefCountNode CFRefCountNode;

#endif // CFREFCOUNT_P_H
