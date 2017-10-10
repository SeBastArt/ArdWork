// 
// 
// 

#include "RGBLed_Mqqt_Wifi_Module_Driver.h"

#include "Led_Device_Driver.h"
#include "Button_Device_Driver.h"
#include "Uart_RGB_Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"

RGBLed_Mqqt_Wifi_Module_Driver::RGBLed_Mqqt_Wifi_Module_Driver(String _hostname, String _ssid, String _password, uint8_t priority) : Mqqt_Wifi_Module_Driver(_hostname, _ssid, _password, priority)
{
	resident_button = NULL;
	resident_Led = NULL;
	resident_strip = NULL;
	resident_luxmeter = NULL;
}

void RGBLed_Mqqt_Wifi_Module_Driver::Build_Descriptor() {
}


void RGBLed_Mqqt_Wifi_Module_Driver::UpdateHardware(uint32_t deltaTime)
{
	if ((resident_button != NULL) && (resident_Led != NULL)) {
		ThreadMessage* pMessage;
		if (PopMessage(&pMessage))
		{
			switch ((pMessage)->Class){
				case MessageClass_Float:
				{
					FloatMessage* pFMessage = (FloatMessage*)(pMessage);
					Serial.print("Lux: ");
					Serial.println(pFMessage->Value);
					break;
				}
			
				case MessageClass_Button:
				{
					ButtonMessage* pButton = (ButtonMessage*)(pMessage);
					if (pButton->State == THREAD_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
					{
						if (pButton->Id == resident_button->GetButtonPinID()) {
							resident_strip->Exec_Animation_Next();
							//resident_strip->Exec_Animation_Color(random(0, 255), random (0, 255), random(0, 255));
							String *msg = new String("1");
							msg_list.PushBack(msg);
						}
					}
					else if (pButton->State == THREAD_MSG_BUTTONSTATE_RELEASED)
					{
						if (pButton->Id == resident_button->GetButtonPinID()) {
							String *msg = new String("0");
							msg_list.PushBack(msg);
						}
					}
					else if (pButton->State == THREAD_MSG_BUTTONSTATE_AUTOREPEAT)
					{
						if (pButton->Id == resident_button->GetButtonPinID()) {
							resident_strip->Exec_Animation_Off();
							String *msg = new String("2");
							msg_list.PushBack(msg);
						}
					}
					break;
				}
			}
		}
	}
}

void RGBLed_Mqqt_Wifi_Module_Driver::DoMQTTMessage(String _message) {
	if (Find_text("on", _message) > -1) {
		resident_strip->Exec_Animation_Next();
	}
	if (Find_text("off", _message) > -1) {
		resident_strip->Exec_Animation_Off();
	}
	if (Find_text("rgb", _message) > -1) {
		String sub_message = _message.substring(_message.indexOf(" ") + 1);
		long long number = strtol(&sub_message[0], NULL, 16);

		// Split them up into r, g, b values
		int r = number >> 16;
		int g = number >> 8 & 0xFF;
		int b = number & 0xFF;
		resident_strip->Exec_Animation_Color(r, g, b);
	}
}

int RGBLed_Mqqt_Wifi_Module_Driver::Find_text(String needle, String haystack) {
	int foundpos = -1;
	if (haystack.length() >= needle.length()) {
		for (int i = 0; i <= haystack.length() - needle.length(); i++) {
			if (haystack.substring(i, needle.length() + i) == needle) {
				foundpos = i;
			}
		}
	}
	return foundpos;
}

void RGBLed_Mqqt_Wifi_Module_Driver::AddComponent(Button_Device_Driver *button) {
	AddDevice(button);
	resident_button = button;
}

void RGBLed_Mqqt_Wifi_Module_Driver::AddComponent(Led_Device_Driver *led) {
	AddDevice(led);
	resident_Led = led;
}

void RGBLed_Mqqt_Wifi_Module_Driver::AddComponent(Uart_RGB_Led_Device_Driver *strip) {
	AddDevice(strip);
	resident_strip = strip;
}

void RGBLed_Mqqt_Wifi_Module_Driver::AddComponent(Luxmeter_Device_Driver *luxmeter) {
	AddDevice(luxmeter);
	resident_luxmeter = luxmeter;
}
