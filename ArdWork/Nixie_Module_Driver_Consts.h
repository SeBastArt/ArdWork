#ifndef _NIXIE_MODULE_DRIVER_CONSTS_h
#define _NIXIE_MODULE_DRIVER_CONSTS_h

#include "Module_Driver_Consts.h"

static unsigned int const NIXIE_MODULE_DRIVER_TYPE = MODULE_DRIVER_LAST_MESSAGE + 1002000;
static unsigned int const NIXIE_MODULE_DRIVER_PATTERN_SWITCH = NIXIE_MODULE_DRIVER_TYPE + 1;
static unsigned int const NIXIE_MODULE_DRIVER_PATTERN_COLOR = NIXIE_MODULE_DRIVER_TYPE + 2;

static unsigned int const NIXIE_MODULE_DRIVER_LAST_MESSAGE = NIXIE_MODULE_DRIVER_PATTERN_COLOR;

#endif

