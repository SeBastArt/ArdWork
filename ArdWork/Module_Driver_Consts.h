#ifndef _MODULE_DRIVER_CONSTS_h
#define _MODULE_DRIVER_CONSTS_h

#include "Base_Consts.h"

static unsigned int const MODULE_DRIVER_FIRST_MESSAGE = BASE_FIRST_MESSAGE + 20000000;
static unsigned int const MODULE_DRIVER_SET_DEBUG = MODULE_DRIVER_FIRST_MESSAGE + 1;
static unsigned int const MODULE_DRIVER_LAST_MESSAGE = MODULE_DRIVER_FIRST_MESSAGE;

#endif