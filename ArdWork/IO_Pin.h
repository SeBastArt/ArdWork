// IO_Pin.h

#ifndef _IO_PIN_h
#define _IO_PIN_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class IO_Pin
{
 private:
	 bool pin_mode;
	 bool pin_state;
	 bool is_active;
	 int pin_ID;
	 uint8_t pin_number;
	 String pin_name;

 public:
	 IO_Pin(int ID, uint8_t number, String name);
	 
	 bool PinMode();
	 bool PinState();
	 int GetID();

	 uint8_t PinNumber();
	 String PinName();

	 void SetPinMode(bool type);
	 void SetPinState(bool state);

	 bool IsActive;
};

#endif


