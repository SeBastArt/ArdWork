// ESP8266_Controller.h

#ifndef _ESP8266_CONTROLLER_h
#define _ESP8266_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Base_Controller.h"
#include "ESP8266_Controller_Consts.h"

class ESP8266_Controller : public Base_Controller
{
 protected:
	void DoUpdatePins(uint32_t deltaTime);
public: 
	ESP8266_Controller(uint8_t priority = THREAD_PRIORITY_NORMAL);
};

#endif


