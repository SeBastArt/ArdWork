#include "GPS_Device_Driver.h"


REGISTERIMPL(GPS_Device_Driver);

//#define DEBUG


TimeChangeRule CEST1 = { "CEST", Last, Sun, Mar, 2, 120 };     //Central European Summer Time
TimeChangeRule CET1 = { "CET ", Last, Sun, Oct, 3, 60 };       //Central European Standard Time
Timezone CE1(CEST1, CET1);


GPS_Device_Driver::GPS_Device_Driver(Module_Driver * module, uint8_t priority) :
	Device_Driver(module, priority) {
	__DriverType = GPS_DEVICE_DRIVER_TYPE;
	__local_time = 0;
	__utc_time = 0;
	__local_timer_delay = 2000;
	__local_timer_delta = 0;
	SetTimerDelay(500);
	__time_is_set = false;
	__active = false;
}


void GPS_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println("Start GPS_Device_Driver::Build_Descriptor");
#endif // DEBUG
	__descriptor->name = (F("GPS-Driver"));
	__descriptor->descr = (F("Sync with GPS-Sattelites for real time"));
	__descriptor->published = true;

	Time_CtrlElem *ctrlElem_utc_time = new Time_CtrlElem(GPS_DEVICE_DRIVER_UTC_TIME, &__utc_time, false, (F("UTC Time")), (F("world time")));

	Time_CtrlElem *ctrlElem_local_time = new Time_CtrlElem(GPS_DEVICE_DRIVER_LOCAL_TIME, &__local_time, false, (F("Local Time")), (F("The Time in your Location")));

	__descriptor->Add_Descriptor_Element(ctrlElem_utc_time);
	__descriptor->Add_Descriptor_Element(ctrlElem_local_time);
#ifdef DEBUG
	Serial.println("Ende GPS_Device_Driver::Build_Descriptor");
#endif // DEBUG
}

void GPS_Device_Driver::DoUpdate(uint32_t deltaTime)
{
	//timer_delta += deltaTime;
	//if (timer_delta > timer_delay) {
	//	timer_delta = 0;

	if (__time_is_set && __active) {
		SerialGPS->end();
		delete SerialGPS;
		delete gps;
		__active = false;
	}

	if (__isIdle) {
		if (__active)
		while (SerialGPS->available() > 0) {
			if (gps->encode(SerialGPS->read())) { // process gps messages
				__local_timer_delta += deltaTime;
				//Serial.println("Timer Delay: " + String(__local_timer_delay) + "Timer Delta: " + String(__local_timer_delta));
				if (__local_timer_delta > __local_timer_delay) {
					__local_timer_delta = 0;
					setTime(gps->time.hour(), gps->time.minute(), gps->time.second(), gps->date.day(), gps->date.month(), gps->date.year());
					__time_is_set = true;
				}
			}
		}
	}
}

void GPS_Device_Driver::displayInfo()
{
	Serial.print(F("Location: "));
	if (gps->location.isValid())
	{
		Serial.print(gps->location.lat(), 6);
		Serial.print(F(","));
		Serial.print(gps->location.lng(), 6);
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F("  Date/Time: "));
	if (gps->date.isValid())
	{
		Serial.print(gps->date.month());
		Serial.print(F("/"));
		Serial.print(gps->date.day());
		Serial.print(F("/"));
		Serial.print(gps->date.year());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F(" "));
	if (gps->time.isValid())
	{
		if (gps->time.hour() < 10) Serial.print(F("0"));
		Serial.print(gps->time.hour());
		Serial.print(F(":"));
		if (gps->time.minute() < 10) Serial.print(F("0"));
		Serial.print(gps->time.minute());
		Serial.print(F(":"));
		if (gps->time.second() < 10) Serial.print(F("0"));
		Serial.print(gps->time.second());
		Serial.print(F("."));
		if (gps->time.centisecond() < 10) Serial.print(F("0"));
		Serial.print(gps->time.centisecond());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.println();
}

void GPS_Device_Driver::OnInit()
{
#ifdef DEBUG
	Serial.println("Start GPS_Device_Driver::OnInit");
#endif // DEBUG
	Device_Driver::OnInit();
	SerialGPS = new SoftwareSerial(5, 4);
	gps = new TinyGPSPlus();
	SerialGPS->begin(9600);
	__active = true;
#ifdef DEBUG
	Serial.println("Ende GPS_Device_Driver::OnInit");
#endif // DEBUG
}

void GPS_Device_Driver::TimerTick()
{
#ifdef DEBUG
	Serial.println("Start GPS_Device_Driver::TimerTick");
#endif // DEBUG
	__utc_time = now();
	__local_time = CE1.toLocal(__utc_time);
#ifdef DEBUG
	Serial.println("Ende GPS_Device_Driver::TimerTick");
#endif // DEBUG
}


void GPS_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case GPS_DEVICE_DRIVER_UTC_TIME:
	{
#ifdef DEBUG
		Serial.println("Start Ntp_Wifi_Device_Driver::DoDeviceMessage - NTP_WIFI_DEVICE_DRIVER_UTC_TIME");
#endif // DEBUG
		//
	}
	break;
	case GPS_DEVICE_DRIVER_LOCAL_TIME:
	{
#ifdef DEBUG
		Serial.println("Start Ntp_Wifi_Device_Driver::DoDeviceMessage - NTP_WIFI_DEVICE_DRIVER_LOCAL_TIME");
#endif // DEBUG
		//
	}
	break;
	}
	}

