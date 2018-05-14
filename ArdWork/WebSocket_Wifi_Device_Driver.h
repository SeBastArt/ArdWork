// WebSocket_Wifi_Device_Driver.h

#ifndef _WEBSOCKET_WIFI_DEVICE_DRIVER_h
#define _WEBSOCKET_WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ESP8266mDNS.h>

#include "Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver_Consts.h"


struct event_msg
{
	bool _filled;
	int _deviceId; 
	int _cmdId;
	String _value;
};

class WebSocket_Wifi_Device_Driver : public Device_Driver, public CommunicationClient
{
	REGISTER(WebSocket_Wifi_Device_Driver);
private:
	static AsyncWebServer *server;
	static AsyncWebSocket  *webSocket;
	static event_msg __event_msg;

	void InitializeServices();

	void GenerateNav(AsyncResponseStream *response, Descriptor_List * _descriptor_list);
	void GenerateTab(AsyncResponseStream *response, Descriptor * _descriptor);
	void GenerateForm(AsyncResponseStream *response, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateDecending(AsyncResponseStream *response, CtrlElem * _ctrl_elem);
	void GenerateColor(AsyncResponseStream *response, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateSelect(AsyncResponseStream *response, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateMultiOption(AsyncResponseStream *response, CtrlElem * _ctrl_elem);
	void GenerateInput(AsyncResponseStream *response, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateButtonGroup(AsyncResponseStream *response, int _deviceId, CtrlElem * _ctrl_elem);
	void GenerateSetButton(AsyncResponseStream *response, int _deviceId, int _cmdId);
	static String Json_GetvalueFromKey(String _text, String _key);
	static void webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t * data, size_t len);
protected:
	void OnBuild_Descriptor();
	void OnNotifyConnected() override;
	void OnNotifyConnectionLost() override;
	void OnNotifyOnline() override;
	void DoDeviceMessage(Int_Task_Msg message);
	void DoUpdate(uint32_t deltaTime);
	void TimerTick() override;
public:
	WebSocket_Wifi_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
};

#endif

