// 
// 
// 

#include "Base_Controller.h"



Base_Controller::Base_Controller(uint8_t priority) : Task(MsToTaskTime(priority)) {
	/*pins = new Vector <IO_Pin*>;*/
};

void Base_Controller::OnUpdate(uint32_t deltaTime) {
	DoUpdatePins(deltaTime);
}

IO_Pin* Base_Controller::GetPin(uint8_t _GPIONumber) {
	IO_Pin* result = nullptr;
	for (int i = 0; i < pins.Size(); i++) {
		if (pins[i]->pinGPIO == _GPIONumber) {
			result = pins[i];
			result->IsActive = true;
		}
	}
	return result;
}

IO_Pin* Base_Controller::GetPin(String name) {
	IO_Pin* result = nullptr;
	for (int i = 0; i < pins.Size(); i++) {
		if (String(pins[i]->pinName).equals(name)) {
			result = pins[i];
			result->IsActive = true;
			result->pinMode = INPUT;
		}
	}
	return result;
}



