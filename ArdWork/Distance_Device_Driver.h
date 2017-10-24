// Distance_Device_Driver.h

#ifndef _DISTANCE_DEVICE_DRIVER_h
#define _DISTANCE_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Device_Driver.h"
#include "Distance_Device_Driver_Consts.h"

class Module_Driver;



class Distance_Device_Driver : public Device_Driver
{
private:
	uint8_t __pin_t;
	uint8_t __pin_e;
	float __distance;
	bool __threePins;
	unsigned long __timeout;
	dist_unit __unit;
	uint16_t __accuracy_delta;
	uint16_t __accuracy_delay;

	float Timing();
	float DistanceRead_cm();
	float DistanceRead_inch();
	void Set_Unit(dist_unit _unit);
	void Set_Timeout(int _timeout);
protected:
	void DoAfterInit();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);
	
	void Build_Descriptor();

 public:
	 Distance_Device_Driver(Module_Driver* module, uint8_t pin_trig, uint8_t pin_echo, uint8_t priority = THREAD_PRIORITY_NORMAL);
	 void Exec_Set_Unit(dist_unit _unit);
	 void Exec_Set_Timeout(int _timeout);

};


#endif

