#ifndef UART_GRBW_LED_DEVICE_DRIVER_CONSTS_h
#define UART_GRBW_LED_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"
																								   
static unsigned int const UART_GRBW_LED_DEVICE_TYPE = DEVICE_DRIVER_LAST_MESSAGE + 1003100;
static unsigned int const UART_GRBW_LED_DEVICE_OFF = UART_GRBW_LED_DEVICE_TYPE + 1;
static unsigned int const UART_GRBW_LED_DEVICE_CYCLON = UART_GRBW_LED_DEVICE_TYPE + 2;
static unsigned int const UART_GRBW_LED_DEVICE_RANDOM = UART_GRBW_LED_DEVICE_TYPE + 3;
static unsigned int const UART_GRBW_LED_DEVICE_FIRE = UART_GRBW_LED_DEVICE_TYPE + 4;
static unsigned int const UART_GRBW_LED_DEVICE_SHINE = UART_GRBW_LED_DEVICE_TYPE + 5;
static unsigned int const UART_GRBW_LED_DEVICE_NEXT = UART_GRBW_LED_DEVICE_TYPE + 6;
static unsigned int const UART_GRBW_LED_DEVICE_PREV = UART_GRBW_LED_DEVICE_TYPE + 7;
static unsigned int const UART_GRBW_LED_DEVICE_COLOR = UART_GRBW_LED_DEVICE_TYPE + 8;
static unsigned int const UART_GRBW_LED_DEVICE_BRIGHTNESS = UART_GRBW_LED_DEVICE_TYPE + 9;

static unsigned int const GRBW_ANIMATION_FIRST = UART_GRBW_LED_DEVICE_TYPE + 50;
static unsigned int const GRBW_ANIMATION_CYLON = GRBW_ANIMATION_FIRST;
static unsigned int const GRBW_ANIMATION_RANDOM = GRBW_ANIMATION_FIRST + 1;
static unsigned int const GRBW_ANIMATION_FIRE = GRBW_ANIMATION_FIRST + 2;
static unsigned int const GRBW_ANIMATION_SHINE = GRBW_ANIMATION_FIRST + 3;
static unsigned int const GRBW_ANIMATION_LAST = GRBW_ANIMATION_SHINE;
static unsigned int const GRBW_ANIMATION_COUNT = GRBW_ANIMATION_LAST - GRBW_ANIMATION_FIRST + 1;
static unsigned int const GRBW_ANIMATION_OFF = GRBW_ANIMATION_FIRST + 70;

//Extern Communication
static unsigned int const UART_GRBW_LED_SET_PATTERN_EXTERN = UART_GRBW_LED_DEVICE_TYPE + 80;
static unsigned int const UART_GRBW_LED_SET_COLOR_EXTERN = UART_GRBW_LED_DEVICE_TYPE + 81;
static unsigned int const UART_GRBW_LED_SET_BRIGHTNESS_EXTERN = UART_GRBW_LED_DEVICE_TYPE + 82;
static unsigned int const Uart_GRBW_Led_Device_Driver_LAST_MESSAGE = UART_GRBW_LED_SET_BRIGHTNESS_EXTERN;

#endif