// Temperature_Device_Driver.h

#ifndef _TEMPERATURE_DEVICE_DRIVER_h
#define _TEMPERATURE_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DHT.h>
#include "Device_Driver.h"
#include "Temperature_Device_Driver_Consts.h"
#include "IO_Pin.h"

class Temperature_Device_Driver : public Device_Driver
{
public:
	Temperature_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool isFahrenheit = false, uint8_t priority = THREAD_PRIORITY_NORMAL);
private:
	DHT *dht;
	void DoAfterInit();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);
	void Build_Descriptor();

private:
	IO_Pin* pin;
	float act_temp;
	float act_hum;
	bool fahrenheit;

	uint16_t accuracy_delta;
	uint16_t accuracy_delay;

protected:
	void Set_SI_To_Fahrenheit();
	void Set_SI_To_Celsius();
	void Set_Accuracy_Time_Ms(uint16_t time);

public:
	void Exec_Set_SI_To_Fahrenheit();
	void Exec_Set_SI_To_Celsius();
	void Exec_Set_Accuracy_Time_Ms(uint16_t time);
	
	float Get_Temperature();
	float Get_Humidity();
};


#endif

