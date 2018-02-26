// 
// 
// 

#include "Nixie_Module_Driver.h"
#include "Nixie_Module_Driver_Consts.h"
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Wifi_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"
#include "Ntp_Wifi_Device_Driver.h"
#include "GPS_Device_Driver.h"

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
	SetTimerDelay(500);
	__date_time = false;
	__sv_date_time = 0;
	__sv_time_format = 0;
	__sv_time_source = 0;
	__timer_count = millis();
#ifdef DEBUG
	Serial.println("create(\"Button_Device_Driver\")");
#endif
	__button = (Button_Device_Driver*)(create(String(F("Button_Device_Driver")).c_str()));
	IO_Pin* _button_pin = pinManager.GetPin("D7");
	__button->SetPin(_button_pin);
	__button->PullUp();

#ifdef DEBUG
	Serial.println("create(\"Wifi_Device_Driver\")");
#endif
	Wifi_Device_Driver* wifi_device = (Wifi_Device_Driver*)create(String(F("Wifi_Device_Driver")).c_str());

#ifdef DEBUG
	Serial.println("create(\"WebSocket_Wifi_Device_Driver\")");
#endif
	WebSocket_Wifi_Device_Driver * temp = (WebSocket_Wifi_Device_Driver*)create(String(F("WebSocket_Wifi_Device_Driver")).c_str());
	wifi_device->AddCommunicationClient(temp);

#ifdef DEBUG
	Serial.println("create(\"Ntp_Wifi_Device_Driver\")");
#endif
	Ntp_Wifi_Device_Driver* __ntp = (Ntp_Wifi_Device_Driver*)create(String(F("Ntp_Wifi_Device_Driver")).c_str());
	wifi_device->AddCommunicationClient(__ntp);

#ifdef DEBUG
	Serial.println("create(\"GPS_Device_Driver\")");
#endif
	__gps = (GPS_Device_Driver*)create(String(F("GPS_Device_Driver")).c_str());
	__gps->SetPins(pinManager.GetPin("D5"), pinManager.GetPin("D4"));
#ifdef DEBUG
	Serial.print("create(\"Uart_GRBW_Led_Device_Driver\")");
#endif
	__strip = (Uart_GRBW_Led_Device_Driver*)create(String(F("Uart_GRBW_Led_Device_Driver")).c_str());
	__strip->SetPixelCount(28);
}


void Nixie_Module_Driver::Build_Discriptor() {
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::Build_Module_Discriptor");
#endif // DEBUG	
	__descriptor->name = F("Nixie Clock");
	__descriptor->descr = F("a stylish digital clock");
	__descriptor->published = true;

	Select_CtrlElem *ctrlElem_pattern = new Select_CtrlElem(NIXIE_MODULE_DRIVER_PATTERN_SWITCH, &__activeAnimaton, F("switch pattern"), F("Switch the build in animations"));
	ctrlElem_pattern->AddMember(F("Off"));
	ctrlElem_pattern->AddMember(F("Fire"));
	ctrlElem_pattern->AddMember(F("Cyclon"));
	ctrlElem_pattern->AddMember(F("Shine"));
	ctrlElem_pattern->AddMember(F("Random"));
	ctrlElem_pattern->AddMember(F("Nixie"));

	Color_CtrlElem *ctrlElem_color = new Color_CtrlElem(NIXIE_MODULE_DRIVER_PATTERN_COLOR, &__sv_color, F("Color"), F("The main color for the ambient light pattern"));

	Select_CtrlElem *ctrlElem_date_time = new Select_CtrlElem(NIXIE_MODULE_DRIVER_PATTERN_DATE_TIME, &__sv_date_time, F("Date or Time"), F("Dsplay the Date, Time or change between both frequently"));
	ctrlElem_date_time->AddMember(F("Time"));
	ctrlElem_date_time->AddMember(F("Date"));
	ctrlElem_date_time->AddMember(F("Time/Date"));

	Select_CtrlElem *ctrlElem_time_format = new Select_CtrlElem(NIXIE_MODULE_DRIVER_PATTERN_TIME_FORMAT, &__sv_time_format, F("Time Format"), F("Select time format 12h or 24h"));
	ctrlElem_time_format->AddMember(F("12 hours"));
	ctrlElem_time_format->AddMember(F("24 hours"));

	Select_CtrlElem *ctrlElem_time_source = new Select_CtrlElem(NIXIE_MODULE_DRIVER_PATTERN_TIME_SOURCE, &__sv_time_source, F("Time Source"), F("Select the source which provides system time"));
	ctrlElem_time_source->AddMember(F("NTP-Server"));
	ctrlElem_time_source->AddMember(F("GPS-Time"));

	__descriptor->Add_Descriptor_Element(ctrlElem_pattern);
	__descriptor->Add_Descriptor_Element(ctrlElem_color);
	__descriptor->Add_Descriptor_Element(ctrlElem_date_time);
	__descriptor->Add_Descriptor_Element(ctrlElem_time_format);
	__descriptor->Add_Descriptor_Element(ctrlElem_time_source);
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
		if (pButton->State == BUTTON_DEVICE_BUTTONSTATE_PRESSED) // any state that is pressed
		{
			if (pButton->Id == __button->GetButtonPinID()) {
				Pattern_Next();
			}
		}
		else if (pButton->State == BUTTON_DEVICE_BUTTONSTATE_RELEASED)
		{
			if (pButton->Id == __button->GetButtonPinID()) {
				//
			}
		}
		else if (pButton->State == BUTTON_DEVICE_BUTTONSTATE_AUTOREPEAT)
		{
			if (pButton->Id == __button->GetButtonPinID()) {
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
		Serial.println("Start Nixie_Module_Driver::DoModuleMessage - NIXIE_MODULE_DRIVER_PATTERN_SWITCH");
#endif // DEBUG
		uint8_t number = message.GetIntParamByIndex(0);
		SwitchPattern(number);
	}
	break;
	case NIXIE_MODULE_DRIVER_PATTERN_COLOR:
	{
#ifdef DEBUG
		Serial.println("Start Nixie_Module_Driver::DoModuleMessage - NIXIE_MODULE_DRIVER_PATTERN_COLOR");
#endif // DEBUG
		uint32_t rgb = (uint32_t)strtol(message.GetStringParamByIndex(0).c_str(), NULL, 16);
		__sv_color.R = (uint8_t)((rgb >> 16) & 0xFF);
		__sv_color.G = (uint8_t)((rgb >> 8) & 0xFF);
		__sv_color.B = (uint8_t)((rgb >> 0) & 0xFF);
		Set_Pattern_Color(__sv_color.R, __sv_color.G, __sv_color.B);
	}
	break;
	case NIXIE_MODULE_DRIVER_PATTERN_TIME_SOURCE:
	{
#ifdef DEBUG
		Serial.println("Start Nixie_Module_Driver::DoModuleMessage - NIXIE_MODULE_DRIVER_PATTERN_TIME_SOURCE");
#endif // DEBUG
		int number = message.GetIntParamByIndex(0);
		SetTimeSource(number);
	}
	break;
	case NIXIE_MODULE_DRIVER_PATTERN_TIME_FORMAT:
	{
#ifdef DEBUG
		Serial.println("Start Nixie_Module_Driver::DoModuleMessage - NIXIE_MODULE_DRIVER_PATTERN_TIME_FORMAT");
#endif // DEBUG
		int number = message.GetIntParamByIndex(0);
		SetTimeFormat(number);
	}
	break;
	case NIXIE_MODULE_DRIVER_PATTERN_DATE_TIME:
	{
#ifdef DEBUG
		Serial.println("Start Nixie_Module_Driver::DoModuleMessage - NIXIE_MODULE_DRIVER_PATTERN_DATE_TIME");
#endif // DEBUG
		int number = message.GetIntParamByIndex(0);
		SetDateTime(number);
	}
	break;
	}


#ifdef DEBUG
	Serial.println("Ende Nixie_Module_Driver::DoModuleMessage");
#endif // DEBUG
}


void Nixie_Module_Driver::SetDateTime(int _number)
{
	__sv_date_time = _number;
}

void Nixie_Module_Driver::SetTimeFormat(int _number)
{
	__sv_time_format = _number;
}

void Nixie_Module_Driver::SetTimeSource(int _number)
{
	__sv_time_source = _number;
	SetTimeBySource(__sv_time_source);
}

void Nixie_Module_Driver::SetTimeBySource(int _timeSource)
{
	switch (__sv_time_source)
	{
	case 0:
		__gps->Exec_Start_Get_Time();
		break;
	case 1:
		__ntp->Exec_Start_Get_Time();
		break;
	}
}


void Nixie_Module_Driver::TimerTick() {
#ifdef DEBUG
	Serial.println("Start Nixie_Module_Driver::TimerTick");
#endif // DEBUG	

	switch (__sv_date_time)
	{
	case 0:
		__date_time = false;
		break;
	case 1:
		__date_time = true;;
		break;
	case 2:
		if (millis() - __timer_count > 30000) {
			__timer_count = millis();
			__date_time == true ? __date_time = false : __date_time = true;
		}
		break;
	}

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


	time_t tTlocal = 0;
	switch (__sv_time_source)
	{
	case 0:
		tTlocal = __gps->local_time;
		break;
	case 1:
		tTlocal = __ntp->local_time;
		break;
	}

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

	if (!__date_time) //display Time
	{

		timeArray[0] = second(tTlocal) % 10;
		timeArray[1] = second(tTlocal) / 10;

		timeArray[2] = minute(tTlocal) % 10;
		timeArray[3] = minute(tTlocal) / 10;

		if (__sv_time_format == 0) {
			timeArray[4] = (hour(tTlocal) % 12) % 10;
			timeArray[5] = (hour(tTlocal) % 12) / 10;
		}
		else
		{
			timeArray[4] = (hour(tTlocal) % 24) % 10;	//unnessessary only for reading code
			timeArray[5] = (hour(tTlocal) % 24) / 10;	//unnessessary only for reading code
		}
	}
	else //display Date
	{
		timeArray[0] = day(tTlocal) % 10;
		timeArray[1] = day(tTlocal) / 10;

		timeArray[2] = month(tTlocal) % 10;
		timeArray[3] = month(tTlocal) / 10;
	}
#endif

#ifdef four_digit

	int timeArray[4] = { 0, //hour first digit
		0, //hour second digit
		0, //minutes first digit
		0, //minutes second digit
	};
	if (!__date_time) //display Time
	{
		timeArray[0] = minute(tTlocal) % 10;
		timeArray[1] = minute(tTlocal) / 10;

		if (__sv_time_format == 0) {
			timeArray[2] = (hour(tTlocal) % 12) % 10;
			timeArray[3] = (hour(tTlocal) % 12) / 10;
		}
		else
		{
			timeArray[2] = (hour(tTlocal) % 24) % 10;	//unnessessary only for reading code
			timeArray[3] = (hour(tTlocal) % 24) / 10;	//unnessessary only for reading code
		}
	}
	else
	{
		timeArray[0] = day(tTlocal) % 10;
		timeArray[1] = day(tTlocal) / 10;

		timeArray[2] = month(tTlocal) % 10;
		timeArray[3] = month(tTlocal) / 10;
	}

#endif
	__strip->Exec_Set_Color_All(0, 0, 0);
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

	int digit_count = (sizeof(timeArray) / sizeof(timeArray[0]));
#ifdef six_digit
	if (__date_time)	//correct the digits to 4 for date and 6 for time
		digit_count = digit_count - 2;
#endif

	for (int i = 0; i < digit_count; i++)
		//for (int i = sizeof(timeArray) / sizeof(timeArray[0]) - 1; i >= 0; --i)
	{
#ifdef DEBUG
		Serial.print(i + String(" "));
		Serial.print(lookUp[timeArray[i]][0] + tubeindex);
		Serial.print(" ");
		Serial.print(lookUp[timeArray[i]][1] + tubeindex);
		Serial.println(" ");
#endif // DEBUG	
		__strip->Exec_Set_Pixel(lookUp[timeArray[i]][0] + tubeindex, __sv_color.R, __sv_color.G, __sv_color.B);
		__strip->Exec_Set_Pixel(lookUp[timeArray[i]][1] + tubeindex, __sv_color.R, __sv_color.G, __sv_color.B);
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
	if (__strip != nullptr) {
		switch (__activeAnimaton)
		{
		case 0:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Off");
#endif // DEBUG
			__strip->Exec_Animation_Off();
		}
		break;
		case 1:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Fire");
#endif // DEBUG
			__strip->Exec_Animation_Fire();
		}
		break;
		case 2:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Cyclon");
#endif // DEBUG
			__strip->Exec_Animation_Cyclon();
		}
		break;
		case 3:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Shine");
#endif // DEBUG
			__strip->Exec_Animation_Shine();
		}
		break;
		case 4:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Exec_Animation_Random");
#endif // DEBUG
			__strip->Exec_Animation_Random();
		}
		break;
		case 5:
		{
#ifdef DEBUG
			Serial.println("Nixie_Module_Driver::SwitchPattern - Nixie");
#endif // DEBUG
			__strip->Exec_Animation_Off();
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
	if (__strip != nullptr)
		__strip->Exec_Animation_Color(_r, _g, _b);
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

