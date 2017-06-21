// WiFi_Module_Driver.h

#ifndef _WIFI_MODULE_DRIVER_h
#define _WIFI_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>

#include "Module_Driver.h"
#include <ArduinoOTA.h>

class Led_Device_Driver;

class WiFi_Module_Driver : public Module_Driver, public ArduinoOTAClass
{
private:
	IPAddress localhost;
	String ssid;
	String password;
	
	bool isConnected;
	uint32_t conn_delta;
	uint32_t conn_delay;

protected:
	Led_Device_Driver *wifi_status_Led;
	String hostname;
protected:
	void DoUpdate(uint32_t deltaTime);
	void DoBeforeSuspend();
	void DoBeforeShutdown();
	void DoAfterInit();
	void DoModuleMessage(Int_Thread_Msg message);
	void SetupOTA();
protected:
	virtual void SetupComm() = 0;
	virtual void UpdateComm(uint32_t deltaTime) = 0;
	virtual void CheckComm(uint32_t deltaTime) = 0;
	virtual void UpdateHardware(uint32_t deltaTime) = 0;

public:
	WiFi_Module_Driver(String _hostname, String _ssid, String _password, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void SetWiFiStatusLed(Led_Device_Driver *led);
	void SetHostName(String _hostname);
};




#endif

