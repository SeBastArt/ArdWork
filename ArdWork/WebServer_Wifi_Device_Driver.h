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
public:
	WebServer_Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED = NULL, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void UpdateControls();
private:
	unsigned long ulReqcount;
	unsigned long ulReconncount;
	WiFiServer *server;
	String GenerateHeader(String _response, bool _found);
	String GenerateValue(uint16_t _driverId, Value_Publisher * _pub_elem);
	String GenerateSwitch(uint16_t _driverId, Switch_Publisher * _pub_elem);
	String GenerateButton(uint16_t _driverId, Button_Publisher * _pub_elem);
	String GetKey(String requestpart);
	String GetValue(String requestpart);
protected:
	void ParseRequest(String _request);
	void UpdateComm(uint32_t deltaTime);
	void SendResponse(WiFiClient _client, String _header, String _response);
	void CheckComm(WiFiClient _client);
	void InitComm();

	
};

#endif


