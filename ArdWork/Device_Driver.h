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

class Device_Driver : public Driver, public Observee
{
public:
	Device_Driver(Module_Driver* _module, uint8_t _priority = TASK_PRIORITY_NORMAL);
protected:
	Module_Driver* __parentModule;
protected:
	virtual void DoDeviceMessage(Int_Task_Msg message) = 0;
protected:
	virtual void OnMessage(Int_Task_Msg message);
	virtual void OnStartup();
	virtual void OnStop();
	virtual void OnInit();
	virtual void OnShutdown();
	virtual void OnSuspend();
	virtual void OnBuild_Descriptor();
};

#endif


