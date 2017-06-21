// ESP_Controller.h

#ifndef _ESP_NODEMCU_CONTROLLER_h
#define _ESP_NODEMCU_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ESP8266_Controller.h"
#include "ESP8266_Controller_Consts.h"

class ESP8266_NodeMCU_Controller : public ESP8266_Controller
{
 
 public:
	 ESP8266_NodeMCU_Controller(uint8_t priority = THREAD_PRIORITY_NORMAL);
};

#endif
