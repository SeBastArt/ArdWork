#ifndef _DEVICE_CONSTS_h
#define _DEVICE_CONSTS_h

#include "Driver_Consts.h"

static unsigned int const DEVICE_DRIVER_TYPE = DRIVER_TYPE + 10000000;
static unsigned int const DEVICE_DRIVER_INIT = DEVICE_DRIVER_TYPE + 1;
static unsigned int const DEVICE_DRIVER_SHUTDOWN = DEVICE_DRIVER_TYPE + 2;
static unsigned int const DEVICE_DRIVER_SUSPEND = DEVICE_DRIVER_TYPE + 3;
static unsigned int const DEVICE_DRIVER_LAST_MESSAGE = DEVICE_DRIVER_SUSPEND;

#endif

