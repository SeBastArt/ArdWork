#include "Led_Device_Driver.h"

Led_Device_Driver::Led_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool _hasPullUp, uint8_t priority) :
	Device_Driver(module, priority),
	pin(_pin),
	hasPullUp(_hasPullUp)
{
	driver_name = "Led_Device_Driver";
	Control *std;
	std = ctrl_List->Front();
	std->Name = "Led";
	std->Description = "Lampe";
	std->Style = Icon_Kind_switch;
};

void Led_Device_Driver::SetPullUp(bool _hasPullUp)
{
	hasPullUp = _hasPullUp;
}


void Led_Device_Driver::DoAfterInit()
{
	pin->IsActive = true;
	(hasPullUp) ? pin->SetPinState(HIGH) : pin->SetPinState(LOW);
	//pin->SetPinState(LOW);
	blink_flag = false;
	blink_delay = 500;
	blink_delta = 0;

	pulse_flag = false;
	pulse_delay = 500;
	pulse_delta = 0;
	pulse_counter = 0;
	pulse_count = 0;

	Serial.println("Led-Driver initialized!");
}

void Led_Device_Driver::DoBeforeShutdown()
{
	pin->SetPinMode(OUTPUT);
	(hasPullUp) ? pin->SetPinState(HIGH) : pin->SetPinState(LOW);
	//pin->SetPinState(LOW);
}

void Led_Device_Driver::DoBeforeSuspend()
{
	pin->SetPinMode(OUTPUT);
	(hasPullUp) ? pin->SetPinState(HIGH) : pin->SetPinState(LOW);
	//pin->SetPinState(LOW);
}


void Led_Device_Driver::Exec_Set_IO_Pin_High() {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_PIN_HIGH);
	PostMessage(&message);
}


void Led_Device_Driver::Exec_Set_IO_Pin_Low() {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_PIN_LOW);
	PostMessage(&message);
}

void Led_Device_Driver::Exec_Set_IO_Pin_Blink(uint16_t delay) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_PIN_BLINK);
	message->AddParam(delay);
	PostMessage(&message);
}

void Led_Device_Driver::Exec_Set_IO_Pin_Pulse(uint16_t count, uint16_t delay) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LED_DEVICE_DRIVER_PIN_PULSE);
	message->AddParam(count);
	message->AddParam(delay);
	PostMessage(&message);
}

int Led_Device_Driver::GetPinNumber()
{
	return pin->PinNumber();
}


void Led_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{
	int messageID = message.GetID();
	switch (messageID)
	{
	case LED_DEVICE_DRIVER_PIN_HIGH:
	{
		Set_IO_Pin_High();
	}
	break;
	case LED_DEVICE_DRIVER_PIN_LOW:
	{
		Set_IO_Pin_Low();
	}
	break;
	case LED_DEVICE_DRIVER_PIN_BLINK:
	{
		int _blink_delay = message.GetIntParamByIndex(1);
		Set_IO_Pin_Blink(_blink_delay);
	}
	break;
	case LED_DEVICE_DRIVER_PIN_PULSE:
	{
		int _pulse_count = message.GetIntParamByIndex(1);
		int _pulse_delay = message.GetIntParamByIndex(2);
		Set_IO_Pin_Pulse(_pulse_count, _pulse_delay);
	}
	break;
	}
}

void Led_Device_Driver::Pulse(uint32_t deltaTime) {
	pulse_delta += deltaTime;
	if (pulse_delta > pulse_delay) {
		pulse_delta = 0;
		if (pulse_counter < pulse_count) {
			if (pulse_On_Off == true) {
				pulse_On_Off = false;
				(hasPullUp) ? pin->SetPinState(HIGH) : pin->SetPinState(LOW);
				//pin->SetPinState(LOW);
			}
			else {
				pulse_counter++;
				pulse_On_Off = true;
				(hasPullUp) ? pin->SetPinState(LOW) : pin->SetPinState(HIGH);
				//pin->SetPinState(HIGH);
			}
		}
		else
		{
			pulse_flag = false;
			pulse_counter = 0;
			pulse_count = 0;
			(hasPullUp) ? pin->SetPinState(HIGH) : pin->SetPinState(LOW);
			//pin->SetPinState(LOW);
		}
	}
}


void Led_Device_Driver::Blink(uint32_t deltaTime) {
	blink_delta += deltaTime;
	if (blink_delta > blink_delay) {
		blink_delta = 0;
		if (blink_On_Off == true) {
			blink_On_Off = false;
			(hasPullUp) ? pin->SetPinState(HIGH) : pin->SetPinState(LOW);
			//pin->SetPinState(LOW);
		}
		else {
			blink_On_Off = true;
			(hasPullUp) ? pin->SetPinState(LOW) : pin->SetPinState(HIGH);
			//pin->SetPinState(HIGH);
		}
	}
}

void Led_Device_Driver::DoUpdate(uint32_t deltaTime) {
	if (pulse_flag == true) {
		Pulse(deltaTime);
	}

	if (blink_flag == true) {
		Blink(deltaTime);
	}
}

void Led_Device_Driver::Set_IO_Pin_High()
{
	(hasPullUp) ? pin->SetPinState(LOW) : pin->SetPinState(HIGH);
	//pin->SetPinState(HIGH);
	blink_flag = false;
}

void Led_Device_Driver::Set_IO_Pin_Low()
{
	(hasPullUp) ? pin->SetPinState(HIGH) : pin->SetPinState(LOW);
	//pin->SetPinState(LOW);
	blink_flag = false;
}

void Led_Device_Driver::Set_IO_Pin_Pulse(uint16_t count, uint16_t delay)
{
	if (!pulse_flag) {
		pulse_delay = delay;
		pulse_count = count;
		pulse_counter = 0;

		//Serial.print("Pulse- delay: ");
		//Serial.print(pulse_delay);
		//Serial.print(" counter: ");
		//Serial.println(pulse_count);

		pulse_flag = true;
		pulse_delta = 0;
		pulse_On_Off = true;
		(hasPullUp) ? pin->SetPinState(LOW) : pin->SetPinState(HIGH);
		//pin->SetPinState(HIGH);
	}
}

void Led_Device_Driver::Set_IO_Pin_Blink(uint16_t delay)
{
	blink_delay = delay;
	if (blink_flag == false) {
		blink_flag = true;
		blink_delta = 0;
		blink_On_Off = true;
		(hasPullUp) ? pin->SetPinState(LOW) : pin->SetPinState(HIGH);
		//pin->SetPinState(HIGH);
	}
}
