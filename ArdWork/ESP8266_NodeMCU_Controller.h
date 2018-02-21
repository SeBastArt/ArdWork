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
	 ESP8266_NodeMCU_Controller(uint8_t priority = TASK_PRIORITY_NORMAL);
	 static ESP8266_NodeMCU_Controller* getInstance() {
		 static ESP8266_NodeMCU_Controller instance;
		 return &instance;
	 }
};

#endif

