#ifndef _DRIVER_CONSTS_h
#define _DRIVER_CONSTS_h

#include "Base_Consts.h"

static unsigned int const DRIVER_FIRST_MESSAGE = BASE_FIRST_MESSAGE + 100000000;
static unsigned int const DRIVER_INIT = DRIVER_FIRST_MESSAGE + 1;
static unsigned int const DRIVER_SHUTDOWN = DRIVER_FIRST_MESSAGE + 2;
static unsigned int const DRIVER_SUSPEND = DRIVER_FIRST_MESSAGE + 3;
static unsigned int const DRIVER_LAST_MESSAGE = DRIVER_SUSPEND;

#endif