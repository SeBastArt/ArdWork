// Pin_Output_Driver.h

#ifndef _LED_DEVICE_DRIVER_h
#define _LED_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Device_Driver.h"
#include "Led_Device_Driver_Consts.h"
#include "IO_Pin.h"

class Led_Device_Driver : public Device_Driver
{
	REGISTER(Led_Device_Driver);
public:
	Led_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
private:
	int __sv_mode;
	float __sv_delay;
	float __sv_pulse_count;

	bool __ledStatus;
	bool __hasPullUp;
	//Led control
	IO_Pin* __pin;

	//Blink
	bool __blink_flag;
	int __blink_delta;

	//Pulse
	bool __pulse_flag;
	int __pulse_delta;

	int __pulse_counter;


	void OnInit() override;
	void DoDeviceMessage(Int_Task_Msg message);
	void Set_Led_Delay(int _delay);
	void Set_Led_Pulse_Count(int _pulse_count);
	void Set_Led_Mode(uint8_t _mode);
	void DoUpdate(uint32_t deltaTime);
	void OnBuild_Descriptor() override;

	void Set_IO_Pin_High();
	void Set_IO_Pin_Low();
	void Blink(uint32_t deltaTime);
	void Pulse(uint32_t deltaTime);

protected:
	void Set_Led_On();
	void Set_Led_Off();
	void Set_Led_Pulse(int count, int delay);
	void Set_Led_Blink(int delay);
public:
	void Exec_Set_Led_On();
	void Exec_Set_Led_Off();
	void Exec_Set_Led_Blink(int delay);
	void Exec_Set_Led_Pulse(int count, int delay);

	void SetPin(IO_Pin * _pin);
	void SetPullUp();
	void SetNoPullUp();

};

#endif


