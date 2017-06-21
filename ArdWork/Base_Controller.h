// Controller.h

#ifndef _BASE_CONTROLLER_h
#define _BASE_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Thread/Thread.h"
#include "m_Vector.h"
//#include "Int_Thread_Msg_Sys.h"
#include "Base_Consts.h"
#include "IO_Pin.h"

class Base_Controller : public Thread
{
private:
	bool OnStart();
	void OnStop();
	void OnUpdate(uint32_t deltaTime);
 protected: 
	 Vector <IO_Pin*> pins;
	 void InitPins();
	 virtual void DoUpdatePins(uint32_t deltaTime) = 0;
 public:
	 IO_Pin* Pin(uint8_t number);
	 IO_Pin* Pin(String name);
	 Base_Controller(uint8_t priority = THREAD_PRIORITY_NORMAL);
};
#endif

