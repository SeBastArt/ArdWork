#ifndef _DRIVER_CONSTS_h
#define _DRIVER_CONSTS_h

#include "Base_Consts.h"

static unsigned int const DRIVER_TYPE = BASE_TYPE + 100000000;
static unsigned int const DRIVER_INIT = DRIVER_TYPE + 1;
static unsigned int const DRIVER_SHUTDOWN = DRIVER_TYPE + 2;
static unsigned int const DRIVER_SUSPEND = DRIVER_TYPE + 3;
static unsigned int const DRIVER_LAST_MESSAGE = DRIVER_SUSPEND;

#endif