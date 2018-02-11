// 
// 
// 

#include "Nixie_Module_Driver.h"
#include "Nixie_Module_Driver_Consts.h"
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"
#include "Ntp_Wifi_Device_Driver.h"

//#define DEBUG

//#define four_digit
#define six_digit

Nixie_Module_Driver::Nixie_Module_Driver(uint8_t priority) :
	Module_Driver(priority)
{
#ifdef DEBUG
	Serial.print("Start Nixie_Module_Driver with ID: ");
	Serial.println(this->DriverId);
#endif // DEBUG
	__DriverType = NIXIE_MODULE_DRIVER_TYPE;
	__AnimationCount = 6;
	__activeAnimaton = 5;
	SetTimerDelay(1000);
}

void Nixie_Module_Driver::Build_Discriptor() {
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::Build_Module_Discriptor");
#endif // DEBUG	
	__descriptor->name = F("Light Control");
	__descriptor->descr = F("control the behavior of the ambilight");
	__descriptor->published = true;

	Select_CtrlElem *ctrlElem_pattern = new Select_CtrlElem(NIXIE_MODULE_DRIVER_PATTERN_SWITCH, &__activeAnimaton, F("switch pattern"), F("Switch the build in animations"));
	ctrlElem_pattern->AddMember("Off");
	ctrlElem_pattern->AddMember("Fire");
	ctrlElem_pattern->AddMember("Cyclon");
	ctrlElem_pattern->AddMember("Shine");
	ctrlElem_pattern->AddMember("Random");
	ctrlElem_pattern->AddMember("Nixie");

	Color_CtrlElem *ctrlElem_color = new Color_CtrlElem(NIXIE_MODULE_DRIVER_PATTERN_COLOR, &__sv_color, F("Color"), F("The main color for the ambient light pattern"));

	__descriptor->Add_Descriptor_Element(ctrlElem_pattern);
	__descriptor->Add_Descriptor_Element(ctrlElem_color);
#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::Build_Module_Discriptor");
#endif // DEBUG
}

void Nixie_Module_Driver::DoTaskMessage(TaskMessage * message)
{
	switch ((message)->Class) {
	case MessageClass_Button:
	{
#ifdef DEBUG
		Serial.println("Start Nixie_Module_Driver::DoTaskMessage - MessageClass_Button");
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

void Nixie_Module_Driver::DoModuleMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case NIXIE_MODULE_DRIVER_PATTERN_SWITCH:
	{
#ifdef DEBUG
		Serial.println("Start Nixie_Module_Driver::DoModuleMessage - Nixie_Module_DRIVER_PATTERN_SWITCH");
#endif // DEBUG
		uint8_t number = message.GetIntParamByIndex(0);
		SwitchPattern(number);
	}
	break;
	case NIXIE_MODULE_DRIVER_PATTERN_COLOR:
	{
#ifdef DEBUG
		Serial.println("Start Nixie_Module_Driver::DoModuleMessage - Nixie_Module_DRIVER_PATTERN_COLOR");
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
	Serial.println("Ende Nixie_Module_Driver::DoModuleMessage");
#endif // DEBUG
}

void Nixie_Module_Driver::TimerTick() {
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::TimerTick");
#endif // DEBUG	
	switch (__activeAnimaton)
	{
	case 5:
	{
#ifdef DEBUG
		Serial.println("Nixie_Module_Driver::TimerTick - DigiClock");
#endif // DEBUG	
		DigiClock();
	}
	break;
	}
#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::TimerTick");
#endif // DEBUG	
}

void Nixie_Module_Driver::DigiClock() {
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::DigiClock");
#endif // DEBUG	
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

#ifdef six_digit

	int timeArray[6] = { 0, //hour first digit
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
#endif

#ifdef four_digit

	int timeArray[4] = { 0, //hour first digit
		0, //hour second digit
		0, //minutes first digit
		0, //minutes second digit
	};

	timeArray[0] = (hour(tTlocal) % 12) / 10;
	timeArray[1] = (hour(tTlocal) % 12) % 10;
	timeArray[2] = minute(tTlocal) / 10;
	timeArray[3] = minute(tTlocal) % 10;
#endif

	Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Color_All(0, 0, 0);

	int tubeindex = 0;

#ifdef DEBUG
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
	Serial.println(timeArray[5]);
	Serial.print("timeArray Length:");
	Serial.println(String(sizeof(timeArray) / sizeof(timeArray[0])));
#endif // DEBUG	

	for (int i = sizeof(timeArray) / sizeof(timeArray[0]) - 1; i >= 0; --i)
	{
#ifdef DEBUG
		Serial.print(i + String(" "));
		Serial.print(lookUp[timeArray[i]][0] + tubeindex);
		Serial.print(" ");
		Serial.print(lookUp[timeArray[i]][1] + tubeindex);
		Serial.println(" ");
#endif // DEBUG	
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(lookUp[timeArray[i]][0] + tubeindex, __sv_color.R, __sv_color.G, __sv_color.B);
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Set_Pixel(lookUp[timeArray[i]][1] + tubeindex, __sv_color.R, __sv_color.G, __sv_color.B);
		tubeindex = tubeindex + 20;
	}
#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::DigiClock");
#endif // DEBUG	
}


void Nixie_Module_Driver::SwitchPattern(uint8_t _number) {
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::SwitchPattern");
#endif // DEBUG
	__activeAnimaton = _number;
	__activeAnimaton = __activeAnimaton % __AnimationCount;
	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr) {
		switch (__activeAnimaton)
		{
		case 0:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Off");
#endif // DEBUG
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Off();
		}
		break;
		case 1:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Fire");
#endif // DEBUG
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Fire();
		}
		break;
		case 2:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Cyclon");
#endif // DEBUG
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Cyclon();
		}
		break;
		case 3:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Shine");
#endif // DEBUG
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Shine();
		}
		break;
		case 4:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Random");
#endif // DEBUG
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Random();
		}
		break;
		case 5:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Nixie");
#endif // DEBUG
			Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Off();
			//Nixie will be animated
		}
		break;
		}
	}
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::SwitchPattern");
#endif // DEBUG
}

void Nixie_Module_Driver::Set_Pattern_Color(int _r, int _g, int _b) {
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::Set_Pattern_Color");
#endif // DEBUG	
	if (Get_Uart_GRBW_Led_DevDrv(0) != nullptr)
		Get_Uart_GRBW_Led_DevDrv(0)->Exec_Animation_Color(_r, _g, _b);
#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::Set_Pattern_Color");
#endif // DEBUG	
}

void Nixie_Module_Driver::Pattern_Next()
{
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::Pattern_Next");
#endif // DEBUG	
	SwitchPattern(++__activeAnimaton);
#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::Pattern_Next");
#endif // DEBUG	
}


void Nixie_Module_Driver::Pattern_Prev()
{
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::Pattern_Prev");
#endif // DEBUG	
	SwitchPattern(--__activeAnimaton);
#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::Pattern_Prev");
#endif // DEBUG	
}

void Nixie_Module_Driver::Pattern_Off()
{
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::Pattern_Off");
#endif // DEBUG	
	SwitchPattern(0);
#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::Pattern_Off");
#endif // DEBUG	
}

