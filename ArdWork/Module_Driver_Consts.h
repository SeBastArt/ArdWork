#ifndef _MODULE_DRIVER_CONSTS_h
#define _MODULE_DRIVER_CONSTS_h

#include "Base_Consts.h"

static unsigned int const MODULE_DRIVER_TYPE = DRIVER_TYPE + 20000000;
static unsigned int const MODULE_DRIVER_SET_DEBUG_MODE = MODULE_DRIVER_TYPE + 1;

static unsigned int const MODULE_DRIVER_SET_DEBUG_MODE_EXTERN = MODULE_DRIVER_TYPE + 50;
static unsigned int const MODULE_DRIVER_LAST_MESSAGE = MODULE_DRIVER_SET_DEBUG_MODE_EXTERN;

#endif