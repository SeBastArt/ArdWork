// 
// 
// 

#include "ESP8266_01_Controller.h"

ESP8266_01_Controller::ESP8266_01_Controller(uint8_t priority) : ESP8266_Controller(priority)
{
	for (int i = 0; i < 11; i++) {
		IO_Pin* pin;
		switch (i)
		{
		case 0:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 2, "GPIO2");
			break;
		case 1:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 0, "GPIO0");
			break;
		case 2:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 3, "RXD");
			break;
		default: 
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, LED_BUILTIN, "LED_BUILTIN");
		}

		pins.PushBack(pin);
	}
}






