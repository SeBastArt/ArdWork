// ESP8266_01_Controller.h

#ifndef _ESP8266_01_CONTROLLER_h
#define _ESP8266_01_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ESP8266_Controller.h"

class ESP8266_01_Controller : public ESP8266_Controller
{
public:
	ESP8266_01_Controller(uint8_t priority = THREAD_PRIORITY_NORMAL);
};


#endif


