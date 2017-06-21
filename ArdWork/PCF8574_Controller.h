// PCF8574_Controller.h

#ifndef _PCF8574_CONTROLLER_h
#define _PCF8574_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "I2C_Controller.h"

class PCF8574_Controller : public I2C_Controller
{
 public:
	 PCF8574_Controller(uint32_t adress, uint8_t priority = THREAD_PRIORITY_NORMAL);
};


#endif

