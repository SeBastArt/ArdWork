// Wifi_Device_Driver.h

#ifndef _WIFI_DEVICE_DRIVER_h
#define _WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <ESP8266WiFi.h>        
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>

#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "./DNSServer.h"
#include "Device_Driver.h"
#include "Wifi_Device_Driver_Consts.h"
#include "Led_Device_Driver.h"
#include "Comm_Device_Driver.h"     
#include "Filesystem.h"



class Wifi_Device_Driver : public Device_Driver, public Comm_Device_Driver
{
public:
	Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED = nullptr , uint8_t priority = THREAD_PRIORITY_NORMAL);
	
private:
	FileSystem _filesystem;
	IPAddress localhost;
	String __ssid;
	String __password;
	uint8_t __connection_try;
	uint32_t conn_delta;
	uint32_t conn_delay;
	DNSServer *__dnsServer;
	void SetSSID(String _ssid);
	void SetPassword(String _password);
	void SetMode(int _mode);
	void ConnectToWifi();
	void SaveConnectionParameters();
	void SetupOTA();
protected:
	bool __isConnected;
	bool __isAP;
	bool __fallbackAP;
	String hostname;
	static Led_Device_Driver *statusLED;
	void Build_Descriptor();
	virtual void UpdateComm(uint32_t deltaTime) = 0;
	virtual void InitComm() = 0;

protected:
	void DoAfterInit();
	void ProvideAP();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);
	
public:
	void Exec_Set_SSID(String _ssid);
	void Exec_Set_Password(String _password);
	void Exec_Reconnect();

};


#endif

