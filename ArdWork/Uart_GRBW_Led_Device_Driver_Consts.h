#ifndef Uart_GRBW_Led_Device_Driver_CONSTS_h
#define Uart_GRBW_Led_Device_Driver_CONSTS_h

#include "Device_Driver_Consts.h"
																								   
static unsigned int const Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE = DEVICE_DRIVER_LAST_MESSAGE + 1003000;
static unsigned int const UART_RGB_LED_DEVICE_OFF = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 1;
static unsigned int const UART_RGB_LED_DEVICE_CYCLON = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 2;
static unsigned int const UART_RGB_LED_DEVICE_RANDOM = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 3;
static unsigned int const UART_RGB_LED_DEVICE_FIRE = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 4;
static unsigned int const UART_RGB_LED_DEVICE_SHINE = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 5;
static unsigned int const UART_RGB_LED_DEVICE_NEXT = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 6;
static unsigned int const UART_RGB_LED_DEVICE_PREV = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 7;
static unsigned int const UART_RGB_LED_DEVICE_COLOR = Uart_GRBW_Led_Device_Driver_FIRST_MESSAGE + 8;
static unsigned int const Uart_GRBW_Led_Device_Driver_LAST_MESSAGE = UART_RGB_LED_DEVICE_COLOR;

static unsigned int const ANIMATION_FIRST = 0;
static unsigned int const ANIMATION_CYLON = ANIMATION_FIRST;
static unsigned int const ANIMATION_RANDOM = ANIMATION_FIRST + 1;
static unsigned int const ANIMATION_FIRE = ANIMATION_FIRST + 2;
static unsigned int const ANIMATION_SHINE = ANIMATION_FIRST + 3;
static unsigned int const ANIMATION_LAST = ANIMATION_SHINE;
static unsigned int const ANIMATION_COUNT = ANIMATION_LAST + 1;
#endif