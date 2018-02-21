// Controller.h

#ifndef _BASE_CONTROLLER_h
#define _BASE_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Task.h"
#include "m_Vector.h"
#include "Base_Consts.h"
#include "IO_Pin.h"

class Base_Controller : public Task
{
private:
	void OnUpdate(uint32_t deltaTime);

 protected: 
	 Vector <IO_Pin*> pins;
	 virtual void DoUpdatePins(uint32_t deltaTime) = 0;
 public:
	 IO_Pin * GetPin(uint8_t _GPIONumber) ;
	 IO_Pin* GetPin(String name);
	 Base_Controller(uint8_t priority = TASK_PRIORITY_NORMAL);
};
#endif


