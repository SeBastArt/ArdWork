// Wifi_Device_Driver_Consts.h

#ifndef _WIFI_DEVICE_DRIVER_CONSTS_h
#define _WIFI_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"

static unsigned int const WIFI_DEVICE_DRIVER_TYPE = DEVICE_DRIVER_LAST_MESSAGE + 1005000;
static unsigned int const WIFI_DEVICE_DRIVER_SET_SSID = WIFI_DEVICE_DRIVER_TYPE + 1;
static unsigned int const WIFI_DEVICE_DRIVER_SET_PASSWORD = WIFI_DEVICE_DRIVER_TYPE + 2;
static unsigned int const WIFI_DEVICE_DRIVER_SET_MODE = WIFI_DEVICE_DRIVER_TYPE + 3;
static unsigned int const WIFI_DEVICE_DRIVER_RECONNECT = WIFI_DEVICE_DRIVER_TYPE + 4;
static unsigned int const WIFI_DEVICE_DRIVER_LAST_MESSAGE = WIFI_DEVICE_DRIVER_RECONNECT;

#endif


