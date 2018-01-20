// 
// 
// 

#include "I2C_Controller.h"
#include "Wire.h"

I2C_Controller::I2C_Controller(uint32_t adress, uint8_t priority) : Base_Controller(priority)
{
	Wire.begin();
	controller_adress = adress;
}


byte I2C_Controller::ExpanderRead(uint8_t i2caddr) {
	uint8_t _data = -1;
	Wire.requestFrom(i2caddr, uint8_t(1));
	if (Wire.available()) {
		_data = Wire.read();
	}
	return _data;
}


void I2C_Controller::ExpanderWrite(uint8_t i2caddr, byte data)
{
	Wire.beginTransmission(i2caddr);
	Wire.write(data);
	Wire.endTransmission();
}

void I2C_Controller::DoUpdatePins(uint32_t deltaTime) {
	byte data = ExpanderRead(controller_adress);
	if (data > -1) {
		for (int i = 0; i < pins.Size(); i++) {
			if (pins[i]->PinMode() == INPUT) {
				pins[i]->SetPinState(data & (1 << i) ? HIGH : LOW);
			}
		}
	}

	byte daten = 0;
	for (int i = 0; i < pins.Size(); ++i) {
		if (pins[pins.Size() - 1 - i]->PinMode() == OUTPUT) {
			if (pins[pins.Size() - 1 - i]->PinState() == 1)
				daten *= 2;
			else if (pins[pins.Size() - 1 - i]->PinState() == 0)
				daten = daten * 2 + 1;
		}
		else {
			if (pins[pins.Size() - 1 - i]->PinState() == 1)
				daten = daten * 2 + 1;
			else if (pins[pins.Size() - 1 - i]->PinState() == 0)
				daten *= 2;
		}
	}
	ExpanderWrite(controller_adress, daten);
}

