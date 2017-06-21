// 
// 
// 

#include "ESP8266_Controller.h"

ESP8266_Controller::ESP8266_Controller(uint8_t priority) : Base_Controller(priority) {};

void ESP8266_Controller::DoUpdatePins(uint32_t deltaTime) {
	for (int i = 0; i < pins.Size(); ++i) {
		if (pins[i]->IsActive == true) {
			pinMode(pins[i]->PinNumber(), pins[i]->PinMode());
			if (pins[i]->PinMode() == OUTPUT) {
				digitalWrite(pins[i]->PinNumber(), pins[i]->PinState());
			}
			else if (pins[i]->PinMode() == INPUT) {
				bool state = digitalRead(pins[i]->PinNumber()) == LOW ? LOW : HIGH;
				pins[i]->SetPinState(state);
			}
		}
	}
}

