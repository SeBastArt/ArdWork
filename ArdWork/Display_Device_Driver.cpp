// 
// 
// 

#include "Display_Device_Driver.h"
#include "Display_Device_Driver_Consts.h"

Display_Device_Driver::Display_Device_Driver(Module_Driver* module, uint8_t priority) : Device_Driver(module, priority) {
	__DriverType = DISPLAY_DEVICE_DRIVER_TYPE;
}

void Display_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{
	int messageID = message.GetID();
	switch (messageID)
	{
	case DISPLAY_DRIVER_CLEAR:
	{
		DoClear();
	}
	break;
	case DISPLAY_DRIVER_FLUSH:
	{
		DoFlush();
	}
	break;
	case DISPLAY_DRIVER_SET_CURSOR:
	{
		int x_pos = message.GetIntParamByIndex(1);
		int y_pos = message.GetIntParamByIndex(2);
		DoSetCursor(x_pos, y_pos);
	}
	break;
	case DISPLAY_DRIVER_DRAW_CHAR:
	{
		unsigned char c = message.GetCharParamByIndex(1);
		DoDrawChar(c);
	}
	break;
	case DISPLAY_DRIVER_DRAW_STRING:
	{
		String text = message.GetStringParamByIndex(1);
		DoDrawString(text);
	}
	break;
	
	case DISPLAY_DRIVER_START_SCROLLING_RIGHT:
	{
		int start = message.GetIntParamByIndex(1);
		int end = message.GetIntParamByIndex(2);
		DoStartScrollRight(start, end);
	}
	break;
	case DISPLAY_DRIVER_START_SCROLLING_LEFT:
	{
		int start = message.GetIntParamByIndex(1);
		int end = message.GetIntParamByIndex(2);
		DoStartScrollLeft(start, end);
	}
	case DISPLAY_DRIVER_STOP_SCROLL:
	{
		DoStopScroll();
	}
	break;
	}
	DoCustomDisplayMessage(message);
}


void Display_Device_Driver::Exec_Display_Clear() {
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_CLEAR);
	PostMessage(&message);
}

void Display_Device_Driver::Exec_Display_Flush() {
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_FLUSH);
	PostMessage(&message);
}


void Display_Device_Driver::Exec_Display_SetCursor(int16_t x, int16_t y)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_FLUSH);
	message->AddParam(x);
	message->AddParam(y);
	PostMessage(&message);
}

void Display_Device_Driver::Exec_Display_DrawChar(unsigned char c)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_DRAW_CHAR);
	message->AddParam(c);
	PostMessage(&message);
}

void Display_Device_Driver::Exec_Display_DrawString(String text)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_DRAW_STRING);
	message->AddParam(text);
	PostMessage(&message);
}

void Display_Device_Driver::Exec_Display_StartScrollRight(uint8_t start, uint8_t stop)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_START_SCROLLING_RIGHT);
	message->AddParam(start);
	message->AddParam(stop);
	PostMessage(&message);
}

void Display_Device_Driver::Exec_Display_StartScrollLeft(uint8_t start, uint8_t stop)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_START_SCROLLING_LEFT);
	message->AddParam(start);
	message->AddParam(stop);
	PostMessage(&message);
}

void Display_Device_Driver::Exec_Display_StopScroll()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISPLAY_DRIVER_STOP_SCROLL);
	PostMessage(&message);
}
