
//#define SLEEP
//#define COMPILE_TEST

#include <TaskMacros.h>
#include <MessageTask.h>
#include <FunctionTask.h>
#include <espsoftwareserial\SoftwareSerial.h>
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
#include "Timezone.h" //https://github.com/JChristensen/Timezone
#include "Driver.h"
#include "Publisher.h"

//#define PICTURE_NodeMCU_GRBW
#define NIXIE_NodeMCU_GRBW
//#define COMPILE_TEST



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

#ifdef NIXIE_NodeMCU_GRBW
//Module
#include "Nixie_Module_Driver.h"
#endif // NIXIE_NodeMCU_GRBW


#ifdef PICTURE_NodeMCU_GRBW
//Module
#include "Picture_Module_Driver.h"
//Controller
#include "ESP8266_NodeMCU_Controller.h"
#include "Uart_RGB_Led_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"
//Driver
#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Wifi_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Ntp_Wifi_Device_Driver.h"
#endif // PICTURE_NodeMCU_GRBW



uint32_t start;

void setup() {
	Serial.begin(115200);

#ifdef SLEEP
	WiFi.forceSleepWake();  // Wlan ON fuer neue Zyklus	
	start = millis();
	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);
#endif // SLEEP



#ifdef PICTURE_NodeMCU_GRBW
	//Module
	Picture_Module_Driver *picture_module = new Picture_Module_Driver();

	//Driver
	Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(picture_module, 28);	
	//Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(picture_module, 4);
	ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	Button_Device_Driver *button = new Button_Device_Driver(picture_module);
	Luxmeter_Device_Driver *luxmeter = new Luxmeter_Device_Driver(picture_module);
	Wifi_Device_Driver *wifi_device = new Wifi_Device_Driver(picture_module);
	Ntp_Wifi_Device_Driver *ntp_device = new Ntp_Wifi_Device_Driver(picture_module);
	WebSocket_Wifi_Device_Driver *webSocket_server_wifi = new WebSocket_Wifi_Device_Driver(picture_module);
#endif // PICTURE_NodeMCU_GRBW

#ifdef NIXIE_NodeMCU_GRBW
	Nixie_Module_Driver *nixie_module = new Nixie_Module_Driver();
#endif // NIXIE_NodeMCU_GRBW

#ifdef COMPILE_TEST
	PCF8574_Controller* pcf8574_controller = new PCF8574_Controller(0x20);
	ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	ESP8266_01_Controller* esp8266_01_controller = new ESP8266_01_Controller();

	Picture_Module_Driver *picture_module = new Picture_Module_Driver();

	Uart_RGB_Led_Device_Driver *strip = new Uart_RGB_Led_Device_Driver(picture_module, 28);
	Led_Device_Driver *led = new Led_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin(BUILTIN_LED), true);
	Button_Device_Driver *button = new Button_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin(4), true);

	Uart_GRBW_Led_Device_Driver *strip = new Uart_GRBW_Led_Device_Driver(picture_module, 120);
	Uart_RGB_Led_Device_Driver *strip = new Uart_RGB_Led_Device_Driver(picture_module, 120);
	OLED_Display_Device_Driver *oled_display = new OLED_Display_Device_Driver(picture_module, 1);
	Temperature_Device_Driver *temp_sensor = new Temperature_Device_Driver(picture_module, esp8266_NodeMCU_controller->Pin("D8"));
	Luxmeter_Device_Driver *luxmeter_sensor = new Luxmeter_Device_Driver(picture_module);
	Distance_Device_Driver *distance_sensor = new Distance_Device_Driver(picture_module, 8, 6);

	Wifi_Device_Driver *wifi_device = new Wifi_Device_Driver(picture_module);
	Mqqt_Wifi_Device_Driver *mqqt_wifi_device = new Mqqt_Wifi_Device_Driver(picture_module, led);
	WebSocket_Wifi_Device_Driver *server_wifi = new WebSocket_Wifi_Device_Driver(picture_module, led);
	Ntp_Wifi_Device_Driver *ntp_device = new Ntp_Wifi_Device_Driver(picture_module);
#endif // COMPILE_TEST

#ifdef PICTURE_NodeMCU_GRBW
	//mqqt_wifi->SetHostName("ESP_MQQT_RGB_WZ");
	//mqqt_wifi->SetInTopic("esp/wohnzimmer/rgb");
	//mqqt_wifi->SetOutTopic("fhem/wohnzimmer/rgb/set");
	//mqqt_wifi->SetMQQTBroker("192.168.178.33");
	wifi_device->AddCommunicationClient(webSocket_server_wifi);
	wifi_device->AddCommunicationClient(ntp_device);
	picture_module->AddDevice(button);
	picture_module->AddDevice(strip);
	picture_module->AddDevice(wifi_device);
	picture_module->AddDevice(webSocket_server_wifi);
	picture_module->AddDevice(ntp_device);
	picture_module->AddDevice(luxmeter);
#endif

	Serial.println(F("Try to start resident driver..."));

#ifdef PICTURE_NodeMCU_GRBW
	taskManager.StartTask(picture_module);
	taskManager.StartTask(esp8266_NodeMCU_controller);
	taskManager.StartTask(strip);
	taskManager.StartTask(luxmeter);
	taskManager.StartTask(button);
	//taskManager.StartTask(mqqt_wifi);
	taskManager.StartTask(wifi_device);
	taskManager.StartTask(webSocket_server_wifi);
	taskManager.StartTask(ntp_device);
#endif

	Serial.flush();
	Serial.println(F("Initialize..."));

#ifdef PICTURE_NodeMCU_GRBW
	picture_module->ExecStart();
#endif

#ifdef NIXIE_NodeMCU_GRBW
	nixie_module->ExecStart();
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
	Driver::taskManager.Loop();
}