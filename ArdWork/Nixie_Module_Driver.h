// Nixie_Module_Driver.h

#ifndef _NIXIE_MODULE_DRIVER_h
#define _NIXIE_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Module_Driver.h"

class Button_Device_Driver;
class Uart_GRBW_Led_Device_Driver;
class Time_Device_Driver;
class Wifi_Device_Driver;
class WebSocket_Wifi_Device_Driver;

class Nixie_Module_Driver : public Module_Driver
{
public:
	Nixie_Module_Driver(uint8_t priority = TASK_PRIORITY_NORMAL);
private:
	Color __sv_color = { 55, 111, 222 };
	int __sv_time_format;
	int __sv_date_time;
	bool __date_time;
	uint __timer_count;
	uint8_t __AnimationCount;
	int __activeAnimaton;
	void SwitchPattern(uint8_t _control);
	void Set_Pattern_Color(int _r, int _g, int _b);
	void Pattern_Prev();
	void Pattern_Next();
	void Pattern_Off();
	void DigiClock();

	Button_Device_Driver *__button;
	Uart_GRBW_Led_Device_Driver* __strip;
	Time_Device_Driver* __time;
	Wifi_Device_Driver* __wifi_device;
	WebSocket_Wifi_Device_Driver * __websocket_device;
protected:
	void DoModuleMessage(Int_Task_Msg message);
	void SetDateTime(int _number);
	void SetTimeFormat(int _number);
	void TimerTick() override;
	void DoTaskMessage(TaskMessage *message);
	void Build_Discriptor();
};


#endif

