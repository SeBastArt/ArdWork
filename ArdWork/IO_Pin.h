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
	 bool __pinMode;
	 bool __pinState;
	 bool __isActive;
	 int __pinID;
	 uint8_t __pinGPIO;
	 String __pinName;

	 void SetPinMode(bool _pinMode) { __pinMode = _pinMode; }
	 bool GetPinMode() const { return __pinMode; }

	 void SetPinState(bool _pinState) { __pinState = _pinState; }
	 bool GetPinState() const { return __pinState; }

	 String GetPinName() const { return __pinName; }
	 int GetPinID() const { return __pinID; }
	 uint8_t GetPinGPIO() const { return __pinGPIO; }
 public:
	 IO_Pin(int _PinID, uint8_t _PinGPIO, String _pinName) :
		 __pinID(_PinID),
		 __pinGPIO(_PinGPIO),
		 __pinName(_pinName)
	 {
		 __pinMode = OUTPUT;
		 __pinState = LOW;
		 __isActive = false;
	 };
	 bool IsActive;
	 Property<bool, IO_Pin> pinMode{this,  &IO_Pin::SetPinMode, &IO_Pin::GetPinMode };
	 Property<bool, IO_Pin> pinState{ this,  &IO_Pin::SetPinState, &IO_Pin::GetPinState };
	 Property<String, IO_Pin> pinName{ this,  nullptr, &IO_Pin::GetPinName };
	 Property<int, IO_Pin> pinID{ this,  nullptr, &IO_Pin::GetPinID };
	 Property<uint8_t, IO_Pin> pinGPIO{ this,  nullptr, &IO_Pin::GetPinGPIO };
};

#endif


