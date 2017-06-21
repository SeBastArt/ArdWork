// Wifi_Device_Driver.h

#ifndef _WIFI_DEVICE_DRIVER_h
#define _WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include "Device_Driver.h"
#include "Wifi_Device_Driver_Consts.h"
#include "Led_Device_Driver.h"
#include "Comm_Device_Driver.h"

class Wifi_Device_Driver : public Device_Driver, public Comm_Device_Driver
{
public:
	Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED = NULL , uint8_t priority = THREAD_PRIORITY_NORMAL);
	void SetWiFiStatusLed(Led_Device_Driver *led);
	void SetHostName(String _hostname);
private:
	IPAddress localhost;
	String Ssid;
	String Password;
	
	bool isConnected;
	uint32_t conn_delta;
	uint32_t conn_delay;

protected:
	static Led_Device_Driver *statusLED;
	String hostname;
	virtual void UpdateComm(uint32_t deltaTime) = 0;
	virtual void InitComm() = 0;
private:
	void DoAfterInit();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);
};


#endif

