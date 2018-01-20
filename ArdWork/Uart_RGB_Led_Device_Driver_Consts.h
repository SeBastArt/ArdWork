#ifndef Uart_RGB_Led_Device_Driver_CONSTS_h
#define Uart_RGB_Led_Device_Driver_CONSTS_h

#include "Device_Driver_Consts.h"

static unsigned int const UART_RGB_LED_DEVICE_TYPE = DEVICE_DRIVER_LAST_MESSAGE + 1003200;
static unsigned int const UART_RGB_LED_DEVICE_OFF = UART_RGB_LED_DEVICE_TYPE + 1;
static unsigned int const UART_RGB_LED_DEVICE_CYCLON = UART_RGB_LED_DEVICE_TYPE + 2;
static unsigned int const UART_RGB_LED_DEVICE_RANDOM = UART_RGB_LED_DEVICE_TYPE + 3;
static unsigned int const UART_RGB_LED_DEVICE_FIRE = UART_RGB_LED_DEVICE_TYPE + 4;
static unsigned int const UART_RGB_LED_DEVICE_SHINE = UART_RGB_LED_DEVICE_TYPE + 5;
static unsigned int const UART_RGB_LED_DEVICE_NEXT = UART_RGB_LED_DEVICE_TYPE + 6;
static unsigned int const UART_RGB_LED_DEVICE_PREV = UART_RGB_LED_DEVICE_TYPE + 7;
static unsigned int const UART_RGB_LED_DEVICE_COLOR = UART_RGB_LED_DEVICE_TYPE + 8;
static unsigned int const UART_RGB_LED_DEVICE_BRIGHTNESS = UART_RGB_LED_DEVICE_TYPE + 9;
static unsigned int const UART_RGB_LED_COLOR_HEX = UART_RGB_LED_DEVICE_TYPE + 10;
static unsigned int const UART_RGB_LED_PATTERN = UART_RGB_LED_DEVICE_TYPE + 11;

static unsigned int const RGB_ANIMATION_FIRST = UART_RGB_LED_DEVICE_TYPE + 20;
static unsigned int const RGB_ANIMATION_CYLON = RGB_ANIMATION_FIRST ;
static unsigned int const RGB_ANIMATION_RANDOM = RGB_ANIMATION_FIRST + 1;
static unsigned int const RGB_ANIMATION_FIRE = RGB_ANIMATION_FIRST + 2;
static unsigned int const RGB_ANIMATION_SHINE = RGB_ANIMATION_FIRST + 3;
static unsigned int const RGB_ANIMATION_LAST = RGB_ANIMATION_SHINE;
static unsigned int const RGB_ANIMATION_COUNT = RGB_ANIMATION_LAST - RGB_ANIMATION_FIRST + 1;
static unsigned int const RGB_ANIMATION_OFF = RGB_ANIMATION_FIRST + 20;

static unsigned int const Uart_RGB_Led_Device_Driver_LAST_MESSAGE = RGB_ANIMATION_OFF;

#endif

