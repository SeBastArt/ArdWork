// 
// 
// 

#include "Picture_Module_Driver.h"
#include "Picture_Module_Driver_Consts.h"
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "WebServer_Wifi_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"

Picture_Module_Driver::Picture_Module_Driver(uint8_t priority) :
	Module_Driver(priority)
{
	driver_name = "Picture_Module_Driver";
}

void Picture_Module_Driver::Build_Descriptor() {
	__descriptor->name = "Picture Module";
	__descriptor->descr = "control the picture";
	__descriptor->published = false;
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

void Picture_Module_Driver::DoThreadMessage(ThreadMessage * message)
{
	switch ((message)->Class) {
	case MessageClass_Float:
	{
		FloatMessage* pFMessage = (FloatMessage*)(message);
		Serial.print("ID: ");
		Serial.print(pFMessage->Id);
		Serial.print("; Lux: ");
		Serial.println(pFMessage->Value);
		break;
	}

	case MessageClass_Button:
	{
		ButtonMessage* pButton = (ButtonMessage*)(message);
		if (pButton->State == THREAD_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
		{
			if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {
				((Uart_GRBW_Led_Device_Driver *)Selected_Uart_GRBW_Led_Device)->Exec_Animation_Next();
				//String *msg = new String("1");
				//msg_list.push_back(msg);

			}
		}
		else if (pButton->State == THREAD_MSG_BUTTONSTATE_RELEASED)
		{
			if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {
				//String *msg = new String("0");
				//msg_list.push_back(msg);
			}
		}
		else if (pButton->State == THREAD_MSG_BUTTONSTATE_AUTOREPEAT)
		{
			if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {
				((Uart_GRBW_Led_Device_Driver *)Selected_Uart_GRBW_Led_Device)->Exec_Animation_Off();
				//String *msg = new String("2");
				//msg_list.push_back(msg);
			}
		}
		break;
	}
	}
}


void Picture_Module_Driver::DoModuleMessage(Int_Thread_Msg message)
{
	Serial.println("rennt");
	int messageID = message.GetID();
	switch (messageID)
	{
	case PICTURE_MODULE_DRIVER_PATTERN_NEXT:
	{
		Pattern_Next();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_PREV:
	{
		Pattern_Prev();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_OFF:
	{
		Pattern_Off();
	}
	break;
	}
}

void Picture_Module_Driver::Exec_Pattern_Next()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(PICTURE_MODULE_DRIVER_PATTERN_NEXT);
	PostMessage(&message);
}

void Picture_Module_Driver::Exec_Pattern_Prev()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(PICTURE_MODULE_DRIVER_PATTERN_PREV);
	PostMessage(&message);
}

void Picture_Module_Driver::Exec_Pattern_Off()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(PICTURE_MODULE_DRIVER_PATTERN_OFF);
	PostMessage(&message);
}

void Picture_Module_Driver::Pattern_Next()
{
	((Uart_GRBW_Led_Device_Driver *)Selected_Uart_GRBW_Led_Device)->Exec_Animation_Next();
}

void Picture_Module_Driver::Pattern_Prev()
{
	((Uart_GRBW_Led_Device_Driver *)Selected_Uart_GRBW_Led_Device)->Exec_Animation_Prev();
}

void Picture_Module_Driver::Pattern_Off()
{
	((Uart_GRBW_Led_Device_Driver *)Selected_Uart_GRBW_Led_Device)->Exec_Animation_Off();
}

