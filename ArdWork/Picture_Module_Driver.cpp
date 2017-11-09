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
	__autoBrightness = false;
	__brightness = 50;
}

void Picture_Module_Driver::Build_Module_Discriptor() {
	__descriptor->name = F("Light Control");
	__descriptor->descr = F("control the behavior of the ambilight");

	Ctrl_Elem *ctrl_elem_buttons = new Ctrl_Elem(PICTURE_MODULE_DRIVER_PATTERN_SWITCH_EXTERN, F("Switch Pattern"), button_group, F("Switch the pattern backward, forward or turn it off"));
	Atomic<String> *atomic_prev = new Atomic<String>(0, F("Previous"));
	Atomic<String> *atomic_off = new Atomic<String>(1, F("Off"));
	Atomic<String> *atomic_next = new Atomic<String>(2, F("Next"));
	ctrl_elem_buttons->AddAtomic(atomic_prev);
	ctrl_elem_buttons->AddAtomic(atomic_off);
	ctrl_elem_buttons->AddAtomic(atomic_next);
	ctrl_elem_buttons->published = true;

	Ctrl_Elem *ctrl_elem_autobrightness = new Ctrl_Elem(PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS_EXTERN, F("Auto Brightness"), select, F("set the brightness automatically"));
	Atomic<String> *atomic_autobrightness_off = new Atomic<String>(0, F("Off"));
	Atomic<String> *atomic_autobrightness_on = new Atomic<String>(1, F("On"));
	ctrl_elem_autobrightness->AddAtomic(atomic_autobrightness_on);
	ctrl_elem_autobrightness->AddAtomic(atomic_autobrightness_off);
	ctrl_elem_autobrightness->published = true;

	Ctrl_Elem *ctrl_elem_brightess = new Ctrl_Elem(PICTURE_MODULE_DRIVER_PATTERN_BRIGHTNESS_EXTERN, F("brightness"), edtvalue, F("the brightness for the ambient light"));
	Atomic<uint8_t> *atomic_brightess = new Atomic<uint8_t>(0, &__brightness, F("%"));
	ctrl_elem_brightess->AddAtomic(atomic_brightess);
	ctrl_elem_brightess->published = true;

	Ctrl_Elem *ctrl_elem_color = new Ctrl_Elem(PICTURE_MODULE_DRIVER_PATTERN_COLOR_EXTERN, F("Color"), color, F("The main color for the ambient light pattern"));
	Atomic<uint8_t> *atomic_color_r = new Atomic<uint8_t>(0, 220, F("Dec"));
	Atomic<uint8_t> *atomic_color_g = new Atomic<uint8_t>(1, 123, F("Dec"));
	Atomic<uint8_t> *atomic_color_b = new Atomic<uint8_t>(2, 234, F("Dec"));

	ctrl_elem_color->AddAtomic(atomic_color_r);
	ctrl_elem_color->AddAtomic(atomic_color_g);
	ctrl_elem_color->AddAtomic(atomic_color_b);
	ctrl_elem_color->published = true;

	__descriptor->Add_Descriptor_Element(ctrl_elem_color);
	__descriptor->Add_Descriptor_Element(ctrl_elem_autobrightness);
	__descriptor->Add_Descriptor_Element(ctrl_elem_brightess);
	__descriptor->Add_Descriptor_Element(ctrl_elem_buttons);

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
		if (pFMessage->Id == Get_Luxmeter_DevDrv(0)->DriverId) {
			__brightness = round(pFMessage->Value);
			if (__autoBrightness) {
				SetBrightness(round(pFMessage->Value));
			}
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
			//if (pButton->Id == ((Button_Device_Driver *)Selected_Button_Device)->GetButtonPinID()) {
			//}
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
	case PICTURE_MODULE_DRIVER_PATTERN_SWITCH_EXTERN:
	{
		uint8_t direction = message.GetIntParamByIndex(1);
		SwitchPattern(direction);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS_EXTERN:
	{
		bool state = message.GetIntParamByIndex(1);
		Set_Auto_Brightness(state);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_BRIGHTNESS_EXTERN:
	{
		uint8_t brightness = message.GetIntParamByIndex(1);
		SetBrightness(brightness);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_COLOR_EXTERN:
	{
		uint32_t rgb = (uint32_t)strtol(message.GetStringParamByIndex(1).c_str(), NULL, 16);
		Set_Pattern_Color((uint8_t)(rgb >> 16), (uint8_t)(rgb >> 8), (uint8_t)(rgb >> 0));
	}
	break;
	}
}


void Picture_Module_Driver::SwitchPattern(uint8_t _brightness) {
	switch (_brightness)
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
}

void Picture_Module_Driver::SetBrightness(uint8_t _brightness) {
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Set_Brightness(_brightness);

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Brightness(_brightness);
}

void Picture_Module_Driver::Set_Auto_Brightness(bool _state) {
	__autoBrightness = _state;
}

void Picture_Module_Driver::Set_Pattern_Color(int _r, int _g, int _b) {
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Color(_r, _g, _b);

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Color(_r, _g, _b);
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
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr) 
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Next();


	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr) 
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Next();
}

void Picture_Module_Driver::Pattern_Prev()
{
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Prev();

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Prev();
}

void Picture_Module_Driver::Pattern_Off()
{
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Off();

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Off();
}

