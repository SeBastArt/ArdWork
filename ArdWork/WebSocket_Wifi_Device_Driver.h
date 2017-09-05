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

class WebSocket_Wifi_Device_Driver : public Wifi_Device_Driver
{
public:
	WebSocket_Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED = NULL, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void UpdateControls();
private:
	static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
	static ESP8266WebServer *server;
	static WebSocketsServer *webSocket;
protected:
	void UpdateComm(uint32_t deltaTime);
	void InitComm();
};

#endif