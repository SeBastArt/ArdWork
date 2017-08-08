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
public:
	Led_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool _hasPullUp = false, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void SetPullUp(bool _hasPullUp);
 private:
	 bool isOn;
	 bool hasPullUp;
	 //Led control
	 IO_Pin* pin;

	 //Blink
	 bool blink_flag;
	 uint16_t blink_delta;
	 uint16_t blink_delay;
	 bool blink_On_Off;

	 //Pulse
	 bool pulse_flag;
	 uint16_t pulse_delta;
	 uint16_t pulse_delay;
	 bool pulse_On_Off;
	 uint8_t pulse_counter;
	 uint8_t pulse_count;

	 void DoAfterInit();
	 void DoBeforeShutdown();
	 void DoBeforeSuspend();
	 void DoDeviceMessage(Int_Thread_Msg message);
	 void DoUpdate(uint32_t deltaTime);
	 void DoExecuteCommand(String _command);

	 void Blink(uint32_t deltaTime);
	 void Pulse(uint32_t deltaTime);

protected:
	void Set_IO_Pin_High();
	void Set_IO_Pin_Low();
	void Set_IO_Pin_Blink(uint16_t delay);
	void Set_IO_Pin_Pulse(uint16_t count, uint16_t delay);
public:
	void Exec_Set_IO_Pin_High();
	void Exec_Set_IO_Pin_Low();
	void Exec_Set_IO_Pin_Blink(uint16_t delay);
	void Exec_Set_IO_Pin_Pulse(uint16_t count, uint16_t delay);
	int GetPinNumber();
};

#endif

