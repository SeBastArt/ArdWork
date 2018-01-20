// BaseDriver.h

#ifndef _BASED_RIVER_h
#define _BASED_RIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Driver.h"
#include "Device_Driver_Consts.h"
#include "Module_Driver.h"
#include "m_Vector.h"
class Device_Driver : public Driver
{
public:
	Device_Driver(Module_Driver* _module, uint8_t _priority = THREAD_PRIORITY_NORMAL);
protected:
	Module_Driver* parentModule;
protected:
	virtual void DoBeforeSuspend() = 0;
	virtual void DoBeforeShutdown() = 0;
	virtual void DoAfterInit() = 0;
	virtual void DoDeviceMessage(Int_Thread_Msg message) = 0;
protected:
	void DoMessage(Int_Thread_Msg message);
	void DoInit();
	void DoShutdown();
	void DoSuspend();
};

#endif


