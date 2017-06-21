// 
// 
// 

#include "Picture_Module_Driver.h"
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "WebServer_Wifi_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"

Picture_Module_Driver::Picture_Module_Driver(uint8_t priority)
{
	driver_name = "Picture_Module_Driver";
}


void Picture_Module_Driver::DoBeforeSuspend()
{
}

void Picture_Module_Driver::DoBeforeShutdown()
{
}

void Picture_Module_Driver::DoAfterInit()
{
}

void Picture_Module_Driver::DoModuleMessage(Int_Thread_Msg message)
{

}

void Picture_Module_Driver::DoUpdate(uint32_t deltaTime)
{
	Vector <Control*> *ctrls = GetControlsList();
	((WebServer_Wifi_Device_Driver *)Selected_WebServer_Wifi_Device)->control_list = ctrls;

	if ((Selected_Button_Device != NULL) && (Selected_Led_Device != NULL)) {

		ThreadMessage *pMessage;
		if (PopMessage(&pMessage))
		{
			UpdateControls();
			switch ((pMessage)->Class) {
			case MessageClass_Lux:
			{
				LuxMessage* pLux = (LuxMessage*)(pMessage);
				Serial.print("Lux: ");
				Serial.println(pLux->Lux);
				break;
			}

			case MessageClass_Server:
			{
				ServerMessage* pServer = (ServerMessage*)(pMessage);
				if (pServer->sCmd == "FUNCTION1ON") {
					((Led_Device_Driver *)Selected_Led_Device)->Exec_Set_IO_Pin_High();
				}
				else if (pServer->sCmd == "FUNCTION1OFF") {
					((Led_Device_Driver *)Selected_Led_Device)->Exec_Set_IO_Pin_Low();
				}
				break;
			}
			case MessageClass_Button:
			{
				ButtonMessage* pButton = (ButtonMessage*)(pMessage);
				if (pButton->State == THREAD_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
				{
					if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {
						((Uart_GRBW_Led_Device_Driver *)Selected_Uart_GRBW_Led_Device)->Exec_Animation_Next();
						//resident_strip->Exec_Animation_Color(random(0, 255), random (0, 255), random(0, 255));
						String *msg = new String("1");
						//msg_list.push_back(msg);

					}
				}
				else if (pButton->State == THREAD_MSG_BUTTONSTATE_RELEASED)
				{
					if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {
						String *msg = new String("0");
						//msg_list.push_back(msg);
					}
				}
				else if (pButton->State == THREAD_MSG_BUTTONSTATE_AUTOREPEAT)
				{
					if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {


						((Uart_GRBW_Led_Device_Driver *)Selected_Uart_GRBW_Led_Device)->Exec_Animation_Off();
						String *msg = new String("2");
						//msg_list.push_back(msg);
					}
				}
				break;
			}
			}
			if (pMessage != NULL) {
				delete pMessage; //objekt löschen 
				pMessage = NULL;
			}
		}
	}


}
