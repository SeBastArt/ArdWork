
//#define DASH_NodeMCU09
//#define DASH_NodeMCU10
//#define DASH_ESP01

//#define SLEEP

//#define COMPILE_TEST
#include "Filesystem.h"
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <WebSockets.h>
#include <ArduinoJson.h>
#include "FS.h"
#define PICTURE_NodeMCU_GBR


#ifdef COMPILE_TEST
//Controller
#include "PCF8574_Controller.h"
#include "ESP8266_NodeMCU_Controller.h"
#include "ESP8266_01_Controller.h"
#include "IO_Pin.h"

//Driver
#include "Uart_GRBW_Led_Device_Driver.h"
#include "Distance_Device_Driver.h"
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "OLED_Display_Device_Driver.h"
#include "Temperature_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "WebServer_Wifi_Device_Driver.h"

//Modules
#include "RGBLed_Mqqt_Wifi_Module_Driver.h"
#include "Dash_Mqqt_Wifi_Module_Driver.h"
#include "Led_Button_Module_Driver.h"
#include "Picture_Module_Driver.h"
#endif // COMPILE_TEST


#ifdef PICTURE_NodeMCU_GBRW
//Module
#include "RGBLed_Mqqt_Wifi_Module_Driver.h"
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "IO_Pin.h"
#include "Uart_GRBW_Led_Device_Driver.h"
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
#include "Uart_GRBW_Led_Device_Driver.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "WebServer_Wifi_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#endif // PICTURE_NodeMCU_GBR


#ifdef DASH_NodeMCU09
//Module
#include "Dash_Mqqt_Wifi_Module_Driver.h"
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "IO_Pin.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#endif // DASH_NodeMCU09

#ifdef DASH_NodeMCU10
//Module
#include "Dash_Mqqt_Wifi_Module_Driver.h"
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "IO_Pin.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#endif // DASH_NodeMCU10

#ifdef DASH_ESP01
//Module
#include "Dash_Mqqt_Wifi_Module_Driver.h"
//Controller
#include "ESP8266_01_Controller.h"
#include "IO_Pin.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#endif // DASH_ESP01

//Main
#include "Thread/ThreadManager.h"

FileSystem _filesystem;
ThreadManager threadManager;

uint32_t start;
String WifiSsid;
String password;

void setup() {
	WiFi.forceSleepWake(); // Wlan ON f�r neue Zyklus	
	Serial.begin(115200);

	_filesystem.OpenFile("/config.json");
	WifiSsid = _filesystem.Json_GetvalueFromKey("WifiSsid");
	password = _filesystem.Json_GetvalueFromKey("password");
	_filesystem.CloseFile();

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
	ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	//RGBLed_Mqqt_Wifi_Module_Driver *rgb_mqqt_wifi_module = new RGBLed_Mqqt_Wifi_Module_Driver("ESP8266", ssid, pass);
	Picture_Module_Driver *picture_module = new Picture_Module_Driver();
#endif

#ifdef SLEEP
	start = millis();
	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);
#endif // SLEEP

#ifdef PICTURE_NodeMCU_GBRW
	Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(rgb_mqqt_wifi_module, 28);
	Led_Device_Driver *led = new Led_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	Led_Device_Driver *wifi_status_led = new Led_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	Button_Device_Driver *button = new Button_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D2"), true);
#endif // PICTURE_NodeMCU_GBRW

#ifdef PICTURE_NodeMCU_GBR
	//Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(rgb_mqqt_wifi_module, 28);
	//Led_Device_Driver *led = new Led_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	//Led_Device_Driver *wifi_status_led = new Led_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	//Button_Device_Driver *button = new Button_Device_Driver(rgb_mqqt_wifi_module, esp8266_NodeMCU_controller->Pin("D2"), true);
	//Luxmeter_Device_Driver *luxmeter = new Luxmeter_Device_Driver(rgb_mqqt_wifi_module);

	Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(picture_module, 28);
	Led_Device_Driver *led = new Led_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin("D3"), true);
	Led_Device_Driver *wifi_status_led = new Led_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin(BUILTIN_LED), true);
	Button_Device_Driver *button = new Button_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin("D2"), true);
	Luxmeter_Device_Driver *luxmeter = new Luxmeter_Device_Driver(picture_module);
	//Mqqt_Wifi_Device_Driver *mqqt_wifi = new Mqqt_Wifi_Device_Driver(picture_module, Wifissid, pass, wifi_status_led);
	//WebServer_Wifi_Device_Driver *server_wifi = new WebServer_Wifi_Device_Driver(picture_module, WifiSsid, password, wifi_status_led);
	WebSocket_Wifi_Device_Driver *webSocket_server_wifi = new WebSocket_Wifi_Device_Driver(picture_module, WifiSsid, password);
#endif // PICTURE_NodeMCU_GBR

#ifdef COMPILE_TEST
	PCF8574_Controller* pcf8574_controller = new PCF8574_Controller(0x20);
	ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	ESP8266_01_Controller* esp8266_01_controller = new ESP8266_01_Controller();

	Picture_Module_Driver *picture_module = new Picture_Module_Driver();
	RGBLed_Mqqt_Wifi_Module_Driver *rgb_mqqt_wifi_module = new RGBLed_Mqqt_Wifi_Module_Driver("ESP8266", ssid, pass);
	Dash_Mqqt_Wifi_Module_Driver *mqqt_wifi_module = new Dash_Mqqt_Wifi_Module_Driver("ESP8266", ssid, pass);
	Led_Button_Module_Driver *led_button_module = new Led_Button_Module_Driver();

	Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(picture_module, 28);
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
	picture_module->AddDevice(button);
	picture_module->AddDevice(led);
	picture_module->AddDevice(strip);
	picture_module->AddDevice(luxmeter);
	//picture_module->AddDevice(mqqt_wifi);
	//picture_module->AddDevice(server_wifi);
	picture_module->AddDevice(webSocket_server_wifi);
#endif

	Serial.println("Try to start resident driver...");

#ifdef DASH_ESP01
	Serial.println("Start esp8266_01_controller");
	threadManager.StartThread(esp8266_01_controller);
#endif

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10)
	Serial.println("Start ESP8266-NodeMCU-Controller");
	threadManager.StartThread(esp8266_NodeMCU_controller);
#endif

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01)
	Serial.println("Start Dash_Mqqt_Wifi_Module_Driver");
	threadManager.StartThread(dash_mqqt_wifi_module);
#endif


#if defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	Serial.println("Start ESP8266-NodeMCU-Controller");
	threadManager.StartThread(esp8266_NodeMCU_controller);

	/*Serial.println("Start Uart_GRBW_Led_Device_Driver");
	threadManager.StartThread(rgb_mqqt_wifi_module);*/

	Serial.println("Start Picture_Module_Driver");
	threadManager.StartThread(picture_module);

	Serial.println("Start UART_Rgb_Led-Driver");
	threadManager.StartThread(strip);

	Serial.println("Start Luxmeter-Driver");
	threadManager.StartThread(luxmeter);
#endif

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01) || defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	Serial.println("Start Wifi-Status LED-Driver");
	threadManager.StartThread(wifi_status_led);

	Serial.println("Start LED-Driver");
	threadManager.StartThread(led);

	Serial.println("Start Button-Driver");
	threadManager.StartThread(button);

	//Serial.println("Start MQQT_Wifi-Driver");
	//threadManager.StartThread(mqqt_wifi);

	//Serial.println("Start WebServer_Wifi-Driver");
	//threadManager.StartThread(server_wifi);

	Serial.println("Start WebSocket_Wifi-Driver");
	threadManager.StartThread(webSocket_server_wifi);
#endif

	Serial.flush();
	Serial.println("Initialize...");

#if defined(DASH_NodeMCU09) || defined(DASH_NodeMCU10) || defined(DASH_ESP01)
	dash_mqqt_wifi_module->ExecInit();
#endif

#if defined(PICTURE_NodeMCU_GBRW) || defined(PICTURE_NodeMCU_GBR)
	picture_module->ExecInit();
#endif

}

void loop() {

#ifdef SLEEP
	if ((millis() - start) > 30 * 1000) {
		digitalWrite(2, LOW);
		delay(100);
		Serial.println("EnterDeepSleep...");
		WiFi.forceSleepBegin(); // Wlan aus
		ESP.deepSleep(0, WAKE_RFCAL); //gehe schlafen
	}
#endif // SLEEP
	threadManager.Loop();
	}

