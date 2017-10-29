#ifndef _I2C_CONTROLLER_CONSTS_h
#define _I2C_CONTROLLER_CONSTS_h


#include "Base_Controller_Consts.h"

static int const I2C_CONTROLLER_TYPE = BASE_CONTROLLER_LAST_MESSAGE + 20000;
static int const I2C_CONTROLLER_LAST_MESSAGE = I2C_CONTROLLER_TYPE;

static int const I2C_CONTROLLER_PIN_ID_START = I2C_CONTROLLER_TYPE + 100;


#endif