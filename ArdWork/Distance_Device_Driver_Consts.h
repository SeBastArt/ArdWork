#ifndef DISTANCE_DEVICE_DRIVER_CONSTS_h
#define DISTANCE_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"

enum dist_unit
{
	cm,
	inch
};

static const char CM_STRING[] = "cm";
static const char INCH_STRING[] = "inch";

//intern
static unsigned int const DISTANCE_DEVICE_DRIVER_TYPE = DEVICE_DRIVER_LAST_MESSAGE + 1002000;
static unsigned int const DISTANCE_DEVICE_DRIVER_SET_UNIT = DISTANCE_DEVICE_DRIVER_TYPE + 1;
static unsigned int const DISTANCE_DEVICE_DRIVER_SET_TIMEOUT = DISTANCE_DEVICE_DRIVER_TYPE + 2;

//publisher
static unsigned int const DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE = DISTANCE_DEVICE_DRIVER_TYPE + 50;
static unsigned int const DISTANCE_DEVICE_DRIVER_LAST_MESSAGE = DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE;
#endif