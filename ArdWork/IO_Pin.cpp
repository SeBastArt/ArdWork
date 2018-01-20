// 
// 
// 

#include "IO_Pin.h"
#include "Base_Controller.h"

IO_Pin::IO_Pin(int ID, uint8_t number, String name){
	pin_ID = ID;
	pin_number = number;
	pin_name = name;
	pin_state = LOW;
	IsActive = false;
}

bool IO_Pin::PinState() {
	return pin_state;
}

bool IO_Pin::PinMode() {
	return pin_mode;
}

int IO_Pin::GetID() {
	return pin_ID;
}

void IO_Pin::SetPinState(bool state) {
	pin_state = state;
}

void IO_Pin::SetPinMode(bool type) {
	pin_mode = type;
}

uint8_t IO_Pin::PinNumber() {
	return pin_number;
}

String IO_Pin::PinName() {
	return pin_name;
}


