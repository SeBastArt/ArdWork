// WebSocket_Wifi_Device_Driver.h

#ifndef _WEBSOCKET_WIFI_DEVICE_DRIVER_h
#define _WEBSOCKET_WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>

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

	static ESP8266WebServer *server;
	static WebSocketsServer *webSocket;
	static event_msg __event_msg;

	void GenerateNav(WiFiClient * client, Descriptor_List * _descriptor_list);
	void GenerateTab(WiFiClient * client, Descriptor * _descriptor);
	void GenerateForm(WiFiClient * client, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateDecending(WiFiClient * client, CtrlElem * _ctrl_elem);
	void GenerateColor(WiFiClient * client, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateBoolean(WiFiClient * client, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateSelect(WiFiClient * client, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateBooleanOption(WiFiClient * client, CtrlElem * _ctrl_elem);
	void GenerateMultiOption(WiFiClient * client, CtrlElem * _ctrl_elem);
	void GenerateInput(WiFiClient * client, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateButtonGroup(WiFiClient * client, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateSetButton(WiFiClient * client, int _deviceId, int _cmdId);
	
	static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
	static String Json_GetvalueFromKey(String Text, String key);

public:
	WebSocket_Wifi_Device_Driver(Module_Driver* module, Led_Device_Driver *_statusLED = nullptr, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void UpdateControls();

//herited
protected:
	void UpdateComm(uint32_t deltaTime);
	void InitComm();
	
};

#endif

