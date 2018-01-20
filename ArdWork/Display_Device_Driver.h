#ifndef _DISPLAY_DEVICE_DRIVER_h
#define _DISPLAY_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Device_Driver.h"

class Display_Device_Driver : public Device_Driver
{
public:
	Display_Device_Driver(Module_Driver* module, uint8_t priority = THREAD_PRIORITY_NORMAL);
private:
	void DoDeviceMessage(Int_Thread_Msg message);

protected:
	virtual void DoClear() = 0;
	virtual void DoFlush() = 0;

	virtual void DoSetCursor(int16_t x, int16_t y) = 0;

	virtual void DoDrawChar(unsigned char c) = 0;
	virtual void DoDrawString(String text) = 0;

	virtual void DoStartScrollRight(uint8_t start, uint8_t stop) = 0;
	virtual void DoStartScrollLeft(uint8_t start, uint8_t stop) = 0;
	virtual void DoStopScroll() = 0;

	virtual void DoCustomDisplayMessage(Int_Thread_Msg message) = 0;
public:
	 void Exec_Display_Clear();
	 void Exec_Display_Flush();

	 void Exec_Display_SetCursor(int16_t x, int16_t y);

	 void Exec_Display_DrawChar(unsigned char c);
	 void Exec_Display_DrawString(String text);

	 void Exec_Display_StartScrollRight(uint8_t start = 0x00, uint8_t stop = 0x0F);
	 void Exec_Display_StartScrollLeft(uint8_t start = 0x00, uint8_t stop = 0x0F);
	 void Exec_Display_StopScroll();

};

#endif


