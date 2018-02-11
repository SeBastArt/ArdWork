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
#include "Ntp_Wifi_Device_Driver.h"

//#define DEBUG

Picture_Module_Driver::Picture_Module_Driver(uint8_t priority) :
	Module_Driver(priority)
{
#ifdef DEBUG
	Serial.print("Start Picture_Module_Driver with ID: ");
	Serial.println(this->DriverId);
#endif // DEBUG
	__DriverType = PICTURE_MODULE_DRIVER_TYPE;
	__envBrightness = 50;
	__AnimationCount = 6;
	__activeAnimaton = 5;
	__sv_relBrightness = 50;
	SetTimerDelay(1000);
}

void Picture_Module_Driver::Build_Discriptor() {
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::Build_Module_Discriptor");
#endif // DEBUG	
	__descriptor->name = F("Light Control");
	__descriptor->descr = F("control the behavior of the ambilight");
	__descriptor->published = true;

	Select_CtrlElem *ctrlElem_pattern = new Select_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_SWITCH, &__activeAnimaton, F("switch pattern"), F("Switch the build in animations"));
	ctrlElem_pattern->AddMember("Off");
	ctrlElem_pattern->AddMember("Fire");
	ctrlElem_pattern->AddMember("Cyclon");
	ctrlElem_pattern->AddMember("Shine");
	ctrlElem_pattern->AddMember("Random");
	ctrlElem_pattern->AddMember("Clock");
	ctrlElem_pattern->AddMember("Nixie");

	Color_CtrlElem *ctrlElem_color = new Color_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_COLOR, &__sv_color, F("Color"), F("The main color for the ambient light pattern"));

	Select_CtrlElem *ctrlElem_autobrightness = new Select_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS, &__sv_autoBrightness, F("Auto Brightness"), F("set the brightness automatically"));
	ctrlElem_autobrightness->AddMember("On");
	ctrlElem_autobrightness->AddMember("Off");

	Value_CtrlElem *ctrlElem_brightess = new Value_CtrlElem(PICTURE_MODULE_DRIVER_PATTERN_REL_BRIGHTNESS, &__sv_relBrightness, true, F("Brightness"), F("the brightness for the ambient light from 1% to 200%"));
	ctrlElem_brightess->unit = "%";

	__descriptor->Add_Descriptor_Element(ctrlElem_pattern);
	__descriptor->Add_Descriptor_Element(ctrlElem_color);
	__descriptor->Add_Descriptor_Element(ctrlElem_autobrightness);
	__descriptor->Add_Descriptor_Element(ctrlElem_brightess);
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::Build_Module_Discriptor");
#endif // DEBUG
}


void Picture_Module_Driver::OnInit()
{
	Module_Driver::OnInit();
}

void Picture_Module_Driver::DoTaskMessage(TaskMessage * message)
{
	switch ((message)->Class) {
	case MessageClass_Float:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoTaskMessage - MessageClass_Float");
#endif // DEBUG
		FloatMessage* pFMessage = (FloatMessage*)(message);
		if (pFMessage->__id == Get_Luxmeter_DevDrv(0)->DriverId) {
			__envBrightness = pFMessage->__absValue;
			SetStripBrightness();
		}
		break;
	}

	case MessageClass_Button:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoTaskMessage - MessageClass_Button");
#endif // DEBUG
		ButtonMessage* pButton = (ButtonMessage*)(message);
		if (pButton->State == TASK_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				Pattern_Next();
			}
		}
		else if (pButton->State == TASK_MSG_BUTTONSTATE_RELEASED)
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				//
			}
		}
		else if (pButton->State == TASK_MSG_BUTTONSTATE_AUTOREPEAT)
		{
			if (pButton->Id == Get_Button_DevDrv(0)->GetButtonPinID()) {
				Pattern_Off();
			}
		}
		break;
	}
	}
}

void Picture_Module_Driver::DoModuleMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case PICTURE_MODULE_DRIVER_PATTERN_STANDARD_NEXT:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_STANDARD_NEXT");
#endif // DEBUG
		Pattern_Next();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_STANDARD_PREV:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_STANDARD_PREV");
#endif // DEBUG
		Pattern_Prev();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_OFF:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_OFF");
#endif // DEBUG
		Pattern_Off();
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_SWITCH:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_SWITCH");
#endif // DEBUG
		uint8_t number = message.GetIntParamByIndex(0);
		SwitchPattern(number);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_AUTO_BRIGHTNESS");
#endif // DEBUG
		bool state = message.GetIntParamByIndex(0);
		Set_Auto_Brightness(state);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_REL_BRIGHTNESS:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_REL_BRIGHTNESS");
#endif // DEBUG
		int brightness = message.GetIntParamByIndex(0);
		SetRelBrightness(brightness);
	}
	break;
	case PICTURE_MODULE_DRIVER_PATTERN_COLOR:
	{
#ifdef DEBUG
		Serial.println("Start Picture_Module_Driver::DoModuleMessage - PICTURE_MODULE_DRIVER_PATTERN_COLOR");
#endif // DEBUG
		uint32_t rgb = (uint32_t)strtol(message.GetStringParamByIndex(0).c_str(), NULL, 16);
		__sv_color.R = (uint8_t)((rgb >> 16) & 0xFF);
		__sv_color.G = (uint8_t)((rgb >> 8) & 0xFF);
		__sv_color.B = (uint8_t)((rgb >> 0) & 0xFF);
		Set_Pattern_Color(__sv_color.R, __sv_color.G, __sv_color.B);
	}
	break;
	}
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::DoModuleMessage");
#endif // DEBUG
}

void Picture_Module_Driver::TimerTick() {
	__activeAnimaton = 6;
	switch (__activeAnimaton)
	{
	case 5:
	{
		AnimateClock();
	}
	break;
	case 6:
	{
		DigiClock();
	}
	break;
	}
}

void Picture_Module_Driver::DigiClock() {
	time_t tTlocal = Get_Ntp_Wifi_Device_DevDrv(0)->local_time;
	int Led_Count = Get_Uart_GRBW_Led_DevDrv(0)->led_count;

	int lookUp[10][2] = {
		{ 10, 20 } ,	//0
		{ 1, 11 } ,		//1
		{ 9, 19 } ,		//2
		{ 2, 12 } ,		//3
		{ 8, 18 } ,		//4
		{ 3, 13 } ,		//5
		{ 7, 17 } ,		//6
		{ 4, 14 } ,		//7
		{ 6, 16 } ,		//8
		{ 5, 15 }		//9
	};

	int timeArray[6] = {	0, //hour first digit
							0, //hour second digit
							0, //minutes first digit
							0, //minutes second digit
							0, //seconds first digit
							0  //seconds second digit
	};

	timeArray[0] = (hour(tTlocal) % 12) / 10;
	timeArray[1] = (hour(tTlocal) % 12) % 10;
	timeArray[2] = minute(tTlocal) / 10;
	timeArray[3] = minute(tTlocal) % 10;
	timeArray[4] = second(tTlocal) / 10;
	timeArray[5] = second(tTlocal) % 10;



	Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Color_All(0, 0, 0);

	int tubeindex = 0;

	String sResponse = "";
	sResponse += hour(tTlocal); sResponse += (":");
	sResponse += minute(tTlocal) / 10; sResponse += minute(tTlocal) % 10; sResponse += (":");
	sResponse += second(tTlocal) / 10; sResponse += second(tTlocal) % 10; sResponse += (" - ");;
	Serial.print(sResponse);

	Serial.print(timeArray[0]);
	Serial.print(timeArray[1]);
	Serial.print(":");
	Serial.print(timeArray[2]);
	Serial.print(timeArray[3]);
	Serial.print(":");
	Serial.print(timeArray[4]);
	Serial.print(timeArray[5]);
	Serial.print(" - ");

	for (int i = 5; i >= 0; --i)
	{
		Serial.println(i);
		Serial.print(lookUp[timeArray[i]][0] + tubeindex);
		Serial.print(" ");
		Serial.print(lookUp[timeArray[i]][1] + tubeindex);
		Serial.print(" ");
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(lookUp[timeArray[i]][0] + tubeindex, __sv_color.R, __sv_color.G, __sv_color.B);
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(lookUp[timeArray[i]][1] + tubeindex, __sv_color.R, __sv_color.G, __sv_color.B);
		tubeindex = tubeindex + 20;
		Serial.print(" ");
	}
	Serial.println();
}

void Picture_Module_Driver::AnimateClock() {
	time_t tTlocal = Get_Ntp_Wifi_Device_DevDrv(0)->local_time;
	int Led_Count = Get_Uart_GRBW_Led_DevDrv(0)->led_count;

	Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Color_All(__sv_color.R, __sv_color.G, __sv_color.B);
	int hour_index = round(Led_Count * (hour(tTlocal) % 12) / 12.0 + 17) % Led_Count;
	int min_index = round(Led_Count * minute(tTlocal) / 60.0 + 17) % Led_Count;
	double sec_index = Led_Count * second(tTlocal) / 60.0 + 17;
	if (sec_index > Led_Count)
		sec_index = sec_index - Led_Count;

	double schweif = 1;
	double safe_sec_index = sec_index + Led_Count;
	int start = trunc(safe_sec_index - schweif) - 1;
	int ende = trunc(safe_sec_index + schweif) + 2;
	for (int i = start; i < ende; i++)
	{
		double multi = schweif - fabs(safe_sec_index - i);
		int led = i % Led_Count;
		int licht = round(multi * 255 / schweif);
		licht = max(0, licht);
		licht = min(255, licht);
		if (licht > 1)
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(led, 5, licht, 5);
	}
	Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(min_index, 240, 5, 5);
	Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(hour_index, 5, 5, 240);

	if (sec_index == min_index)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(min_index, __sv_color.G, __sv_color.G, __sv_color.G);

	if (sec_index == min_index == hour_index)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(hour_index, 240, 240, 240);

	if (min_index == hour_index)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(hour_index, __sv_color.B, __sv_color.B, __sv_color.B);

}

void Picture_Module_Driver::SwitchPattern(uint8_t _number) {
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::SwitchPattern");
#endif // DEBUG
	__activeAnimaton = _number;
	__activeAnimaton = __activeAnimaton % __AnimationCount;
	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr) {
		switch (_number)
		{
		case 0:
		{
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Off();
		}
		break;
		case 1:
		{
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Fire();
		}
		break;
		case 2:
		{
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Cyclon();
		}
		break;
		case 3:
		{
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Shine();
		}
		break;
		case 4:
		{
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Random();
		}
		break;
		case 5:
		{
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Off();
			//Clock will be animated
		}
		break;
		}
	}
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::SwitchPattern");
#endif // DEBUG
}

void Picture_Module_Driver::SetStripBrightness() {
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::SetStripBrightness");
#endif // DEBUG
	uint8_t brightness;
	if (__sv_autoBrightness > 0) {
		float faktor = 200.0 / __sv_relBrightness;
		brightness = round(MIN(60.0, MAX(5.0, (__envBrightness * 100.0) / (200.0 * faktor))));
	}
	else {
		brightness = round(__sv_relBrightness / 2);
	}

	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Set_Brightness(brightness);

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Brightness(brightness);
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::SetStripBrightness");
#endif // DEBUG
}

void Picture_Module_Driver::SetRelBrightness(uint8_t _relBrightness)
{
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::SetRelBrightness");
#endif // DEBUG
	__sv_relBrightness = MAX(1, MIN(200, _relBrightness));
	SetStripBrightness();
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::SetRelBrightness");
#endif // DEBUG
}

void Picture_Module_Driver::Set_Auto_Brightness(bool _state) {
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::Set_Auto_Brightness");
#endif // DEBUG	
	__sv_autoBrightness = (int)_state;
	SetStripBrightness();
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::Set_Auto_Brightness");
#endif // DEBUG	
}

void Picture_Module_Driver::Set_Pattern_Color(int _r, int _g, int _b) {
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::Set_Pattern_Color");
#endif // DEBUG	
	if (Get_Uart_RGB_Led_DevDrv(0) != nullptr)
		Get_Uart_RGB_Led_DevDrv(0)->Exec_Animation_Color(_r, _g, _b);

	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Color(_r, _g, _b);
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::Set_Pattern_Color");
#endif // DEBUG	
}

void Picture_Module_Driver::Pattern_Next()
{
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::Pattern_Standard_Next");
#endif // DEBUG	
	SwitchPattern(__activeAnimaton++);
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::Pattern_Standard_Next");
#endif // DEBUG	
}


void Picture_Module_Driver::Pattern_Prev()
{
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::Pattern_Standard_Prev");
#endif // DEBUG	
	SwitchPattern(__activeAnimaton--);
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::Pattern_Standard_Prev");
#endif // DEBUG	
}

void Picture_Module_Driver::Pattern_Off()
{
#ifdef DEBUG
	Serial.println("Start Picture_Module_Driver::Pattern_Off");
#endif // DEBUG	
	SwitchPattern(0);
#ifdef DEBUG
	Serial.println("Ende Picture_Module_Driver::Pattern_Off");
#endif // DEBUG	
}

