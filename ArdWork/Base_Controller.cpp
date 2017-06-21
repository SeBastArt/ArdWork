// 
// 
// 

#include "Base_Controller.h"


Base_Controller::Base_Controller(uint8_t priority) : Thread(MsToThreadTime(priority)) {
	/*pins = new Vector <IO_Pin*>;*/
};



void Base_Controller::InitPins() {
	for (int i = 0; i < pins.Size(); i++) {
		pins[i]->SetPinMode(OUTPUT);
		pins[i]->SetPinState(LOW);
		pins[i]->IsActive = false;
	}
}


bool Base_Controller::OnStart() {

}

void Base_Controller::OnStop() {

}

void Base_Controller::OnUpdate(uint32_t deltaTime) {
	DoUpdatePins(deltaTime);
}

IO_Pin* Base_Controller::Pin(uint8_t number) {
	IO_Pin* result = nullptr;
	for (int i = 0; i < pins.Size(); i++) {
		if (pins[i]->PinNumber() == number) {
			result = pins[i];
			result->IsActive = true;
		}
	}
	return result;
}

IO_Pin* Base_Controller::Pin(String name) {
	IO_Pin* result = nullptr;
	for (int i = 0; i < pins.Size(); i++) {
		if (pins[i]->PinName() == name) {
			result = pins[i];
			result->IsActive = true;
		}
	}
	return result;
}


