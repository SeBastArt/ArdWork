#include "Led_Device_Driver.h"

Led_Device_Driver::Led_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool _hasPullUp, uint8_t priority) :
	Device_Driver(module, priority),
	__pin(_pin),
	__hasPullUp(_hasPullUp)
{
	driver_name = "Led_Device_Driver";
};

void Led_Device_Driver::Build_Descriptor() {
	__descriptor->name = "LED";
	__descriptor->descr = "a simple LED";
}

void Led_Device_Driver::SetPullUp(bool _hasPullUp)
{
	__hasPullUp = _hasPullUp;
}


void Led_Device_Driver::DoAfterInit()
{
	__pin->IsActive = true;
	Set_IO_Pin_Low();
	__blink_flag = false;
	__blink_delay = 500;
	__blink_delta = 0;

	__pulse_flag = false;
	__pulse_delay = 500;
	__pulse_delta = 0;
	__pulse_counter = 0;
	__pulse_count = 0;

	Serial.println("Led-Driver initialized!");
}

void Led_Device_Driver::DoBeforeShutdown()
{
	__pin->SetPinMode(OUTPUT);
	(__hasPullUp) ? __pin->SetPinState(HIGH) : __pin->SetPinState(LOW);
}

void Led_Device_Driver::DoBeforeSuspend()
{
	__pin->SetPinMode(OUTPUT);
	(__hasPullUp) ? __pin->SetPinState(HIGH) : __pin->SetPinState(LOW);
}


void Led_Device_Driver::Exec_Set_Led_On() {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_LED_ON);
	PostMessage(&message);
}


void Led_Device_Driver::Exec_Set_Led_Off() {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_LED_OFF);
	PostMessage(&message);
}

void Led_Device_Driver::Exec_Set_Led_Blink(uint16_t delay) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_LED_BLINK);
	message->AddParam(delay);
	PostMessage(&message);
}

void Led_Device_Driver::Exec_Set_Led_Pulse(uint16_t count, uint16_t delay) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_LED_PULSE);
	message->AddParam(count);
	message->AddParam(delay);
	PostMessage(&message);
}

int Led_Device_Driver::GetPinNumber()
{
	return __pin->PinNumber();
}


void Led_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{
	int messageID = message.GetID();
	switch (messageID)
	{
	case LED_DEVICE_DRIVER_LED_ON:
	{
		Set_Led_On();
	}
	break;
	case LED_DEVICE_DRIVER_LED_OFF:
	{
		Set_Led_Off();
	}
	break;
	case LED_DEVICE_DRIVER_LED_BLINK:
	{
		int blink_delay = message.GetIntParamByIndex(1);
		Set_Led_Blink(blink_delay);
	}
	break;
	case LED_DEVICE_DRIVER_LED_PULSE:
	{
		int pulse_count = message.GetIntParamByIndex(1);
		int pulse_delay = message.GetIntParamByIndex(2);
		Set_Led_Pulse(pulse_count, pulse_delay);
	}
	break;
	}
}

void Led_Device_Driver::Pulse(uint32_t deltaTime) {
	__pulse_delta += deltaTime;
	if (__pulse_delta > __pulse_delay) {
		__pulse_delta = 0;
		if (__pulse_counter < __pulse_count) {
			if (__ledStatus == true) {
				Set_Led_Off();
			}
			else {
				__pulse_counter++;
				Set_Led_On();
			}
		}
		else
		{
			__pulse_flag = false;
			__pulse_counter = 0;
			__pulse_count = 0;
			Set_IO_Pin_Low();
		}
	}
}


void Led_Device_Driver::Blink(uint32_t deltaTime) {
	__blink_delta += deltaTime;
	if (__blink_delta > __blink_delay) {
		__blink_delta = 0;
		if (__ledStatus == true) {
			Set_Led_Off();
		}
		else {
			Set_Led_Off();
		}
	}
}

void Led_Device_Driver::DoUpdate(uint32_t deltaTime) {
	if (__pulse_flag == true) {
		Pulse(deltaTime);
	}

	if (__blink_flag == true) {
		Blink(deltaTime);
	}
}

void Led_Device_Driver::Set_Led_On()
{
	Set_IO_Pin_High();
	__blink_flag = false;
	__pulse_flag = false;
}

void Led_Device_Driver::Set_IO_Pin_High()
{
	__ledStatus = true;
	(__hasPullUp) ? __pin->SetPinState(LOW) : __pin->SetPinState(HIGH);
}

void Led_Device_Driver::Set_Led_Off() {
	Set_IO_Pin_Low();
	__blink_flag = false;
	__pulse_flag = false;
}

void Led_Device_Driver::Set_IO_Pin_Low()
{
	__ledStatus = false;
	(__hasPullUp) ? __pin->SetPinState(HIGH) : __pin->SetPinState(LOW);
}

void Led_Device_Driver::Set_Led_Pulse(uint16_t count, uint16_t delay)
{
	if (!__pulse_flag) {
		__pulse_delay = delay;
		__pulse_count = count;
		__pulse_counter = 0;
		__pulse_flag = true;
		__pulse_delta = 0;
		Set_Led_On();
	}
}

void Led_Device_Driver::Set_Led_Blink(uint16_t delay)
{
	__blink_delay = delay;
	if (__blink_flag == false) {
		__blink_flag = true;
		__blink_delta = 0;
		Set_Led_On();
	}
}
