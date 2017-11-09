#include "Led_Device_Driver.h"

Led_Device_Driver::Led_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool _hasPullUp, uint8_t priority) :
	Device_Driver(module, priority),
	__pin(_pin),
	__hasPullUp(_hasPullUp)
{
	__DriverType = LED_DEVICE_DRIVER_TYPE;
};

void Led_Device_Driver::Build_Descriptor() {
	__descriptor->name = F("LED");
	__descriptor->descr = F("a simple LED");
	__descriptor->published = false;

	Ctrl_Elem *ctrl_elem_mode = new Ctrl_Elem(LED_DEVICE_DRIVER_LED_SET_MODE, F("LED-Mode"), select, F("Select the Mode of the LED"));

	Atomic<String> *atomic_mode_on = new Atomic<String>(0, F("On"), "");
	Atomic<String> *atomic_mode_off = new Atomic<String>(1, F("OFF"), "");
	Atomic<String> *atomic_mode_blink = new Atomic<String>(2, F("Blink"), "");
	Atomic<String> *atomic_mode_pulse = new Atomic<String>(3, F("Pulse"), "");

	ctrl_elem_mode->AddAtomic(atomic_mode_on);
	ctrl_elem_mode->AddAtomic(atomic_mode_off);
	ctrl_elem_mode->AddAtomic(atomic_mode_blink);
	ctrl_elem_mode->AddAtomic(atomic_mode_pulse);
	ctrl_elem_mode->published = true;

	Ctrl_Elem *ctrl_elem_delay = new Ctrl_Elem(LED_DEVICE_DRIVER_LED_SET_DELAY, F("Delay"), text, F("Blink Delay is the delay between status change On/Off"));
	Atomic<String> *atomic_delay = new Atomic<String>(0, F("Delay"));
	ctrl_elem_delay->AddAtomic(atomic_delay);
	ctrl_elem_delay->published = true;

	Ctrl_Elem *ctrl_elem_pulse_count = new Ctrl_Elem(LED_DEVICE_DRIVER_LED_SET_PULSE_COUNT, F("Pulse count"), text, F("change the count how often die led is blinking during pulse"));
	Atomic<String> *atomic_pulse_count = new Atomic<String>(0, F("Pulse Count"));
	ctrl_elem_pulse_count->AddAtomic(atomic_pulse_count);
	ctrl_elem_pulse_count->published = true;

	__descriptor->Add_Descriptor_Element(ctrl_elem_mode);
	__descriptor->Add_Descriptor_Element(ctrl_elem_delay);
	__descriptor->Add_Descriptor_Element(ctrl_elem_pulse_count);

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
	__delay = 500;
	__blink_delta = 0;
	
	__pulse_flag = false;
	__pulse_delta = 0;
	__pulse_counter = 0;
	__pulse_count = 5;

	Serial.println(F("Led-Driver initialized!"));
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

void Led_Device_Driver::Exec_Set_Led_Blink(int delay) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_LED_BLINK);
	message->AddParam(delay);
	PostMessage(&message);
}

void Led_Device_Driver::Exec_Set_Led_Pulse(int count, int delay) {
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
	case LED_DEVICE_DRIVER_LED_SET_DELAY:
	{
		int delay = message.GetIntParamByIndex(1);
		Set_Led_Delay(delay);
	}
	break;
	case LED_DEVICE_DRIVER_LED_SET_PULSE_COUNT:
	{
		int pulse_count = message.GetIntParamByIndex(1);
		Set_Led_Pulse_Count(pulse_count);
	}
	break;
	case LED_DEVICE_DRIVER_LED_SET_MODE:
	{
		int mode = message.GetIntParamByIndex(1);
		Set_Led_Mode(mode);
	}
	}
}

void Led_Device_Driver::Set_Led_Delay(int _delay) {
	if ((_delay >= 200) && (_delay < 60000)) {
		__delay = _delay;
	}
}

void Led_Device_Driver::Set_Led_Pulse_Count(int _pulse_count) {
	if ((_pulse_count > 0) && (_pulse_count < 1000)) {
		__pulse_count = _pulse_count;
	}
}

void Led_Device_Driver::Set_Led_Mode(uint8_t _mode) {
	switch (_mode)
	{
	case 0:
	{
		Set_Led_On();
	}
	break;
	case 1:
	{
		Set_Led_Off();
	}
	break;
	case 2:
	{
		Set_Led_Blink(__delay);
	}
	break;
	case 3:
	{
		Set_Led_Pulse(__pulse_count, __delay);
	}
	break;
	}
}

void Led_Device_Driver::Pulse(uint32_t deltaTime) {
	__pulse_delta += deltaTime;
	if (__pulse_delta > __delay) {
		__pulse_delta = 0;
		if (__pulse_counter < __pulse_count) {
			if (__ledStatus == true) {
				Set_IO_Pin_Low();
			}
			else {
				__pulse_counter++;
				Set_IO_Pin_High();
			}
		}
		else
		{
			__pulse_counter = 0;
			Set_Led_Off();
		}
	}
}


void Led_Device_Driver::Blink(uint32_t deltaTime) {
	__blink_delta += deltaTime;
	if (__blink_delta > __delay) {
		__blink_delta = 0;
		if (__ledStatus == true) {
			Set_IO_Pin_Low();
		}
		else {
			Set_IO_Pin_High();
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

void Led_Device_Driver::Set_Led_Pulse(int count, int delay)
{
	if (!__pulse_flag) {
		__delay = delay;
		__pulse_count = count;
		__pulse_counter = 0;
		__pulse_flag = true;
		__blink_flag = false;
		__pulse_delta = 0;
		Set_IO_Pin_Low();
	}
}

void Led_Device_Driver::Set_Led_Blink(int delay)
{
	__delay = delay;
	if (__blink_flag == false) {
		__blink_flag = true;
		__pulse_flag = false;
		__blink_delta = 0;
		Set_IO_Pin_Low();
	}
}
