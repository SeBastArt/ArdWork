// WebSocket_Wifi_Device_Driver.h

#ifndef _WEBSOCKET_WIFI_DEVICE_DRIVER_h
#define _WEBSOCKET_WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include "Wifi_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver_Consts.h"


struct event_msg
{
	bool _filled;
	int _deviceId; 
	int _cmdId;
	String _value;
};

class WebSocket_Wifi_Device_Driver : public Wifi_Device_Driver
{
private:
	int accuracy_delta;
	int accuracy_delay;
	void GenerateNav(WiFiClient * client, Descriptor_List * _descriptor_list);
	void GenerateTab(WiFiClient * client, Descriptor * _descriptor);
	void GenerateForm(WiFiClient * client, int _deviceId, Ctrl_Elem * _ctrl_elem);
	void GenerateDecending(WiFiClient * client, Ctrl_Elem * _ctrl_elem);
	void GenerateColor(WiFiClient * client, int _deviceId, Ctrl_Elem * _ctrl_elem);
	void GenerateSelect(WiFiClient * client, int _deviceId, Ctrl_Elem * _ctrl_elem);
	void GenerateOption(WiFiClient * client, Ctrl_Elem * _ctrl_elem);
	void GenerateInput(WiFiClient * client, int _deviceId, Ctrl_Elem * _ctrl_elem);
	void GenerateButton(WiFiClient * client, int _deviceId, Ctrl_Elem * _ctrl_elem);
	void GenerateSetButton(WiFiClient * client, int _deviceId, int _cmdId);
public:
	WebSocket_Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED = NULL, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void UpdateControls();
private:
	static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
	static ESP8266WebServer *server;
	static WebSocketsServer *webSocket;
protected:
	static event_msg __event_msg;
	void UpdateComm(uint32_t deltaTime);
	void InitComm();

	static String Json_GetvalueFromKey(String Text, String key);

	void Build_Descriptor();

};

#endif