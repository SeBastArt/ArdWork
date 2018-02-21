// Luxmeter_Device_Driver.h

#ifndef _LUXMETER_DEVICE_DRIVER_h
#define _LUXMETER_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Device_Driver.h"
#include "Luxmeter_Device_Driver_Consts.h"

#include <pgmspace.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>


const float EPSILON = 5.0;

class Luxmeter_Device_Driver : public Device_Driver
{
	REGISTER(Luxmeter_Device_Driver);
public:
	Luxmeter_Device_Driver(Module_Driver* module, uint8_t adress = TSL2561_ADDR_FLOAT, uint8_t priority = TASK_PRIORITY_NORMAL);
	
 private:
	 float lastLux;
	 int sv_integrationTime = 0;
	 int sv_auto_range = 0;
	 int sv_gain = 0;
	 int sv_acc_rate = 0;
	 void OnInit() override;
	 void DoBeforeShutdown();
	 void DoBeforeSuspend();
	 void DoDeviceMessage(Int_Task_Msg message);
	 void DoUpdate(uint32_t deltaTime);
	 void OnBuild_Descriptor() override;
private:
	uint16_t accuracy_delta;
	uint16_t accuracy_delay;
	Adafruit_TSL2561_Unified *tsl;
	void DisplaySensorDetails(void);

protected:
	void Set_Accuracy_Delay(uint16 _milliseconds);
	void Set_Integration_Time(tsl2561IntegrationTime_t _integrationTime);
	void Set_Gain(tsl2561Gain_t _gain);
	void Set_Enable_AutoRange(bool _autoRange);
public:
	void Exec_Set_Accuracy_Delay(uint16 _milliseconds);
	void Exec_Set_Integration_Time(tsl2561IntegrationTime_t _integrationTime);
	void Exec_Set_Gain(tsl2561Gain_t _gain);
	void Exec_Set_Enable_AutoRange(bool _autoRange);
};

#endif


