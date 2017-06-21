// OLED_Display_Device_Driver.h

#ifndef _OLED_DISPLAY_DEVICE_DRIVER_h
#define _OLED_DISPLAY_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Display_Device_Driver.h"

class OLED_Display_Device_Driver : public Display_Device_Driver
{
public:
	OLED_Display_Device_Driver(Module_Driver* module, uint8_t _reset_pin, uint8_t priority = THREAD_PRIORITY_NORMAL);

//Divice Driver
private:
	void DoAfterInit();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoCustomDisplayMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);

//OLED_Diplay_device_Driver
private:
	Adafruit_SSD1306 *display;
	uint8_t reset_pin;

protected:
	void DoClear();
	void DoFlush();
	void DoInvert(uint8_t i);
	
	//Painting
	void DoDrawPixel(uint16_t x_pos, uint16_t y_pos, uint16_t color);
	void DoDrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void DoDrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void DoDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color);
	void DoDrawFilledTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void DoDrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void DoDrawFilledRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color);
	void DoDrawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
	
	//Text
	void DoSetCursor(int16_t x, int16_t y);
	void DoSetTextColor(uint16_t color);
	void DoSetTextSize(uint8_t size);
	void DoSetTextWrap(boolean wrap);
	void DoSetRotation(uint8_t rotaion);
	void DoSetFont(const GFXfont *font = NULL);
	void DoDrawChar(unsigned char c);
	void DoDrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
	void DoDrawString(String text);

	//Srcolling
	void DoStartScrollRight(uint8_t start, uint8_t stop);
	void DoStartScrollLeft(uint8_t start, uint8_t stop);
	void DoStartScrollDiagRight(uint8_t start, uint8_t stop);
	void DoStartScrollDiagLeft(uint8_t start, uint8_t stop);
	void DoStopScroll();

public:
	void Exec_Display_Invert(uint8_t i);

	//Painting
	void Exec_Display_DrawPixel(uint16_t x_pos, uint16_t y_pos, uint16_t color);
	void Exec_Display_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void Exec_Display_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void Exec_Display_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void Exec_Display_DrawFilledTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void Exec_Display_DrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void Exec_Display_DrawFilledRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void Exec_Display_DrawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

	void Exec_Display_DrawChar(int16_t x, int16_t y, const char* c, uint16_t color, uint8_t size);
	void Exec_Display_DrawChar(unsigned char c);

	//Text
	void Exec_Display_SetTextColor(uint16_t color);
	void Exec_Display_SetTextSize(uint8_t size);
	void Exec_Display_SetTextWrap(boolean wrap);
	void Exec_Display_SetRotation(uint8_t rotaion);
	void Exec_Display_SetFont(const GFXfont *font = NULL);

	//Srcolling
	void Exec_Display_StartScrollDiagRight(uint8_t start = 0x00, uint8_t stop = 0x0F);
	void Exec_Display_StartScrollDiagLeft(uint8_t start = 0x00, uint8_t stop = 0x0F);
};


#endif

