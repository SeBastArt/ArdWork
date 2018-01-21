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

//#define DEBUG

Picture_Module_Driver::Picture_Module_Driver(uint8_t priority) :
	Module_Driver(priority)
{
#ifdef  DEBUG
	Serial.print("Start Picture_Module_Driver with ID: ");
	Serial.println(this->DriverId);
#endif //  DEBUG
	__DriverType = PICTURE_MODULE_DRIVER_TYPE;
	__absBrightness = 50;
}

void Picture_Module_Driver::Build_Module_Discriptor() {
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::Build_Module_Discriptor");
#endif //  DEBUG	
	__descriptor->name = F("Light Control");
	__descriptor->descr = F("control the behavior of the ambilight");
	__descriptor->published = true;

	Group_CtrlElem *ctrlElem_buttons = new Group_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_SWITCH, F("Switch Pattern"), F("Switch the pattern backward, forward or turn it off"));
	ctrlElem_buttons->AddMember("Prev");
	ctrlElem_buttons->AddMember("Off");
	ctrlElem_buttons->AddMember("Next");

	Color_CtrlElem *ctrlElem_color = new Color_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_COLOR, &__sv_color, F("Color"), F("The main color for the ambient light pattern"));

	Select_CtrlElem *ctrlElem_autobrightness = new Select_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS, &__sv_autoBrightness, F("Auto Brightness"), F("set the brightness automatically"));
	ctrlElem_autobrightness->AddMember("On");
	ctrlElem_autobrightness->AddMember("Off");

	Value_CtrlElem *ctrlElem_brightess = new Value_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_REL_BRIGHTNESS, &__sv_relBrightness, true, F("Brightness"), F("the brightness for the ambient light from 1% to 200%"));
	ctrlElem_brightess->unit = "%";

	__descriptor->Add_Descriptor_Element(ctrlElem_buttons);
	__descriptor->Add_Descriptor_Element(ctrlElem_color);
	__descriptor->Add_Descriptor_Element(ctrlElem_autobrightness);
	__descriptor->Add_Descriptor_Element(ctrlElem_brightess);
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::Build_Module_Discriptor");
#endif //  DEBUG
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
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::DoThreadMessage");
#endif //  DEBUG
	switch ((message)->Class) {
	case MessageClass_Float:
	{
		FloatMessage* pFMessage = (FloatMessage*)(message);
		if (pFMessage->__id == Get_Luxmeter_DevDrv(0)->DriverId) {
			__absBrightness = pFMessage->__absValue;
			//SetStripBrightness();
		}
		break;
	}

	case MessageClass_Button:
	{
		ButtonMessage* pButton = (ButtonMessage*)(message);
		if (pButton->State == THREAD_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				Pattern_Next();
			}
		}
		else if (pButton->State == THREAD_MSG_BUTTONSTATE_RELEASED)
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				//
			}
		}
		else if (pButton->State == THREAD_MSG_BUTTONSTATE_AUTOREPEAT)
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				Pattern_Off();
			}
		}
		break;
	}
	}
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::DoThreadMessage");
#endif //  DEBUG
}

void Picture_Module_Driver::DoModuleMessage(Int_Thread_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case PICTURE_MODULE_DRIVER_PATTERN_NEXT:
	{
#ifdef  DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_NEXT");
#endif //  DEBUG
		Pattern_Next();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_PREV:
	{
#ifdef  DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_PREV");
#endif //  DEBUG
		Pattern_Prev();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_OFF:
	{
#ifdef  DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_OFF");
#endif //  DEBUG
		Pattern_Off();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_SWITCH:
	{
#ifdef  DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_SWITCH");
#endif //  DEBUG
		uint8_t direction = message.GetIntParamByIndex(0);
		SwitchPattern(direction);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS:
	{
#ifdef  DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS");
#endif //  DEBUG
		bool state = message.GetIntParamByIndex(0);
		Set_Auto_Brightness(state);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_REL_BRIGHTNESS:
	{
#ifdef  DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_REL_BRIGHTNESS");
#endif //  DEBUG
		int brightness = message.GetIntParamByIndex(0);
		SetRelBrightness(brightness);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_COLOR:
	{
#ifdef  DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_COLOR");
#endif //  DEBUG
		uint32_t rgb = (uint32_t)strtol(message.GetStringParamByIndex(0).c_str(), NULL, 16);
		__sv_color.R = (uint8_t)((rgb >> 16) & 0xFF);
		__sv_color.G = (uint8_t)((rgb >> 8) & 0xFF);
		__sv_color.B = (uint8_t)((rgb >> 0) & 0xFF);
		Set_Pattern_Color(__sv_color.R, __sv_color.R, __sv_color.R);
	}
	break;
	}
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::DoModuleMessage");
#endif //  DEBUG
}


void Picture_Module_Driver::SwitchPattern(uint8_t _control) {
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::SwitchPattern");
#endif //  DEBUG
	switch (_control)
	{
	case 0:
	{
		Pattern_Prev();
	}
	break;
	case 1:
	{
		Pattern_Off();
	}
	break;
	case 2:
	{
		Pattern_Next();
	}
	break;
	}
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::SwitchPattern");
#endif //  DEBUG
}

void Picture_Module_Driver::SetStripBrightness() {
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::SetStripBrightness");
#endif //  DEBUG
	uint8_t brightness;
	if (__sv_autoBrightness > 0) {
		float faktor = 200.0 / __sv_relBrightness;
		brightness = round(MIN(60.0, MAX(5.0, (__absBrightness * 100.0) / (200.0 * faktor))));
	}
	else {
		brightness = round(__sv_relBrightness / 2);
	}
	
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Set_Brightness(brightness);

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Brightness(brightness);
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::SetStripBrightness");
#endif //  DEBUG
}

void Picture_Module_Driver::SetRelBrightness(uint8_t _relBrightness)
{
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::SetRelBrightness");
#endif //  DEBUG
	__sv_relBrightness = MAX(1, MIN(200, _relBrightness));
	SetStripBrightness();
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::SetRelBrightness");
#endif //  DEBUG
}

void Picture_Module_Driver::Set_Auto_Brightness(bool _state) {
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::Set_Auto_Brightness");
#endif //  DEBUG	
	__sv_autoBrightness = (int)_state;
	SetStripBrightness();
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::Set_Auto_Brightness");
#endif //  DEBUG	
}

void Picture_Module_Driver::Set_Pattern_Color(int _r, int _g, int _b) {
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::Set_Pattern_Color");
#endif //  DEBUG	
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Color(_r, _g, _b);

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Color(_r, _g, _b);
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::Set_Pattern_Color");
#endif //  DEBUG	
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
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::Pattern_Next");
#endif //  DEBUG	
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr) 
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Next();


	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr) 
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Next();
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::Pattern_Next");
#endif //  DEBUG	
}

void Picture_Module_Driver::Pattern_Prev()
{
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::Pattern_Prev");
#endif //  DEBUG	
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Prev();

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Prev();
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::Pattern_Prev");
#endif //  DEBUG	
}

void Picture_Module_Driver::Pattern_Off()
{
#ifdef  DEBUG
	Serial.println("Start Picture_Module_Driver::Pattern_Off");
#endif //  DEBUG	
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Off();

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Off();
#ifdef  DEBUG
	Serial.println("Ende Picture_Module_Driver::Pattern_Off");
#endif //  DEBUG	
}

