
//#define DASH_NodeMCU09
//#define DASH_NodeMCU10
//#define DASH_ESP01

//#define SLEEP
////#define DEBUG
//#define COMPILE_TEST

#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <TimeLib.h>
#include <Time.h>
#include <TaskManager.h>
#include <Task.h>
#include "Filesystem.h"
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <WebSockets.h>
#include <ArduinoJson.h>

#define PICTURE_NodeMCU_GBR


#ifdef COMPILE_TEST
//Controller
#include "PCF8574_Controller.h"
#include "ESP8266_NodeMCU_Controller.h"
#include "ESP8266_01_Controller.h"
#include "IO_Pin.h"

//Driver
#include "Uart_RGB_Led_Device_Driver.h"
#include "Distance_Device_Driver.h"
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "OLED_Display_Device_Driver.h"
#include "Temperature_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "WebServer_Wifi_Device_Driver.h"

//Modules
#include "Picture_Module_Driver.h"
#endif // COMPILE_TEST


#ifdef PICTURE_NodeMCU_GBRW
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "IO_Pin.h"
#include "Uart_RGB_Led_Device_Driver.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#endif // PICTURE_NodeMCU_GBRW

#ifdef PICTURE_NodeMCU_GBR
//Module
#include "Picture_Module_Driver.h"
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "IO_Pin.h"
#include "Uart_RGB_Led_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Ntp_Wifi_Device_Driver.h"
#endif // PICTURE_NodeMCU_GBR


#ifdef DASH_NodeMCU09
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "IO_Pin.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#endif // DASH_NodeMCU09

#ifdef DASH_NodeMCU10
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "IO_Pin.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#endif // DASH_NodeMCU10

#ifdef DASH_ESP01
//Controller
#include "ESP8266_01_Controller.h"
#include "IO_Pin.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#endif // DASH_ESP01

TaskManager taskManager;
uint32_t start;

void setup() {
#ifdef SLEEP
	WiFi.forceSleepWake();  // Wlan ON fuer neue Zyklus	
#endif
	Serial.begin(115200);

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01) 
	Dash_Mqqt_Wifi_Module_Driver *dash_mqqt_wifi_module = new Dash_Mqqt_Wifi_Module_Driver("ESP8266", ssid, pass);
#endif

#ifdef DASH_ESP01
	ESP8266_01_Controller* esp8266_01_controller = new ESP8266_01_Controller();
	Led_Device_Driver *led = new Led_Device_Driver(dash_mqqt_wifi_module, esp8266_01_controller->Pin("RXD"), true);
	Led_Device_Driver *wifi_status_led = new Led_Device_Driver(dash_mqqt_wifi_module, esp8266_01_controller->Pin("RXD"), true);
	Button_Device_Driver *button = new Button_Device_Driver(dash_mqqt_wifi_module, esp8266_01_controller->Pin("GPIO0"), true);
#endif // DASH_ESP01

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10)
	ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	Led_Device_Driver *led = new Led_Device_Driver(dash_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	Led_Device_Driver *wifi_status_led = new Led_Device_Driver(dash_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	Button_Device_Driver *button = new Button_Device_Driver(dash_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D7"), true);
#endif // DASH_NodeMCU09

#if defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	//ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	//RGBLed_Mqqt_Wifi_Module_Driver *rgb_mqqt_wifi_module = new RGBLed_Mqqt_Wifi_Module_Driver("ESP8266", ssid, pass);
	Picture_Module_Driver *picture_module = new Picture_Module_Driver();
#endif

#ifdef SLEEP
	start = millis();
	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);
#endif // SLEEP

#ifdef PICTURE_NodeMCU_GBRW
	Uart_RGB_Led_Device_Driver *strip = new Uart_RGB_Led_Device_Driver(rgb_mqqt_wifi_module, 28);
	Led_Device_Driver *led = new Led_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	Led_Device_Driver *wifi_status_led = new Led_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	Button_Device_Driver *button = new Button_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D2"), true);
#endif // PICTURE_NodeMCU_GBRW

#ifdef PICTURE_NodeMCU_GBR
	//Uart_RGB_Led_Device_Driver *strip = new Uart_RGB_Led_Device_Driver(picture_module, 24);
	Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(picture_module, 28);
	//Led_Device_Driver *led = new Led_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin(LED_BUILTIN), true);
	//Led_Device_Driver *wifi_status_led = new Led_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin(D3), true);
	//Button_Device_Driver *button = new Button_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin("D5"), true);
	//Mqqt_Wifi_Device_Driver *mqqt_wifi = new Mqqt_Wifi_Device_Driver(picture_module, wifi_status_led);
	Wifi_Device_Driver *wifi_device = new Wifi_Device_Driver(picture_module);
	Ntp_Wifi_Device_Driver *ntp_device = new Ntp_Wifi_Device_Driver(picture_module);
	WebSocket_Wifi_Device_Driver *webSocket_server_wifi = new WebSocket_Wifi_Device_Driver(picture_module);
	wifi_device->AddCommunicationClient(webSocket_server_wifi);
	wifi_device->AddCommunicationClient(ntp_device);
	Luxmeter_Device_Driver *luxmeter = new Luxmeter_Device_Driver(picture_module);
#endif // PICTURE_NodeMCU_GBR

#ifdef COMPILE_TEST
	PCF8574_Controller* pcf8574_controller = new PCF8574_Controller(0x20);
	ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	ESP8266_01_Controller* esp8266_01_controller = new ESP8266_01_Controller();

	Picture_Module_Driver *picture_module = new Picture_Module_Driver();
	RGBLed_Mqqt_Wifi_Module_Driver *rgb_mqqt_wifi_module = new RGBLed_Mqqt_Wifi_Module_Driver("ESP8266", ssid, pass);
	Dash_Mqqt_Wifi_Module_Driver *mqqt_wifi_module = new Dash_Mqqt_Wifi_Module_Driver("ESP8266", ssid, pass);

	Uart_RGB_Led_Device_Driver *strip = new Uart_RGB_Led_Device_Driver(picture_module, 28);
	Led_Device_Driver *led = new Led_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin(BUILTIN_LED), true);
	Button_Device_Driver *button = new Button_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin(4), true);

	OLED_Display_Device_Driver *oled_display = new OLED_Display_Device_Driver(picture_module, 1);
	Temperature_Device_Driver *temp_sensor = new Temperature_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin("D8"));
	Luxmeter_Device_Driver *luxmeter_sensor = new Luxmeter_Device_Driver(picture_module);
	Distance_Device_Driver *distance_sensor = new Distance_Device_Driver(picture_module, 8, 6);

	Mqqt_Wifi_Device_Driver *mqqt_wifi_device = new Mqqt_Wifi_Device_Driver(picture_module, ssid, pass, led);
	WebServer_Wifi_Device_Driver *server_wifi = new WebServer_Wifi_Device_Driver(picture_module, ssid, pass, led);
#endif // COMPILE_TEST


#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01)
	dash_mqqt_wifi_module->SetHostName("ESP_MQQT_DASH_WZ");
	dash_mqqt_wifi_module->SetInTopic("esp/schlafzimmer/dash");
	dash_mqqt_wifi_module->SetOutTopic("fhem/schlafzimmer/dash/set");
	dash_mqqt_wifi_module->SetMQQTBroker("192.168.178.33");

	dash_mqqt_wifi_module->SetWiFiStatusLed(wifi_status_led);
	dash_mqqt_wifi_module->AddComponent(button);
	dash_mqqt_wifi_module->AddComponent(led);
#endif


#if defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	//mqqt_wifi->SetHostName("ESP_MQQT_RGB_WZ");
	//mqqt_wifi->SetInTopic("esp/wohnzimmer/rgb");
	//mqqt_wifi->SetOutTopic("fhem/wohnzimmer/rgb/set");
	//mqqt_wifi->SetMQQTBroker("192.168.178.33");
	//picture_module->AddDevice(button);
	//picture_module->AddDevice(led);
	picture_module->AddDevice(strip);
	//picture_module->AddDevice(mqqt_wifi);
	picture_module->AddDevice(wifi_device);
	picture_module->AddDevice(webSocket_server_wifi);
	picture_module->AddDevice(ntp_device);
	picture_module->AddDevice(luxmeter);
#endif

	Serial.println(F("Try to start resident driver..."));

#ifdef DASH_ESP01
	Serial.println(F("Start esp8266_01_controller");
	TaskManager.StartTask(esp8266_01_controller);
#endif

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10)
	Serial.println(F("Start ESP8266-NodeMCU-Controller");
	TaskManager.StartTask(esp8266_NodeMCU_controller);
#endif

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01)
	Serial.println(F("Start Dash_Mqqt_Wifi_Module_Driver");
	TaskManager.StartTask(dash_mqqt_wifi_module);
#endif


#if defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	//TaskManager.StartTask(esp8266_NodeMCU_controller);
	taskManager.StartTask(picture_module);
	taskManager.StartTask(strip);
	taskManager.StartTask(luxmeter);
#endif

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01) || defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	//taskManager.StartTask(wifi_status_led);
	//taskManager.StartTask(led);
	//taskManager.StartTask(button);
	//taskManager.StartTask(mqqt_wifi);
	//taskManager.StartTask(server_wifi);
	taskManager.StartTask(wifi_device);
	taskManager.StartTask(webSocket_server_wifi);
	taskManager.StartTask(ntp_device);
#endif

	Serial.flush();
	Serial.println(F("Initialize..."));

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01)
	dash_mqqt_wifi_module->ExecInit();
#endif

#if defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	picture_module->ExecStart();
#endif
}

void loop() {

#ifdef SLEEP
	if ((millis() - start) > 30 * 1000) {
		digitalWrite(2, LOW);
		delay(100);
		Serial.println(F("EnterDeepSleep...");
		WiFi.forceSleepBegin(); // Wlan aus
		ESP.deepSleep(0, WAKE_RFCAL); //gehe schlafen
	}
#endif // SLEEP
	taskManager.Loop();
}