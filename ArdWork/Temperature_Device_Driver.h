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
	REGISTER(Temperature_Device_Driver);
public:
	Temperature_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
private:
	DHT *__dht;
	void OnInit() override;
	void InitSensor();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Task_Msg message);
	void DoUpdate(uint32_t deltaTime);
	void OnBuild_Descriptor() override;

private:
	IO_Pin* __pin;
	float act_temp;
	float act_hum;
	bool __isfahrenheit;

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
	void SetPin(IO_Pin* _pin);
	float Get_Temperature();
	float Get_Humidity();
};


#endif


