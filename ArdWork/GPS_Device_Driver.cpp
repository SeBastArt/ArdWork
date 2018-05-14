#include "GPS_Device_Driver.h"
#include "Timezones.h"

REGISTERIMPL(GPS_Device_Driver);

//#define DEBUG

GPS_Device_Driver::GPS_Device_Driver(Module_Driver * module, uint8_t priority) :
	Device_Driver(module, priority) {
	__DriverType = GPS_DEVICE_DRIVER_TYPE;
	__local_time = 0;
	__utc_time = 0;
	__local_timer = 0;
	__active = false;
	SetTimerDelay(500);
	__time_is_set = false;
	__active = false;
	__rx = nullptr;
	__tx = nullptr;
}

void GPS_Device_Driver::SetPins(IO_Pin * _rx, IO_Pin * _tx)
{
	__rx = _rx;
	__tx = _tx;
}


void GPS_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println(F("Start GPS_Device_Driver::Build_Descriptor"));
#endif // DEBUG
	__descriptor->name = (F("GPS-Driver"));
	__descriptor->descr = (F("GPS-Sattelites for real time"));
	__descriptor->published = true;

	Time_CtrlElem *ctrlElem_utc_time = new Time_CtrlElem(GPS_DEVICE_DRIVER_UTC_TIME, &__utc_time, false, F("UTC Time"), F("world time"));

	Time_CtrlElem *ctrlElem_local_time = new Time_CtrlElem(GPS_DEVICE_DRIVER_LOCAL_TIME, &__local_time, false, F("Local Time"), F("The Time in your Location"));

	Select_CtrlElem *ctrlElem_timeZone = new Select_CtrlElem(GPS_DEVICE_DRIVER_TIMEZONE, &__sv_timezone, F("Timezone"), F("Select the Timezone you want to display"));
	for (uint8_t i = 0; i < sizeof(timezone_Arr) / sizeof(timezone_struct); ++i)
		ctrlElem_timeZone->AddMember(timezone_Arr[i]._name);
	
	Group_CtrlElem *ctrlElem_SetTime = new Group_CtrlElem(GPS_DEVICE_DRIVER_GET_TIME, F("Get new Timestamp"), F("Get a new timestamp from GPS-Module"));
	ctrlElem_SetTime->AddMember(F("Update Time"));

	__descriptor->Add_Descriptor_Element(ctrlElem_timeZone);
	__descriptor->Add_Descriptor_Element(ctrlElem_SetTime);
	__descriptor->Add_Descriptor_Element(ctrlElem_utc_time);
	__descriptor->Add_Descriptor_Element(ctrlElem_local_time);
#ifdef DEBUG
	Serial.println(F("Ende GPS_Device_Driver::Build_Descriptor"));
#endif // DEBUG
}

void GPS_Device_Driver::OnInit()
{
#ifdef DEBUG
	Serial.println(F("Start GPS_Device_Driver::OnInit"));
#endif // DEBUG
	Device_Driver::OnInit();
	//StartGetTime();
#ifdef DEBUG
	Serial.println(F("Ende GPS_Device_Driver::OnInit"));
#endif // DEBUG
}

time_t GPS_Device_Driver::GetLocalTime() const {
	time_t utc_time;
	time_t local_time;
	utc_time = now();
	local_time = ((Timezone)timezone_Arr[__sv_timezone]._timezone).toLocal(utc_time);
	return local_time;
}

time_t GPS_Device_Driver::GetUtcTime() const {
	time_t utc_time;
	utc_time = now();
	return utc_time;
}

void GPS_Device_Driver::DoUpdate(uint32_t deltaTime) {
	if (!__isIdle)
		return;

	if (__time_is_set && __active) {
		EndGPS();
	}

	if (__local_timer <= 0) {
		__time_is_set = true;
		return;
	}
	if (!__active)
		return;
	if (!__time_is_set) {
		__local_timer -= deltaTime;
		while (SerialGPS->available() > 0) {
			if (gps->encode(SerialGPS->read())) { // process gps messages
				setTime(gps->time.hour(), gps->time.minute(), gps->time.second(), gps->date.day(), gps->date.month(), gps->date.year());
				__time_is_set = true;
			}
		}
	}
}


void GPS_Device_Driver::StartGetTime() {
#ifdef DEBUG
	Serial.println(F("Start GPS_Device_Driver::StartGetTime"));
#endif // DEBUG
	if ((__rx == nullptr) || (__tx == nullptr))
		return;

	EndGPS();
	SerialGPS = new SoftwareSerial(__rx->pinGPIO, __tx->pinGPIO);
	gps = new TinyGPSPlus();
	SerialGPS->begin(9600);
	__active = true;
	__time_is_set = false;
	__local_timer = 10000;  // try it for 10 seconds
#ifdef DEBUG
	Serial.println(F("Ende GPS_Device_Driver::StartGetTime"));
#endif // DEBUG
}

void GPS_Device_Driver::EndGPS() {
#ifdef DEBUG
	Serial.println(F("Start GPS_Device_Driver::EndGPS"));
#endif // DEBUG
	if (SerialGPS != nullptr) {
		SerialGPS->end();
		delete SerialGPS;
		SerialGPS = nullptr;
	}
	if (gps != nullptr) {
		delete gps;
		gps = nullptr;
	}
	__active = false;
#ifdef DEBUG
	Serial.println(F("Ende GPS_Device_Driver::EndGPS"));
#endif // DEBUG
}

void GPS_Device_Driver::TimerTick() {
	__utc_time = now();
	__local_time = ((Timezone)timezone_Arr[__sv_timezone]._timezone).toLocal(__utc_time);
}


void GPS_Device_Driver::DoDeviceMessage(Int_Task_Msg message) {
	int messageID = message.id;
	switch (messageID)
	{
	case GPS_DEVICE_DRIVER_UTC_TIME:
	{
#ifdef DEBUG
		Serial.println(F("Start GPS_Device_Driver::DoDeviceMessage - GPS_DEVICE_DRIVER_UTC_TIME"));
#endif // DEBUG
		//
	}
	break;
	case GPS_DEVICE_DRIVER_LOCAL_TIME:
	{
#ifdef DEBUG
		Serial.println(F("Start GPS_Device_Driver::DoDeviceMessage - GPS_DEVICE_DRIVER_LOCAL_TIME"));
#endif // DEBUG
		//
	}
	break;
	case GPS_DEVICE_DRIVER_GET_TIME:
	{
#ifdef DEBUG
		Serial.println(F("Start GPS_Device_Driver::DoDeviceMessage - GPS_DEVICE_DRIVER_SET_TIME"));
#endif // DEBUG
		StartGetTime();
	}
	break;
	case GPS_DEVICE_DRIVER_TIMEZONE:
	{
#ifdef DEBUG
		Serial.println(F("Start GPS_Device_Driver::DoDeviceMessage - GPS_DEVICE_DRIVER_TIMEZONE"));
#endif // DEBUG
		int timezone = message.GetIntParamByIndex(0);
		SetTimezone(timezone);
	}
	break;
	}
}



void GPS_Device_Driver::SetTimezone(int _timezone) {
#ifdef DEBUG
	Serial.println(F("Start GPS_Device_Driver::SetTimezone"));
#endif // DEBUG
	__sv_timezone = _timezone;
#ifdef DEBUG
	Serial.println(F("Ende GPS_Device_Driver::SetTimezone"));
#endif // DEBUG
}

void GPS_Device_Driver::Exec_Start_Get_Time() {
#ifdef DEBUG
	Serial.println(F("Start GPS_Device_Driver::Exec_Start_Get_Time"));
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(GPS_DEVICE_DRIVER_GET_TIME);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println(F("Ende GPS_Device_Driver::Exec_Start_Get_Time"));
#endif // DEBUG
}

void GPS_Device_Driver::Exec_Set_Timezone(int _timezone) {
#ifdef DEBUG
	Serial.println(F("Start GPS_Device_Driver::Exec_Set_Timezone"));
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(GPS_DEVICE_DRIVER_TIMEZONE);
	message->AddParam(_timezone);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println(F("Ende GPS_Device_Driver::Exec_Set_Timezone"));
#endif // DEBUG
}