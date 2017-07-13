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
#include "Module_Driver.h"
#include "IO_Pin.h" 


class Button_Device_Driver : public Device_Driver
{
private:
	IO_Pin* pin ;
	static const uint16_t _next_msg = 200; // (30-100) are good values
	static const uint16_t _debouceMs = 50; // (30-100) are good values
	static const uint16_t _repeatDelayMs = 1600; // (400 - 1200) are reasonable values
	static const uint16_t _repeatRateMs = 50; // (40-1000) are reasonable
	uint16_t _timer;

	Button_State sys_state;
	Button_State last_state;
	uint32 last_tick;

	bool hasPullUp;
	unsigned int lastMessage;
private:
	void DoAfterInit();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);
	void DoExecuteCommand(String _command);
	
	void Do_Push_Button();
public:
	Button_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool _hasPullUp = false, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void SetPullUp(bool _hasPullUp);
	uint8_t GetButtonPinID();

	void Exec_Push_Button();
};

#endif

