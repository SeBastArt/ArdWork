// 
// 
// 

#include "PCF8574_Controller.h"
#include "PCF8574_Controller_Consts.h"

PCF8574_Controller::PCF8574_Controller(uint32_t adress, uint8_t priority) : I2C_Controller(adress, priority) {

	for (int i = 0; i < 8; i++) {
		IO_Pin* pin;
		switch (i)
		{
		case 0:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 4, "P0");
			break;
		case 1:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 5, "P1");
			break;
		case 2:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 6, "P2");
			break;
		case 3:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 7, "P3");
			break;
		case 4:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 9, "P4");
			break;
		case 5:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 10, "P5");
			break;
		case 6:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 11, "P6");
			break;
		case 7:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 12, "P7");
			break;
		default:
			pin = new IO_Pin(PCF8574_CONTROLLER_PIN_ID_START + i, 12, "P7");
		}
		pins.PushBack(pin);
	}
	InitPins();
}

