//
//
//

#include "OLED_Display_Device_Driver.h"
#include "OLED_Display_Device_Driver_Consts.h"
#include "Fonts\FreeMono9pt7b.h"

OLED_Display_Device_Driver::OLED_Display_Device_Driver(Module_Driver* module, uint8_t _reset_pin, uint8_t priority) : Display_Device_Driver(module, priority) {
	__DriverType = OLED_DISPLAY_DEVICE_DRIVER_TYPE;
	__reset_pin = _reset_pin;
	__display = new Adafruit_SSD1306(__reset_pin);
};

void OLED_Display_Device_Driver::Build_Descriptor() {
	__descriptor->name = F("Display");
	__descriptor->descr = F("a monochrome Display");
}

void OLED_Display_Device_Driver::DoAfterInit()
{
	__display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
	DoClear();
	Serial.println(F("OLED-Driver initialized!"));
}

void OLED_Display_Device_Driver::DoBeforeShutdown()
{

}

void OLED_Display_Device_Driver::DoBeforeSuspend()
{

}

void OLED_Display_Device_Driver::DoUpdate(uint32_t deltaTime) {

}


void OLED_Display_Device_Driver::DoCustomDisplayMessage(Int_Thread_Msg message)
{
	int messageID = message.GetID();
	switch (messageID)
	{
	case OLED_DISPLAY_DRIVER_DRAW_PIXEL:
	{
		uint16_t x_pos; uint16_t y_pos; uint16_t color;
		x_pos = message.GetIntParamByIndex(1);
		y_pos = message.GetIntParamByIndex(2);
		color = message.GetIntParamByIndex(3);
		DoDrawPixel(x_pos, y_pos, color);
	}
	break;
	case OLED_DISPLAY_DRIVER_DRAW_CIRCLE:
	{
		uint16_t x0 = message.GetIntParamByIndex(1);
		uint16_t  y0 = message.GetIntParamByIndex(2);
		int16_t r = message.GetIntParamByIndex(3);
		uint16_t color = message.GetIntParamByIndex(4);
		DoDrawCircle(x0, y0, r, color);
	}
	break;
	case OLED_DISPLAY_DRIVER_DRAW_FILLED_CIRCLE:
	{
		uint16_t x0 = message.GetIntParamByIndex(1);
		uint16_t  y0 = message.GetIntParamByIndex(2);
		int16_t r = message.GetIntParamByIndex(3);
		uint16_t color = message.GetIntParamByIndex(4);
		DoDrawFilledCircle(x0, y0, r, color);
	}
	break;
	case OLED_DISPLAY_DRIVER_DRAW_TRIANGLE:
	{
		uint16_t x0 = message.GetIntParamByIndex(1);
		uint16_t y0 = message.GetIntParamByIndex(2);
		uint16_t x1 = message.GetIntParamByIndex(3);
		uint16_t y1 = message.GetIntParamByIndex(4);
		uint16_t x2 = message.GetIntParamByIndex(5);
		uint16_t y2 = message.GetIntParamByIndex(6);
		uint16_t color = message.GetIntParamByIndex(7);
		DoDrawTriangle(x0, y0, x1, y1, x2, y2, color);
	}
	break;
	case OLED_DISPLAY_DRIVER_DRAW_FILLED_TRIANGLE:
	{
		uint16_t x0 = message.GetIntParamByIndex(1);
		uint16_t y0 = message.GetIntParamByIndex(2);
		uint16_t x1 = message.GetIntParamByIndex(3);
		uint16_t y1 = message.GetIntParamByIndex(4);
		uint16_t x2 = message.GetIntParamByIndex(5);
		uint16_t y2 = message.GetIntParamByIndex(6);
		uint16_t color = message.GetIntParamByIndex(7);
		DoDrawFilledTriangle(x0, y0, x1, y1, x2, y2, color);
	}
	break;
	case OLED_DISPLAY_DRIVER_DRAW_ROUND_RECT:
	{
		uint16_t x0 = message.GetIntParamByIndex(1);
		uint16_t y0 = message.GetIntParamByIndex(2);
		uint16_t w = message.GetIntParamByIndex(3);
		uint16_t h = message.GetIntParamByIndex(4);
		uint16_t radius = message.GetIntParamByIndex(5);
		uint16_t color = message.GetIntParamByIndex(6);
		DoDrawRoundRect(x0, y0, w, h, radius, color);
	}
	break;
	case OLED_DISPLAY_DRIVER_DRAW_FILLED_ROUND_RECT:
	{
		uint16_t x0 = message.GetIntParamByIndex(1);
		uint16_t y0 = message.GetIntParamByIndex(2);
		uint16_t w = message.GetIntParamByIndex(3);
		uint16_t h = message.GetIntParamByIndex(4);
		uint16_t radius = message.GetIntParamByIndex(5);
		uint16_t color = message.GetIntParamByIndex(6);
		DoDrawFilledRoundRect(x0, y0, w, h, radius, color);
	}
	break;
	case OLED_DISPLAY_DRIVER_DRAW_BITMAP:
	{
		uint16_t x = message.GetIntParamByIndex(1);
		uint16_t y = message.GetIntParamByIndex(2);
		uint8_t bitmap = message.GetIntParamByIndex(3);
		uint16_t w = message.GetIntParamByIndex(4);
		uint16_t h = message.GetIntParamByIndex(5);
		uint16_t color = message.GetIntParamByIndex(6);
		DoDrawBitmap(x, y, &bitmap, w, h, color);
	}
	break;
	case DISPLAY_DRIVER_DRAW_CHAR_COMPLETE:
	{
		uint16_t x = message.GetIntParamByIndex(1);
		uint16_t y = message.GetIntParamByIndex(2);
		unsigned char c = message.GetCharParamByIndex(3);
		uint16_t color = message.GetIntParamByIndex(4);
		uint16_t bg_colorh = message.GetIntParamByIndex(5);
		uint16_t size = message.GetIntParamByIndex(6);
		DoDrawChar(x, y, c, color, bg_colorh, size);
	}
	break;
	case OLED_DISPLAY_DRIVER_SET_TEXT_COLOR:
	{
		uint16_t color = message.GetIntParamByIndex(1);
		DoSetTextColor(color);
	}
	break;
	case OLED_DISPLAY_DRIVER_SET_TEXT_SIZE:
	{
		uint8_t size = message.GetIntParamByIndex(1);
		DoSetTextSize(size);
	}
	break;
	case OLED_DISPLAY_DRIVER_SET_TEXT_WRAP:
	{
		boolean w = message.GetIntParamByIndex(1);
		DoSetTextSize(w);
	}
	break;
	case OLED_DISPLAY_DRIVER_SET_ROTATION:
	{
		uint8_t r = message.GetIntParamByIndex(1);
		DoSetRotation(r);
	}
	break;
	case OLED_DISPLAY_DRIVER_SET_FONT:
	{
		GFXfont* font = new GFXfont(FreeMono9pt7b);
		DoSetFont(font);
	}
	break;
	case OLED_DISPLAY_DRIVER_START_SCROLL_DIAG_RIGHT:
	{
		uint8_t start = message.GetIntParamByIndex(1);
		uint8_t end = message.GetIntParamByIndex(1);
		DoStartScrollDiagRight(start, end);
	}
	break;
	case OLED_DISPLAY_DRIVER_START_SCROLL_DIAG_LEFT:
	{
		uint8_t start = message.GetIntParamByIndex(1);
		uint8_t end = message.GetIntParamByIndex(1);
		DoStartScrollDiagLeft(start, end);
	}
	break;
	}
}


void OLED_Display_Device_Driver::DoClear() {
	__display->clearDisplay();
	__display->display();
}

void OLED_Display_Device_Driver::DoFlush() {
	__display->display();
}

void OLED_Display_Device_Driver::DoInvert(uint8_t i)
{
	__display->invertDisplay(i);
}

void OLED_Display_Device_Driver::DoDrawPixel(uint16_t x_pos, uint16_t y_pos, uint16_t color) {
	__display->drawPixel(x_pos, y_pos, color);
}

void OLED_Display_Device_Driver::DoDrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	__display->drawCircle(x0, y0, r, color);
}

void OLED_Display_Device_Driver::DoDrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	__display->fillCircle(x0, y0, r, color);
}

void OLED_Display_Device_Driver::DoDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	__display->drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void OLED_Display_Device_Driver::DoDrawFilledTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	__display->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void OLED_Display_Device_Driver::DoDrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
	__display->drawRoundRect(x0, y0, w, h, radius, color);
}

void OLED_Display_Device_Driver::DoDrawFilledRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
	__display->fillRoundRect(x0, y0, w, h, radius, color);
}

void OLED_Display_Device_Driver::DoDrawBitmap(int16_t x, int16_t y, uint8_t * bitmap, int16_t w, int16_t h, uint16_t color)
{
	__display->drawBitmap(x, y, bitmap, w, h, color);
}


void OLED_Display_Device_Driver::DoSetCursor(int16_t x, int16_t y)
{
	__display->setCursor(x, y);
}

void OLED_Display_Device_Driver::DoSetTextColor(uint16_t color)
{
	__display->setTextColor(color);
}

void OLED_Display_Device_Driver::DoSetTextSize(uint8_t size)
{
	__display->setTextSize(size);
}

void OLED_Display_Device_Driver::DoSetTextWrap(boolean wrap)
{
	__display->setTextWrap(wrap);
}

void OLED_Display_Device_Driver::DoSetRotation(uint8_t rotaion)
{
	__display->setRotation(rotaion);
}

void OLED_Display_Device_Driver::DoSetFont(const GFXfont* font)
{
	__display->setFont(font);
}

void OLED_Display_Device_Driver::DoDrawChar(unsigned char c)
{
	__display->write(c);
}


void OLED_Display_Device_Driver::DoDrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	__display->drawChar(x, y, c, color, bg, size);
}

void OLED_Display_Device_Driver::DoDrawString(String text) {
	__display->print(text);
}

void OLED_Display_Device_Driver::DoStartScrollRight(uint8_t start, uint8_t stop) {
	__display->startscrollright(start, stop);
}

void OLED_Display_Device_Driver::DoStartScrollLeft(uint8_t start, uint8_t stop) {
	__display->startscrollleft(start, stop);
}

void OLED_Display_Device_Driver::DoStartScrollDiagRight(uint8_t start, uint8_t stop)
{
	__display->startscrolldiagright(start, stop);
}

void OLED_Display_Device_Driver::DoStartScrollDiagLeft(uint8_t start, uint8_t stop)
{
	__display->startscrolldiagleft(start, stop);
}

void OLED_Display_Device_Driver::DoStopScroll()
{
	__display->stopscroll();
}


void OLED_Display_Device_Driver::Exec_Display_Invert(uint8_t i)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_INVERT);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawPixel(uint16_t x_pos, uint16_t y_pos, uint16_t color) {
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_PIXEL);
	message->AddParam(x_pos);
	message->AddParam(y_pos);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_CIRCLE);
	message->AddParam(x0);
	message->AddParam(y0);
	message->AddParam(r);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_FILLED_CIRCLE);
	message->AddParam(x0);
	message->AddParam(y0);
	message->AddParam(r);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_TRIANGLE);
	message->AddParam(x0);
	message->AddParam(y0);
	message->AddParam(x1);
	message->AddParam(y1);
	message->AddParam(x2);
	message->AddParam(y2);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawFilledTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_FILLED_TRIANGLE);
	message->AddParam(x0);
	message->AddParam(y0);
	message->AddParam(x1);
	message->AddParam(y1);
	message->AddParam(x2);
	message->AddParam(y2);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_ROUND_RECT);
	message->AddParam(x0);
	message->AddParam(y0);
	message->AddParam(w);
	message->AddParam(h);
	message->AddParam(radius);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawFilledRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_FILLED_ROUND_RECT);
	message->AddParam(x0);
	message->AddParam(y0);
	message->AddParam(w);
	message->AddParam(h);
	message->AddParam(radius);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_DRAW_BITMAP);
	message->AddParam(x);
	message->AddParam(y);
	message->AddParam(*bitmap);
	message->AddParam(w);
	message->AddParam(h);
	message->AddParam(color);
	PostMessage(&message);
}


void OLED_Display_Device_Driver::Exec_Display_DrawChar(int16_t x, int16_t y, const char* c, uint16_t color, uint8_t size)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_DRAW_CHAR_COMPLETE);
	message->AddParam(x);
	message->AddParam(y);
	message->AddParam(c);
	message->AddParam(color);
	message->AddParam(size);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_DrawChar(unsigned char c)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_DRAW_CHAR);
	message->AddParam(c);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_SetTextColor(uint16_t color)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_SET_TEXT_COLOR);
	message->AddParam(color);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_SetTextSize(uint8_t size)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_SET_TEXT_SIZE);
	message->AddParam(size);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_SetTextWrap(boolean wrap)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_SET_TEXT_WRAP);
	message->AddParam(wrap);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_SetRotation(uint8_t rotaion)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_SET_ROTATION);
	message->AddParam(rotaion);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_SetFont(const GFXfont * font)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_SET_FONT);
	PostMessage(&message);
}


void OLED_Display_Device_Driver::Exec_Display_StartScrollDiagRight(uint8_t start, uint8_t stop)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_START_SCROLL_DIAG_RIGHT);
	message->AddParam(start);
	message->AddParam(stop);
	PostMessage(&message);
}

void OLED_Display_Device_Driver::Exec_Display_StartScrollDiagLeft(uint8_t start, uint8_t stop)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(OLED_DISPLAY_DRIVER_START_SCROLL_DIAG_LEFT);
	message->AddParam(start);
	message->AddParam(stop);
	PostMessage(&message);
}







