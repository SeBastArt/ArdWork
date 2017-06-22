// WebServer_Wifi_Device_Driver.h

#ifndef _WEBSERVER_WIFI_DEVICE_DRIVER_h
#define _WEBSERVER_WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include "Wifi_Device_Driver.h"
#include "WebServer_Wifi_Device_Driver_Consts.h"


class WebServer_Wifi_Device_Driver : public Wifi_Device_Driver
{
	class Request
	{
	public:
		void Test(String _request);
		void SetRequest(String _request);
		void FillCtrl(String requestpart);
		void Clear();
		String sPath;
		String sParam;
		String sCmd;
		String sHead;
		boolean isEmpty;
	};

public:
	WebServer_Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED = NULL, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void UpdateControls();
private:
	Control *control;
	unsigned long ulReqcount;
	unsigned long ulReconncount;
	Request *request;
	WiFiServer *server;
	String GenerateHeader(String _response, bool _found);
protected:
	
	void UpdateComm(uint32_t deltaTime);
	String GenerateInteger(Control * _conrtrol);
	String GenerateSwitch(Control * _conrtrol);
	String GenerateButton(Control * _conrtrol);
	void SendResponse(WiFiClient _client, String _header, String _response);
	void CheckComm(WiFiClient _client);
	void InitComm();
};

#endif


