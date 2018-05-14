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

#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "./DNSServer.h"
#include "Device_Driver.h"
#include "Wifi_Device_Driver_Consts.h"
#include "Led_Device_Driver.h"
//#include "Comm_Device_Driver.h"     
#include "Filesystem.h"



class Wifi_Device_Driver : public Device_Driver, public CommunicationProvider
{
	REGISTER(Wifi_Device_Driver);
public:
	Wifi_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
	
private:
	String __ssid;
	String __password;
	uint8_t __connection_try;
	uint32_t conn_delta;
	uint32_t conn_delay;
	DNSServer *__dnsServer;
	struct ping_option pingopt;
	bool __isMSDN;
	void SetSSID(String _ssid);
	void SetPassword(String _password);
	void SetMode(uint8_t _mode);
	void ResetConnection();
	void ConnectToWifi();
	void SetupOTA();
	void StartMSDNServices();
	
	void Ping();
	static void callback_reply(void * arg, void * pdata);
protected:
	bool __WiFi_isConnected;
	bool __AP_isConnected;
	bool __run_isAp;
	bool __isNotifyOnline;
	static bool __isOnline;
	int __isAP;
	String hostname;
	static Led_Device_Driver *statusLED;
	void OnBuild_Descriptor() override;
	void TimerTick() override;
protected:
	void OnInit() override;
	void ProvideAP();
	void DoDeviceMessage(Int_Task_Msg message);
	void DoUpdate(uint32_t deltaTime);
	
public:
	void Exec_Set_SSID(String _ssid);
	void Exec_Set_Password(String _password);
	void Exec_Reconnect();

};


#endif


