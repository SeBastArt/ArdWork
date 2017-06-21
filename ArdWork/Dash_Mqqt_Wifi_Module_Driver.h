// Dash_Mqqt_Wifi_Module_Driver.h

#ifndef _DASH_MQQT_WIFI_MODULE_DRIVER_h
#define _DASH_MQQT_WIFI_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Mqqt_Wifi_Module_Driver.h"

class Button_Device_Driver;
class Led_Device_Driver;
class Uart_GRBW_Led_Device_Driver;

class Dash_Mqqt_Wifi_Module_Driver : public Mqqt_Wifi_Module_Driver
{
 private:
	 Button_Device_Driver *resident_button;
	 Led_Device_Driver *resident_Led;
	 Uart_GRBW_Led_Device_Driver *resident_stripe;

	 void UpdateHardware(uint32_t deltaTime);

	 void DoMQTTMessage(String message);
 public:
	 Dash_Mqqt_Wifi_Module_Driver(String _hostname, String _ssid, String _password, uint8_t priority = THREAD_PRIORITY_NORMAL);
	 void AddComponent(Led_Device_Driver *led);
	 void AddComponent(Button_Device_Driver *button);
};


#endif

