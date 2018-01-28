// I2C_Controller.h

#ifndef _I2C_CONTROLLER_h
#define _I2C_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Base_Controller.h"
#include "Base_Controller_Consts.h"


class I2C_Controller: public Base_Controller
{
 private:
	 uint32_t controller_adress;
	 void DoUpdatePins(uint32_t deltaTime);
	 byte ExpanderRead(uint8_t i2caddr);
	 void ExpanderWrite(uint8_t i2caddr, byte data);
 public:
	 I2C_Controller(uint32_t adress, uint8_t priority = TASK_PRIORITY_NORMAL);
};

#endif


