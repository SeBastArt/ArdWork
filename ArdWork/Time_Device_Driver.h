#ifndef _TIME_DEVICE_DRIVER_h
#define _TIME_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Time.h> //https://github.com/PaulStoffregen/Time
#include "Timezone.h" //https://github.com/JChristensen/Timezone
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "Device_Driver.h"
#include "Time_Device_Driver_Consts.h"

#include "TinyGPS++.h"
#include <SoftwareSerial.h>


class Time_Device_Driver : public Device_Driver, public CommunicationClient
{
	REGISTER(Time_Device_Driver);
private:
	//NTP
	int __ntp_counter;
	static bool __time_is_set;
	
	void StartGetNTPTime();
	static unsigned long getNTPTimestamp();
	static unsigned long sendNTPpacket(IPAddress& address);
	static time_t getNTP_UTCTime1970();
	void SyncTimeWithNTP();
	
	
	//GPS
	
	bool __hasGPSModule;
	void StartGetGPSTime();
	SoftwareSerial *SerialGPS;
	TinyGPSPlus *gps;
	IO_Pin* __rx;
	IO_Pin* __tx;
	void EndGPS();

	bool __GPSactive;
	time_t __local_time;
	time_t __utc_time;
	int __sv_timezone = 0;
	int __sv_time_source = 0;
	int __local_timer;

	time_t GetLocalTime() const;
	time_t GetUtcTime() const;
protected:
	void SetTimeSource(int _number);
	void SetTimezone(int _timezone);
	void StartGetTime();
	
	void DoUpdate(uint32_t deltaTime);
	void OnInit() override;
	void TimerTick() override;
	void DoDeviceMessage(Int_Task_Msg message);
	void OnBuild_Descriptor();

	//CommunicationClient
	void OnNotifyConnected() override;
	void OnNotifyConnectionLost() override;
	void OnNotifyOnline() override;
public:
	Time_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
	void SetGPSModule(IO_Pin * _rx, IO_Pin * _tx);
	Property<time_t, Time_Device_Driver> local_time{ this, nullptr, &Time_Device_Driver::GetLocalTime };
	Property<time_t, Time_Device_Driver> utc_time{ this, nullptr, &Time_Device_Driver::GetUtcTime };
	
	void Exec_Start_Get_Time();
	void Exec_Set_Timezone(int _timezone);
	void Exec_Set_Timesource(int _timesource);
};




#endif