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
	 bool __ledStatus;
	 bool __hasPullUp;
	 //Led control
	 IO_Pin* __pin;

	 //Blink
	 bool __blink_flag;
	 uint16_t __blink_delta;
	 uint16_t __blink_delay;

	 //Pulse
	 bool __pulse_flag;
	 uint16_t __pulse_delta;
	 uint16_t __pulse_delay;
	 uint8_t __pulse_counter;
	 uint8_t __pulse_count;

	 void DoAfterInit();
	 void DoBeforeShutdown();
	 void DoBeforeSuspend();
	 void DoDeviceMessage(Int_Thread_Msg message);
	 void DoUpdate(uint32_t deltaTime);
	 void Build_Descriptor();

	 void Set_IO_Pin_High();
	 void Set_IO_Pin_Low();
	 void Blink(uint32_t deltaTime);
	 void Pulse(uint32_t deltaTime);

protected:
	void Set_Led_On();
	void Set_Led_Off();
	void Set_Led_Blink(uint16_t delay);
	void Set_Led_Pulse(uint16_t count, uint16_t delay);
public:
	void Exec_Set_Led_On();
	void Exec_Set_Led_Off();
	void Exec_Set_Led_Blink(uint16_t delay);
	void Exec_Set_Led_Pulse(uint16_t count, uint16_t delay);
	int GetPinNumber();
};

#endif

