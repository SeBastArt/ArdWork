// 
// 
// 

#include "ESP8266_Controller.h"

ESP8266_Controller::ESP8266_Controller(uint8_t priority) : Base_Controller(priority) {};

void ESP8266_Controller::DoUpdatePins(uint32_t deltaTime) {
	for (int i = 0; i < pins.Size(); ++i) {
		if (pins[i]->IsActive == true) {
			pinMode(pins[i]->pinGPIO, pins[i]->pinMode);
			if (pins[i]->pinMode == OUTPUT) {
				digitalWrite(pins[i]->pinGPIO, pins[i]->pinState);
			}
			else if (pins[i]->pinMode  == INPUT) {
				bool state = digitalRead(pins[i]->pinGPIO) == LOW ? LOW : HIGH;
				pins[i]->pinState = state;
			}
		}
	}
}


