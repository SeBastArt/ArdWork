#include "Button_Device_Driver.h"

Button_Device_Driver::Button_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool _hasPullUp, uint8_t priority) :
	Device_Driver(module, priority),
	pin(_pin),
	hasPullUp(_hasPullUp)
{
	driver_name = "Button_Device_Driver";
	publisher->name = "Button";
	publisher->descr = "Push Me";
	Button_Publisher *elem = new Button_Publisher("Push", "Push the Button");
	elem->cmdId = BUTTON_DEVICE_DRIVER_PUSH_BUTTON;
	publisher->Add_Publisher_Element(elem);
	publisher->published = true;
}

void Button_Device_Driver::DoAfterInit()
{
	pin->IsActive = true;
	pin->SetPinMode(INPUT);
	Button_State sys_state = buttonstate_released;
	Button_State last_state = buttonstate_released;
	lastMessage = THREAD_MSG_BUTTONSTATE_RELEASED;
	last_tick = 0;
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

void Button_Device_Driver::DoExecuteCommand(String _command) {

}

void Button_Device_Driver::Do_Push_Button()
{
	ButtonMessage* message = new ButtonMessage(pin->GetID(), THREAD_MSG_BUTTONSTATE_PRESSED);
	if (!parentModule->SendAsyncThreadMessage(message))
	{
		Serial.println(">> message buffer overflow <<");
	}
	lastMessage = THREAD_MSG_BUTTONSTATE_PRESSED;
}



void Button_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{
	int messageID = message.GetID();
	switch (messageID)
	{
		case BUTTON_DEVICE_DRIVER_PUSH_BUTTON:
		{
			Do_Push_Button();
		}
		break;
	}
}

uint8_t Button_Device_Driver::GetButtonPinID() {
	return pin->GetID();
}

void Button_Device_Driver::Exec_Push_Button()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(BUTTON_DEVICE_DRIVER_PUSH_BUTTON);
	PostMessage(&message);
}

void Button_Device_Driver::SetPullUp(bool _hasPullUp)
{
	hasPullUp = _hasPullUp;
}

void Button_Device_Driver::DoUpdate(uint32_t deltaTime) {
	uint16_t deltaTimeMs = ThreadTimeToMs(deltaTime);
	Button_State new_State;
	if (hasPullUp == true) {
		new_State = (pin->PinState() == HIGH) ? buttonstate_released : buttonstate_pressed;
	}
	else
	{
		new_State = (pin->PinState() == HIGH) ? buttonstate_pressed : buttonstate_released;
	}
	if (new_State != last_state) {
		if (new_State == buttonstate_pressed) {
			// just read button down and start timer
			_timer = _debouceMs;
			sys_state = buttonstate_tracking;
			last_state = buttonstate_pressed;
		}
		else {
			sys_state = buttonstate_released;
			last_state = buttonstate_released;
			if (lastMessage != THREAD_MSG_BUTTONSTATE_RELEASED) {
				ButtonMessage* message = new ButtonMessage(pin->GetID(), THREAD_MSG_BUTTONSTATE_RELEASED);
				if (!parentModule->SendAsyncThreadMessage(message))
				{
					Serial.println(">> message buffer overflow <<");
				}
				lastMessage = THREAD_MSG_BUTTONSTATE_RELEASED;
			}
		}
	}
	else {
		switch (sys_state)
		{
		case buttonstate_tracking:
			if (deltaTimeMs >= _timer)
			{
				// press debounced 
				last_state = buttonstate_pressed;
				sys_state = buttonstate_pressed;
				_timer = _repeatDelayMs;
				if (lastMessage != THREAD_MSG_BUTTONSTATE_PRESSED) {
					ButtonMessage* message = new ButtonMessage(pin->GetID(), THREAD_MSG_BUTTONSTATE_PRESSED);
					if (!parentModule->SendAsyncThreadMessage(message))
					{
						Serial.println(">> message buffer overflow <<");
					}
					lastMessage = THREAD_MSG_BUTTONSTATE_PRESSED;
				}
			}
			else
			{
				_timer -= deltaTimeMs;
			}
			break;

		case buttonstate_pressed:
			if (deltaTimeMs >= _timer)
			{
				// auto repeat started
				sys_state = buttonstate_autorepeat;
				last_state = buttonstate_pressed;
				_timer = _repeatRateMs;
				if (lastMessage != THREAD_MSG_BUTTONSTATE_AUTOREPEAT) {
					ButtonMessage* message = new ButtonMessage(pin->GetID(), THREAD_MSG_BUTTONSTATE_AUTOREPEAT);
					if (!parentModule->SendAsyncThreadMessage(message))
					{
						Serial.println(">> message buffer overflow <<");
					}
					lastMessage = THREAD_MSG_BUTTONSTATE_AUTOREPEAT;
				}
			}
			else
			{
				_timer -= deltaTimeMs;
			}
			break;

		case buttonstate_autorepeat:
			if (deltaTimeMs >= _timer)
			{
				sys_state = buttonstate_autorepeat;
				_timer += _repeatRateMs;

				if (new_State == buttonstate_pressed) {
					last_state = buttonstate_pressed;
					if (lastMessage != THREAD_MSG_BUTTONSTATE_AUTOREPEAT) {
						ButtonMessage* message = new ButtonMessage(pin->GetID(), THREAD_MSG_BUTTONSTATE_AUTOREPEAT);
						if (!parentModule->SendAsyncThreadMessage(message))
						{
							Serial.println(">> message buffer overflow <<");
						}
						lastMessage = THREAD_MSG_BUTTONSTATE_AUTOREPEAT;
					}
				}
				else if (new_State == buttonstate_released) {
					last_state = buttonstate_released;
					if (lastMessage != THREAD_MSG_BUTTONSTATE_RELEASED) {
						ButtonMessage* message = new ButtonMessage(pin->GetID(), THREAD_MSG_BUTTONSTATE_RELEASED);
						parentModule->SendAsyncThreadMessage(message);
						if (!parentModule->SendAsyncThreadMessage(message))
						{
							Serial.println(">> message buffer overflow <<");
						}
						lastMessage = THREAD_MSG_BUTTONSTATE_RELEASED;
					}
				}
			}
			else
			{
				_timer -= deltaTimeMs;
			}
			break;
		}
	}
}
