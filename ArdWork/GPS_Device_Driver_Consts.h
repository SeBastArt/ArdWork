#ifndef _GPS_DEVICE_DRIVER_CONSTS_h
#define _GPS_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"

static unsigned int const GPS_DEVICE_DRIVER_TYPE = DEVICE_DRIVER_TYPE + 300;
static unsigned int const GPS_DEVICE_DRIVER_LOCAL_TIME = DEVICE_DRIVER_TYPE + 1;
static unsigned int const GPS_DEVICE_DRIVER_UTC_TIME = DEVICE_DRIVER_TYPE + 2;
static unsigned int const GPS_DEVICE_DRIVER_LAST_MESSAGE = DEVICE_DRIVER_TYPE;

#endif
