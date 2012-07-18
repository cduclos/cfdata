#ifndef CFDATADEFS_H
#define CFDATADEFS_H

#include "config.h"

#ifdef HAVE_BOOL
#include <stdbool.h>
#else
#define bool int
#define true 1
#define false 0
#endif // HAVE_BOOL

#endif // CFDATADEFS_H
