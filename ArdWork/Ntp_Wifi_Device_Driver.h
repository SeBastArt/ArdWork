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
#include "Timezone.h" //https://github.com/JChristensen/Timezone
#include "Device_Driver.h"
#include "Ntp_Wifi_Device_Driver_Consts.h"


class Ntp_Wifi_Device_Driver : public Device_Driver, public CommunicationClient
{
private:
	//typedef struct
	//{
	//	unsigned char second; // 0-59
	//	unsigned char minute; // 0-59
	//	unsigned char hour;   // 0-23
	//	unsigned char day;    // 1-31
	//	unsigned char month;  // 1-12
	//	unsigned char year;   // 0-99 (representing 2000-2099)
	//}
	//date_time_t;

	static unsigned long getNTPTimestamp();
	static unsigned long sendNTPpacket(IPAddress& address);
	static time_t getNTP_UTCTime1970();
	time_t __local_time;
	time_t __utc_time;
	void SyncTimeWithNTP();
	time_t GetLocalTime() const;
	time_t GetUtcTime() const;
protected:
	void TimerTick() override; 
	void OnBuild_Descriptor();
	void OnNotifyConnected();
	void OnNotifyConnectionLost();
	void OnNotifyOnline();
	void DoDeviceMessage(Int_Task_Msg message);
	void DoUpdate(uint32_t deltaTime);
public:
	Property<time_t, Ntp_Wifi_Device_Driver> local_time{ this, nullptr, &Ntp_Wifi_Device_Driver::GetLocalTime };
	Property<time_t, Ntp_Wifi_Device_Driver> utc_time{ this, nullptr, &Ntp_Wifi_Device_Driver::GetUtcTime };
	Ntp_Wifi_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);



};

#endif

