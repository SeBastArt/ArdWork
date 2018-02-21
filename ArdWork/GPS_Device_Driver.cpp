#include "GPS_Device_Driver.h"


TimeChangeRule CEST1 = { "CEST", Last, Sun, Mar, 2, 120 };     //Central European Summer Time
TimeChangeRule CET1 = { "CET ", Last, Sun, Oct, 3, 60 };       //Central European Standard Time
Timezone CE1(CEST1, CET1);

REGISTERIMPL(GPS_Device_Driver);

//#define DEBUG

GPS_Device_Driver::GPS_Device_Driver(Module_Driver * module, uint8_t priority) :
	Device_Driver(module, priority) {
	__DriverType = GPS_DEVICE_DRIVER_TYPE;
	__local_time = 0;
	__utc_time = 0;
	SetTimerDelay(15000);
}


void GPS_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println("Start GPS_Device_Driver::Build_Descriptor");
#endif // DEBUG
	__descriptor->name = ("GPS-Driver");
	__descriptor->descr = ("Sync with GPS-Sattelites for real time");
	__descriptor->published = true;

	Time_CtrlElem *ctrlElem_utc_time = new Time_CtrlElem(GPS_DEVICE_DRIVER_UTC_TIME, &__utc_time, false, ("UTC Time"), ("world time"));

	Time_CtrlElem *ctrlElem_local_time = new Time_CtrlElem(GPS_DEVICE_DRIVER_LOCAL_TIME, &__local_time, false, ("Local Time"), ("The Time in your Location"));

	__descriptor->Add_Descriptor_Element(ctrlElem_utc_time);
	__descriptor->Add_Descriptor_Element(ctrlElem_local_time);
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::Build_Descriptor");
#endif // DEBUG
}

void GPS_Device_Driver::DoUpdate(uint32_t deltaTime)
{
	if (__isIdle)
		while (Serial.available() > 0) {
			if (gps->encode(Serial.read())) { // process gps messages
												// when TinyGPS reports new data...
				unsigned long age;
				int Year;
				byte Month, Day, Hour, Minute, Second;
				gps->crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
				if (age < 500) {
					// set the Time to the latest GPS reading
					setTime(Hour, Minute, Second, Day, Month, Year);
				}
			}
		}
}

void GPS_Device_Driver::OnInit()
{
#ifdef DEBUG
	Serial.println("Start GPS_Device_Driver::OnInit");
#endif // DEBUG
	SerialGPS = new SoftwareSerial(13, 15, false, 256);
	gps = new TinyGPS();
	SerialGPS->begin(115200);

#ifdef DEBUG
	Serial.println("Ende GPS_Device_Driver::OnInit");
#endif // DEBUG
}

void GPS_Device_Driver::TimerTick()
{
#ifdef DEBUG
	Serial.println("Start GPS_Device_Driver::TimerTick");
#endif // DEBUG


	//	__utc_time = now();
	//	__local_time = CE1.toLocal(__utc_time);

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

