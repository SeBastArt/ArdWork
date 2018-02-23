// 
// 
// 

#include "ESP8266_NodeMCU_Controller.h"

ESP8266_NodeMCU_Controller::ESP8266_NodeMCU_Controller(uint8_t priority) : ESP8266_Controller(priority)
{
	for (int i = 0; i < 11; i++) {
		IO_Pin* pin;
		switch (i)
		{
		case 0:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 3, F("RXD"));
			break;
		case 1:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 1, F("D10"));
			break;
		case 2:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 16, F("D0"));
			break;
		case 3:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 5, F("D1"));
			break;
		case 4:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 4, F("D2"));
			break;
		case 5:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 0, F("D3"));
		break;
		case 6:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 2, F("D4"));
			break;
		case 7:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 14, F("D5"));
			break;
		case 8:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 12, F("D6"));
			break;
		case 9:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 13, F("D7"));
			break;
		case 10:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 15, F("D8"));
			break;
		default:
			pin = new IO_Pin(ESP_CONTROLLER_PIN_ID_START + i, 16, F("LED_BUILTIN"));
		}

		pins.PushBack(pin);
	}
}




