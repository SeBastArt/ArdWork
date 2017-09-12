// Distance_Device_Driver.h

#ifndef _DISTANCE_DEVICE_DRIVER_h
#define _DISTANCE_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Device_Driver.h"

class Module_Driver;

class Distance_Device_Driver : public Device_Driver
{
private:
	uint8_t __pin_t;
	uint8_t __pin_e;
	float __distance;

private:
	void DoAfterInit();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);
	void Build_Descriptor();

 public:
	 Distance_Device_Driver(Module_Driver* module, uint8_t pin_trig, uint8_t pin_echo, uint8_t priority = THREAD_PRIORITY_NORMAL);
};


#endif

