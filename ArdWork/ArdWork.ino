
//#define SLEEP
//#define COMPILE_TEST

#include "Filesystem.h"
#include <ArduinoJson.h>
#include "Timezone.h" //https://github.com/JChristensen/Timezone
#include "Driver.h"
#include "Publisher.h"

#define PICTURE_NodeMCU_GRBW
//#define NIXIE_NodeMCU_GRBW
//#define COMPILE_TEST



#ifdef COMPILE_TEST
//Controller
#include "PCF8574_Controller.h"
#include "ESP8266_NodeMCU_Controller.h"
#include "ESP8266_01_Controller.h"
#include "IO_Pin.h"
#include "Pin_Manager.h"

//Driver
#include "Button_Device_Driver.h"
#include "Distance_Device_Driver.h"
#include "GPS_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "Ntp_Wifi_Device_Driver.h"
#include "OLED_Display_Device_Driver.h"
#include "Temperature_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"
#include "Uart_RGB_Led_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Wifi_Device_Driver.h"
//Modules
#include "Nixie_Module_Driver.h"
#include "Picture_Module_Driver.h"
#endif // COMPILE_TEST

#ifdef NIXIE_NodeMCU_GRBW
//Module
#include "Nixie_Module_Driver.h"
#endif // NIXIE_NodeMCU_GRBW


#ifdef PICTURE_NodeMCU_GRBW
//Module
#include "Picture_Module_Driver.h"
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
#endif // PICTURE_NodeMCU_GRBW

#ifdef NIXIE_NodeMCU_GRBW
	Nixie_Module_Driver *nixie_module = new Nixie_Module_Driver();
#endif // NIXIE_NodeMCU_GRBW

#ifdef COMPILE_TEST
	PCF8574_Controller* pcf8574_controller = new PCF8574_Controller(0x20);
	ESP8266_NodeMCU_Controller* esp8266_NodeMCU_controller = new ESP8266_NodeMCU_Controller();
	ESP8266_01_Controller* esp8266_01_controller = new ESP8266_01_Controller();

	Picture_Module_Driver *picture_module = new Picture_Module_Driver();
	Nixie_Module_Driver *nixie_module = new Nixie_Module_Driver();

	Button_Device_Driver *button = new Button_Device_Driver(picture_module);
	Distance_Device_Driver *distance_sensor = new Distance_Device_Driver(picture_module);
	GPS_Device_Driver *gps = new GPS_Device_Driver(picture_module);
	Led_Device_Driver *led = new Led_Device_Driver(picture_module);
	Luxmeter_Device_Driver *luxmeter_sensor = new Luxmeter_Device_Driver(picture_module);
	Mqqt_Wifi_Device_Driver *mqqt_wifi_device = new Mqqt_Wifi_Device_Driver(picture_module);
	Ntp_Wifi_Device_Driver *ntp_device = new Ntp_Wifi_Device_Driver(picture_module);
	OLED_Display_Device_Driver *oled_display = new OLED_Display_Device_Driver(picture_module);
	Temperature_Device_Driver *temp_sensor = new Temperature_Device_Driver(picture_module);
	Uart_GRBW_Led_Device_Driver *strip_GRBW = new Uart_GRBW_Led_Device_Driver(picture_module);
	Uart_RGB_Led_Device_Driver *strip_RGB = new Uart_RGB_Led_Device_Driver(picture_module);
	WebSocket_Wifi_Device_Driver *server_wifi = new WebSocket_Wifi_Device_Driver(picture_module);
	Wifi_Device_Driver *wifi_device = new Wifi_Device_Driver(picture_module);	
#endif // COMPILE_TEST

	Serial.flush();
	Serial.println(F("Initialize..."));

#ifdef PICTURE_NodeMCU_GRBW
	Driver::taskManager.StartTask(picture_module);
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