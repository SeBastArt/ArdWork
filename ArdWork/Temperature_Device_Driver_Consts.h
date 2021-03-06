#ifndef TEMPERATURE_DEVICE_DRIVER_CONSTS_h
#define TEMPERATURE_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"

static unsigned int const TEMPERATURE_DEVICE_DRIVER_TYPE = DEVICE_DRIVER_LAST_MESSAGE + 1008000;
static unsigned int const TEMPERATURE_DEVICE_DRIVER_SI_TEMPERATURE = TEMPERATURE_DEVICE_DRIVER_TYPE + 1;
static unsigned int const TEMPERATURE_DEVICE_DRIVER_SI_FAHRENHEIT = TEMPERATURE_DEVICE_DRIVER_TYPE + 2;
static unsigned int const TEMPERATURE_DEVICE_DRIVER_SI_CELSIUS = TEMPERATURE_DEVICE_DRIVER_TYPE + 3;
static unsigned int const TEMPERATURE_DEVICE_DRIVER_SET_ACCURACY = TEMPERATURE_DEVICE_DRIVER_TYPE + 4;
static unsigned int const TEMPERATURE_DEVICE_DRIVER_LAST_MESSAGE = TEMPERATURE_DEVICE_DRIVER_SET_ACCURACY;

#endif

