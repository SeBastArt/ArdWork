#ifndef _PIN_MANAGER_h
#define _PIN_MANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "PCF8574_Controller.h"
#include "ESP8266_NodeMCU_Controller.h"
#include "ESP8266_01_Controller.h"
#include "TaskManager.h"

class Pin_Manager
{
private:
	Base_Controller * controller;
public:
	Pin_Manager()
	{
#ifdef ARDUINO_ESP8266_NODEMCU
		controller = new ESP8266_NodeMCU_Controller;
#endif

#ifdef ARDUINO_ESP8266_GENERIC
		controller = new ESP8266_01_Controller;
#endif
	};

	Pin_Manager(uint8_t _i2caddr)
	{
		controller = new PCF8574_Controller(_i2caddr);
	};

	IO_Pin* GetPin(String _name) {
		return controller->GetPin(_name);
	}
	IO_Pin* GetPin(uint8_t _GPIONumber) {
		return controller->GetPin(_GPIONumber);
	}

	void SetTaskContext(TaskManager* _taskmanager)
	{
		_taskmanager->StartTask(controller);
	}
};

#endif


