#include "Led_Device_Driver.h"
#include "ESP8266_NodeMCU_Controller.h"
#include <mutex>
REGISTERIMPL(Led_Device_Driver);

Led_Device_Driver::Led_Device_Driver(Module_Driver* module, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = LED_DEVICE_DRIVER_TYPE;
};

void Led_Device_Driver::OnBuild_Descriptor() {
	__descriptor->name = F("LED");
	__descriptor->descr = F("a simple LED");
	__descriptor->published = true;

	Select_CtrlElem *ctrlElem_mode = new Select_CtrlElem(LED_DEVICE_DRIVER_LED_SET_MODE, &__sv_mode, F("LED-Mode"), F("Select the Mode of the LED"));
	ctrlElem_mode->AddMember(F("On"));
	ctrlElem_mode->AddMember(F("OFF"));
	ctrlElem_mode->AddMember(F("Blink"));
	ctrlElem_mode->AddMember(F("Pulse"));

	FValue_CtrlElem *ctrlElem_delay = new FValue_CtrlElem(LED_DEVICE_DRIVER_LED_SET_DELAY, &__sv_delay, true, F("Delay"), F("Blink Delay is the delay between status change On/Off"));
	ctrlElem_delay->unit = F("ms");

	FValue_CtrlElem *ctrlElem_pulse_count = new FValue_CtrlElem(LED_DEVICE_DRIVER_LED_SET_PULSE_COUNT, &__sv_pulse_count, true, F("Pulse count"), F("change the count how often die led is blinking during pulse"));

	__descriptor->Add_Descriptor_Element(ctrlElem_mode);
	__descriptor->Add_Descriptor_Element(ctrlElem_delay);
	__descriptor->Add_Descriptor_Element(ctrlElem_pulse_count);
}


void Led_Device_Driver::SetPullUp()
{
	__hasPullUp = true;
}

void Led_Device_Driver::SetNoPullUp()
{
	__hasPullUp = false;
}


void Led_Device_Driver::SetPin(IO_Pin* _pin)
{
	__pin = _pin;
	__pin->pinMode = OUTPUT;
	Set_IO_Pin_Low();
}

void Led_Device_Driver::OnInit()
{
	Device_Driver::OnInit();
	__blink_flag = false;
	__sv_delay = 500;
	__blink_delta = 0;

	__pulse_flag = false;
	__pulse_delta = 0;
	__pulse_counter = 0;
	__sv_pulse_count = 5;

	Serial.println(F("Led-Driver initialized!"));
}


void Led_Device_Driver::Exec_Set_Led_On() {
	Int_Task_Msg *message = new Int_Task_Msg(LED_DEVICE_DRIVER_LED_ON);
	PostMessage(&message);
}


void Led_Device_Driver::Exec_Set_Led_Off() {
	Int_Task_Msg *message = new Int_Task_Msg(LED_DEVICE_DRIVER_LED_OFF);
	PostMessage(&message);
}

void Led_Device_Driver::Exec_Set_Led_Blink(int delay) {
	Int_Task_Msg *message = new Int_Task_Msg(LED_DEVICE_DRIVER_LED_BLINK);
	message->AddParam(delay);
	PostMessage(&message);
}

void Led_Device_Driver::Exec_Set_Led_Pulse(int count, int delay) {
	Int_Task_Msg *message = new Int_Task_Msg(LED_DEVICE_DRIVER_LED_PULSE);
	message->AddParam(count);
	message->AddParam(delay);
	PostMessage(&message);
}


void Led_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
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
		int blink_delay = message.GetIntParamByIndex(0);
		Set_Led_Blink(blink_delay);
	}
	break;
	case LED_DEVICE_DRIVER_LED_PULSE:
	{
		int pulse_count = message.GetIntParamByIndex(0);
		int pulse_delay = message.GetIntParamByIndex(1);
		Set_Led_Pulse(pulse_count, pulse_delay);
	}
	break;
	case LED_DEVICE_DRIVER_LED_SET_DELAY:
	{
		int delay = message.GetIntParamByIndex(0);
		Set_Led_Delay(delay);
	}
	break;
	case LED_DEVICE_DRIVER_LED_SET_PULSE_COUNT:
	{
		int pulse_count = message.GetIntParamByIndex(0);
		Set_Led_Pulse_Count(pulse_count);
	}
	break;
	case LED_DEVICE_DRIVER_LED_SET_MODE:
	{
		int mode = message.GetIntParamByIndex(0);
		Set_Led_Mode(mode);
	}
	}
}

void Led_Device_Driver::Set_Led_Delay(int _delay) {
	if ((_delay >= 200) && (_delay < 60000)) {
		__sv_delay = _delay;
	}
}

void Led_Device_Driver::Set_Led_Pulse_Count(int _pulse_count) {
	if ((_pulse_count > 0) && (_pulse_count < 1000)) {
		__sv_pulse_count = _pulse_count;
	}
}

void Led_Device_Driver::Set_Led_Mode(uint8_t _mode) {

	__sv_mode = _mode;

	switch (__sv_mode)
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
		Set_Led_Blink(__sv_delay);
	}
	break;
	case 3:
	{
		Set_Led_Pulse(__sv_pulse_count, __sv_delay);
	}
	break;
	}
}

void Led_Device_Driver::Pulse(uint32_t deltaTime) {
	__pulse_delta += deltaTime;
	if (__pulse_delta > __sv_delay) {
		__pulse_delta = 0;
		if (__pulse_counter < __sv_pulse_count) {
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
	if (__blink_delta > __sv_delay) {
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
	if (__pin == nullptr)
		return;

	(__hasPullUp) ? (__pin->pinState = LOW) : (__pin->pinState = HIGH) ;
}

void Led_Device_Driver::Set_Led_Off() {
	Set_IO_Pin_Low();
	__blink_flag = false;
	__pulse_flag = false;
}

void Led_Device_Driver::Set_IO_Pin_Low()
{
	__ledStatus = false;
	if (__pin == nullptr)
		return;
	(__hasPullUp) ? (__pin->pinState =  HIGH) : (__pin->pinState = LOW);
}

void Led_Device_Driver::Set_Led_Pulse(int count, int delay)
{
	if (!__pulse_flag) {
		__sv_delay = delay;
		__sv_pulse_count = count;
		__pulse_counter = 0;
		__pulse_flag = true;
		__blink_flag = false;
		__pulse_delta = 0;
		Set_IO_Pin_Low();
	}
}

void Led_Device_Driver::Set_Led_Blink(int delay)
{
	__sv_delay = delay;
	if (__blink_flag == false) {
		__blink_flag = true;
		__pulse_flag = false;
		__blink_delta = 0;
		Set_IO_Pin_Low();
	}
}

