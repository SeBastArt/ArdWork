#ifndef _MODULE_DRIVER_CONSTS_h
#define _MODULE_DRIVER_CONSTS_h

#include "Base_Consts.h"

static unsigned int const MODULE_DRIVER_TYPE = BASE_TYPE + 20000000;
static unsigned int const MODULE_DRIVER_SET_DEBUG_OFF = MODULE_DRIVER_TYPE + 1;
static unsigned int const MODULE_DRIVER_SET_DEBUG_ON = MODULE_DRIVER_TYPE + 2;
static unsigned int const MODULE_DRIVER_LAST_MESSAGE = MODULE_DRIVER_TYPE;

#endif