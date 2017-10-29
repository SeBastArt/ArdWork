#include "Button_Device_Driver.h"

Button_Device_Driver::Button_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool _hasPullUp, uint8_t priority) :
	Device_Driver(module, priority),
	__pin(_pin),
	__hasPullUp(_hasPullUp)
{
	__DriverType = BUTTON_DEVICE_DRIVER_TYPE;
}

void Button_Device_Driver::Build_Descriptor() {
	__descriptor->name = "Button";
	__descriptor->descr = "a simple button";
	__descriptor->published = false;

	Ctrl_Elem *ctrl_elem = new Ctrl_Elem(BUTTON_DEVICE_DRIVER_PUSH_BUTTON, "Press Me", button, "Press the button");
	ctrl_elem->published = true;

	__descriptor->Add_Descriptor_Element(ctrl_elem);
}

void Button_Device_Driver::DoAfterInit()
{
	__pin->IsActive = true;
	__pin->SetPinMode(INPUT);
	__sys_state = buttonstate_released;
	__last_state = buttonstate_released;
	__lastMessage = THREAD_MSG_BUTTONSTATE_RELEASED;
	Serial.println("Button-Driver initialized!");
}

void Button_Device_Driver::DoBeforeShutdown()
{
	//

}

void Button_Device_Driver::DoBeforeSuspend()
{
	//

}

void Button_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{

}

int Button_Device_Driver::GetButtonPinID() {
	return __pin->GetID();
}

void Button_Device_Driver::Exec_Push_Button()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(BUTTON_DEVICE_DRIVER_PUSH_BUTTON);
	PostMessage(&message);
}

void Button_Device_Driver::SetPullUp(bool _hasPullUp)
{
	__hasPullUp = _hasPullUp;
}

void Button_Device_Driver::DoUpdate(uint32_t deltaTime) {
	uint16_t deltaTimeMs = ThreadTimeToMs(deltaTime);
	Button_State new_State;
	if (__hasPullUp == true) {
		new_State = (__pin->PinState() == HIGH) ? buttonstate_released : buttonstate_pressed;
	}
	else {
		new_State = (__pin->PinState() == HIGH) ? buttonstate_pressed : buttonstate_released;
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
			if (__lastMessage != THREAD_MSG_BUTTONSTATE_RELEASED) {
				ButtonMessage* message = new ButtonMessage(__pin->GetID(), THREAD_MSG_BUTTONSTATE_RELEASED);
				if (!parentModule->SendAsyncThreadMessage(message))
				{
					Serial.println(">> message buffer overflow <<");
				}
				__lastMessage = THREAD_MSG_BUTTONSTATE_RELEASED;
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
				if (__lastMessage != THREAD_MSG_BUTTONSTATE_PRESSED) {
					ButtonMessage* message = new ButtonMessage(__pin->GetID(), THREAD_MSG_BUTTONSTATE_PRESSED);
					if (!parentModule->SendAsyncThreadMessage(message))
					{
						Serial.println(">> message buffer overflow <<");
					}
					__lastMessage = THREAD_MSG_BUTTONSTATE_PRESSED;
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
				if (__lastMessage != THREAD_MSG_BUTTONSTATE_AUTOREPEAT) {
					ButtonMessage* message = new ButtonMessage(__pin->GetID(), THREAD_MSG_BUTTONSTATE_AUTOREPEAT);
					if (!parentModule->SendAsyncThreadMessage(message))
					{
						Serial.println(">> message buffer overflow <<");
					}
					__lastMessage = THREAD_MSG_BUTTONSTATE_AUTOREPEAT;
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
					if (__lastMessage != THREAD_MSG_BUTTONSTATE_AUTOREPEAT) {
						ButtonMessage* message = new ButtonMessage(__pin->GetID(), THREAD_MSG_BUTTONSTATE_AUTOREPEAT);
						if (!parentModule->SendAsyncThreadMessage(message))
						{
							Serial.println(">> message buffer overflow <<");
						}
						__lastMessage = THREAD_MSG_BUTTONSTATE_AUTOREPEAT;
					}
				}
				else if (new_State == buttonstate_released) {
					__last_state = buttonstate_released;
					if (__lastMessage != THREAD_MSG_BUTTONSTATE_RELEASED) {
						ButtonMessage* message = new ButtonMessage(__pin->GetID(), THREAD_MSG_BUTTONSTATE_RELEASED);
						if (!parentModule->SendAsyncThreadMessage(message))
						{
							Serial.println(">> message buffer overflow <<");
						}
						__lastMessage = THREAD_MSG_BUTTONSTATE_RELEASED;
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
