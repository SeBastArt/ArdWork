#include "Button_Device_Driver.h"

REGISTERIMPL(Button_Device_Driver);

Button_Device_Driver::Button_Device_Driver(Module_Driver* module, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = BUTTON_DEVICE_DRIVER_TYPE;
	__pin = nullptr;
}

void Button_Device_Driver::OnBuild_Descriptor() {
	__descriptor->name = F("Button");
	__descriptor->descr = F("a simple button");
	__descriptor->published = false;
	
	Group_CtrlElem *ctrlElem_Press = new Group_CtrlElem(BUTTON_DEVICE_DRIVER_PUSH_BUTTON, F("Virtual Button"), F("You can press it, if you want"));
	ctrlElem_Press->AddMember(F("Press"));

	__descriptor->Add_Descriptor_Element(ctrlElem_Press);
}

void Button_Device_Driver::SetPin(IO_Pin* _pin)
{
	__pin = _pin;
	__pin->pinMode = INPUT;
}

void Button_Device_Driver::SetPullUp()
{
	__hasPullUp = true;
}

void Button_Device_Driver::SetNoPullUp()
{
	__hasPullUp = false;
}


void Button_Device_Driver::OnInit()
{
	Device_Driver::OnInit();
	__sys_state = buttonstate_released;
	__last_state = buttonstate_released;
	__lastMessage = BUTTON_DEVICE_BUTTONSTATE_RELEASED;
}


void Button_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case BUTTON_DEVICE_DRIVER_PUSH_BUTTON:
	{
#ifdef DEBUG
		Serial.println("Start Button_Device_Driver::DoDeviceMessage - BUTTON_DEVICE_DRIVER_PUSH_BUTTON");
#endif // DEBUG
		Push_Button();
	}
	break;
	}
#ifdef DEBUG
	Serial.println("Ende Button_Device_Driver::DoDeviceMessage");
#endif // DEBUG
}

int Button_Device_Driver::GetButtonPinID() {
	return __pin->pinID;
}

void Button_Device_Driver::Exec_Push_Button()
{
	Int_Task_Msg *message = new Int_Task_Msg(BUTTON_DEVICE_DRIVER_PUSH_BUTTON);
	PostMessage(&message);
}

void Button_Device_Driver::Push_Button()
{
	if (__hasPullUp == true) {
		__pin->pinState == LOW;
	}
	else {
		__pin->pinState == HIGH;
	}
}


void Button_Device_Driver::DoUpdate(uint32_t deltaTime) {
	uint16_t deltaTimeMs = TaskTimeToMs(deltaTime);
	Button_State new_State;
	if (__pin == nullptr)
		return;

	if (__hasPullUp == true) {
		new_State = (__pin->pinState == HIGH) ? buttonstate_released : buttonstate_pressed;
	}
	else {
		new_State = (__pin->pinState == HIGH) ? buttonstate_pressed : buttonstate_released;
	}
	if (new_State != __last_state) {
		if (new_State == buttonstate_pressed) {
			// just read button down and start timer
			__timer = __debouceMs;
			__sys_state = buttonstate_tracking;
			__last_state = buttonstate_pressed;
		}
		else {
			__sys_state = buttonstate_released;
			__last_state = buttonstate_released;
			if (__lastMessage != BUTTON_DEVICE_BUTTONSTATE_RELEASED) {
				ButtonMessage* message = new ButtonMessage(__pin->pinID, BUTTON_DEVICE_BUTTONSTATE_RELEASED);
				if (!__parentModule->SendAsyncTaskMessage(message))
				{
					Serial.println(F(">> message buffer overflow <<"));
				}
				__lastMessage = BUTTON_DEVICE_BUTTONSTATE_RELEASED;
			}
		}
	}
	else {
		switch (__sys_state)
		{
		case buttonstate_tracking:
			if (deltaTimeMs >= __timer)
			{
				// press debounced 
				__last_state = buttonstate_pressed;
				__sys_state = buttonstate_pressed;
				__timer = __repeatDelayMs;
				if (__lastMessage != BUTTON_DEVICE_BUTTONSTATE_PRESSED) {
					ButtonMessage* message = new ButtonMessage(__pin->pinID, BUTTON_DEVICE_BUTTONSTATE_PRESSED);
					if (!__parentModule->SendAsyncTaskMessage(message))
					{
						Serial.println(F(">> message buffer overflow <<"));
					}
					__lastMessage = BUTTON_DEVICE_BUTTONSTATE_PRESSED;
				}
			}
			else
			{
				__timer -= deltaTimeMs;
			}
			break;

		case buttonstate_pressed:
			if (deltaTimeMs >= __timer)
			{
				// auto repeat started
				__sys_state = buttonstate_autorepeat;
				__last_state = buttonstate_pressed;
				__timer = __repeatRateMs;
				if (__lastMessage != BUTTON_DEVICE_BUTTONSTATE_AUTOREPEAT) {
					ButtonMessage* message = new ButtonMessage(__pin->pinID, BUTTON_DEVICE_BUTTONSTATE_AUTOREPEAT);
					if (!__parentModule->SendAsyncTaskMessage(message))
					{
						Serial.println(F(">> message buffer overflow <<"));
					}
					__lastMessage = BUTTON_DEVICE_BUTTONSTATE_AUTOREPEAT;
				}
			}
			else
			{
				__timer -= deltaTimeMs;
			}
			break;

		case buttonstate_autorepeat:
			if (deltaTimeMs >= __timer)
			{
				__sys_state = buttonstate_autorepeat;
				__timer += __repeatRateMs;

				if (new_State == buttonstate_pressed) {
					__last_state = buttonstate_pressed;
					if (__lastMessage != BUTTON_DEVICE_BUTTONSTATE_AUTOREPEAT) {
						ButtonMessage* message = new ButtonMessage(__pin->pinID, BUTTON_DEVICE_BUTTONSTATE_AUTOREPEAT);
						if (!__parentModule->SendAsyncTaskMessage(message))
						{
							Serial.println(F(">> message buffer overflow <<"));
						}
						__lastMessage = BUTTON_DEVICE_BUTTONSTATE_AUTOREPEAT;
					}
				}
				else if (new_State == buttonstate_released) {
					__last_state = buttonstate_released;
					if (__lastMessage != BUTTON_DEVICE_BUTTONSTATE_RELEASED) {
						ButtonMessage* message = new ButtonMessage(__pin->pinID, BUTTON_DEVICE_BUTTONSTATE_RELEASED);
						if (!__parentModule->SendAsyncTaskMessage(message))
						{
							Serial.println(F(">> message buffer overflow <<"));
						}
						__lastMessage = BUTTON_DEVICE_BUTTONSTATE_RELEASED;
					}
				}
			}
			else
			{
				__timer -= deltaTimeMs;
			}
			break;
		}
	}
}

