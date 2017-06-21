// 
// 
// 

#include "Dash_Mqqt_Wifi_Module_Driver.h"
#include "Led_Device_Driver.h"
#include "Button_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"

Dash_Mqqt_Wifi_Module_Driver::Dash_Mqqt_Wifi_Module_Driver(String _hostname, String _ssid, String _password, uint8_t priority) : Mqqt_Wifi_Module_Driver(_hostname, _ssid, _password, priority)
{
	resident_button = NULL;
	resident_Led = NULL;
}

void Dash_Mqqt_Wifi_Module_Driver::UpdateHardware(uint32_t deltaTime)
{
	if ((resident_button != NULL) && (resident_Led != NULL)) {
		ThreadMessage* pMessage;
		if (PopMessage(&pMessage))
		{
			if ((pMessage)->Class == MessageClass_Button)
			{
				ButtonMessage* pButton = (ButtonMessage*)(pMessage);
				if (pButton->State == THREAD_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
				{
					if (pButton->Id == resident_button->GetButtonPinID()) {
						resident_Led->Exec_Set_IO_Pin_High();
						String *msg = new String("1");
						msg_list.PushBack(msg);
					}
				}
				else if (pButton->State == THREAD_MSG_BUTTONSTATE_RELEASED)
				{
					if (pButton->Id == resident_button->GetButtonPinID()) {
						resident_Led->Exec_Set_IO_Pin_Low();
					}
					String *msg = new String("0");
					msg_list.PushBack(msg);
				}
				else if (pButton->State == THREAD_MSG_BUTTONSTATE_AUTOREPEAT)
				{
					if (pButton->Id == resident_button->GetButtonPinID()) {
						String *msg = new String("2");
						msg_list.PushBack(msg);
					}
				}
			}
		}
	}
}


void Dash_Mqqt_Wifi_Module_Driver::DoMQTTMessage(String _message) {
	if (_message.equals("2")) {
		resident_Led->Exec_Set_IO_Pin_Pulse(7, 100);
	}
	if (_message.equals("1")) {
		resident_Led->Exec_Set_IO_Pin_High();
	}
	if (_message.equals("0")) {
		resident_Led->Exec_Set_IO_Pin_Low();
	}
}




void Dash_Mqqt_Wifi_Module_Driver::AddComponent(Button_Device_Driver *button) {
	AddDevice(button);
	resident_button = button;
}

void Dash_Mqqt_Wifi_Module_Driver::AddComponent(Led_Device_Driver *led) {
	AddDevice(led);
	resident_Led = led;
}