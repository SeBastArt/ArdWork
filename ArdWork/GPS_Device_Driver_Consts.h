#ifndef _GPS_DEVICE_DRIVER_CONSTS_h
#define _GPS_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"

static unsigned int const GPS_DEVICE_DRIVER_TYPE = DEVICE_DRIVER_TYPE + 1004000;
static unsigned int const GPS_DEVICE_DRIVER_LOCAL_TIME = GPS_DEVICE_DRIVER_TYPE + 1;
static unsigned int const GPS_DEVICE_DRIVER_UTC_TIME = GPS_DEVICE_DRIVER_TYPE + 2;
static unsigned int const GPS_DEVICE_DRIVER_TIMEZONE = GPS_DEVICE_DRIVER_TYPE + 3;
static unsigned int const GPS_DEVICE_DRIVER_GET_TIME = GPS_DEVICE_DRIVER_TYPE + 4;
static unsigned int const GPS_DEVICE_DRIVER_LAST_MESSAGE = GPS_DEVICE_DRIVER_TIMEZONE;

#endif
