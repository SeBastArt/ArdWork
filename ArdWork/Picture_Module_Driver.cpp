// 
// 
// 

#include "Picture_Module_Driver.h"
#include "Picture_Module_Driver_Consts.h"
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "Uart_RGB_Led_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"

Picture_Module_Driver::Picture_Module_Driver(uint8_t priority) :
	Module_Driver(priority)
{
	__DriverType = PICTURE_MODULE_DRIVER_TYPE;
}

void Picture_Module_Driver::Build_Descriptor() {
	__descriptor->name = "Bild";
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
		if (pFMessage->Id == Get_Luxmeter_DevDrv(0)->DriverId ) {
			Get_Uart_RGB_Led_DevDrv(0)->Exec_Set_Brightness (round(pFMessage->Value));
		}
		break;
	}

	case MessageClass_Button:
	{
		ButtonMessage* pButton = (ButtonMessage*)(message);
		if (pButton->State == THREAD_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				//Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Next();
			}
		}
		else if (pButton->State == THREAD_MSG_BUTTONSTATE_RELEASED)
		{
			//if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {
			//}
		}
		else if (pButton->State == THREAD_MSG_BUTTONSTATE_AUTOREPEAT)
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				//Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Off();
			}
		}
		break;
	}
	}
}


void Picture_Module_Driver::DoModuleMessage(Int_Thread_Msg message)
{
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
	Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Next();
}

void Picture_Module_Driver::Pattern_Prev()
{
	Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Prev();
}

void Picture_Module_Driver::Pattern_Off()
{
	Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Off();
}

