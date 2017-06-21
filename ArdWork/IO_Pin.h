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
	 uint32_t pin_ID;
	 uint8_t pin_number;
	 String pin_name;

 public:
	 IO_Pin(uint32_t ID, uint8_t number, String name);
	 
	 bool PinMode();
	 bool PinState();
	 uint32_t GetID();

	 uint8_t PinNumber();
	 String PinName();

	 void SetPinMode(bool type);
	 void SetPinState(bool state);

	 bool IsActive;
};

#endif

