// Pin_Input_Driver.h

#ifndef _BUTTON_DEVICE_DRIVER_h
#define _BUTTON_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Device_Driver.h"
#include "Button_Device_Driver_Consts.h"
#include "IO_Pin.h" 


class Button_Device_Driver : public Device_Driver
{
	REGISTER(Button_Device_Driver);
private:
	IO_Pin* __pin ;
	static const uint16_t __next_msg = 200; // (30-100) are good values
	static const uint16_t __debouceMs = 50; // (30-100) are good values
	static const uint16_t __repeatDelayMs = 1600; // (400 - 1200) are reasonable values
	static const uint16_t __repeatRateMs = 50; // (40-1000) are reasonable
	uint16_t __timer;

	Button_State __sys_state;
	Button_State __last_state;

	bool __hasPullUp;
	uint8_t __lastMessage;
private:
	void OnInit() override;
	void DoDeviceMessage(Int_Task_Msg message);
	void DoUpdate(uint32_t deltaTime);
	void OnBuild_Descriptor() override;
	void Push_Button();
public:
	Button_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
	int GetButtonPinID();
	void SetPin(IO_Pin * _pin);
	void SetPullUp();
	void SetNoPullUp();
	void Exec_Push_Button();

};

#endif


