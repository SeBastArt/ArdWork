#ifndef _ESP_CONTROLLER_CONSTS_h
#define _ESP_CONTROLLER_CONSTS_h


#include "Base_Controller_Consts.h"

static int const ESP_CONTROLLER_TYPE = BASE_CONTROLLER_LAST_MESSAGE + 10000;
static int const ESP_CONTROLLER_LAST_MESSAGE = ESP_CONTROLLER_TYPE;

static int const ESP_CONTROLLER_PIN_ID_START = ESP_CONTROLLER_TYPE + 100;

#endif

