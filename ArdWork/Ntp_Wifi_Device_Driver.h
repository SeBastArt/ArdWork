// Ntp_Wifi_Device_Driver.h

#ifndef _NTP_WIFI_DEVICE_DRIVER_h
#define _NTP_WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Time.h> //https://github.com/PaulStoffregen/Time
#include "Device_Driver.h"
#include "Ntp_Wifi_Device_Driver_Consts.h"

class Ntp_Wifi_Device_Driver : public Device_Driver, public CommunicationClient
{
	REGISTER(Ntp_Wifi_Device_Driver);
private:
	static unsigned long getNTPTimestamp();
	static unsigned long sendNTPpacket(IPAddress& address);
	static time_t getNTP_UTCTime1970();
	static bool __time_is_set;
	time_t __local_time;
	time_t __utc_time;
	void SyncTimeWithNTP();
	time_t GetLocalTime() const;
	time_t GetUtcTime() const;
	int __sv_timezone;
	int __local_counter;
	void StartGetTime();
	void SetTimezone(int _timezone);
protected:
	void DoUpdate(uint32_t deltaTime);
	void TimerTick() override; 
	void OnBuild_Descriptor();
	void OnNotifyOnline();
	void DoDeviceMessage(Int_Task_Msg message);
public:
	virtual ~Ntp_Wifi_Device_Driver() {};
	Property<time_t, Ntp_Wifi_Device_Driver> local_time{ this, nullptr, &Ntp_Wifi_Device_Driver::GetLocalTime };
	Property<time_t, Ntp_Wifi_Device_Driver> utc_time{ this, nullptr, &Ntp_Wifi_Device_Driver::GetUtcTime };
	Ntp_Wifi_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
	void Exec_Start_Get_Time();
	void Exec_Set_Timezone(int _timezone);
};

#endif

