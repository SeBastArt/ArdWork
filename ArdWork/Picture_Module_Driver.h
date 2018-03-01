// Picture_Module_Driver.h

#ifndef _PICTURE_MODULE_DRIVER_h
#define _PICTURE_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Module_Driver.h"

class Button_Device_Driver;
class Uart_GRBW_Led_Device_Driver;
class Time_Device_Driver;
class Luxmeter_Device_Driver;
class Wifi_Device_Driver;

class Picture_Module_Driver : public Module_Driver
{
public:
	Picture_Module_Driver(uint8_t priority = TASK_PRIORITY_NORMAL);
private:
	int __sv_autoBrightness = 0;
	float __sv_relBrightness = 100;
	Color __sv_color = { 55, 111, 222 };
	uint8_t __AnimationCount;
	int __activeAnimaton;
	int __envBrightness;
	void Pattern_Prev();
	void Pattern_Next();
	void Pattern_Off();
	void SwitchPattern(uint8_t _control);
	void SetStripBrightness();
	void SetRelBrightness(uint8_t _relBrightness);
	void Set_Auto_Brightness(bool _state);
	void Set_Pattern_Color(int _r, int _g, int _b);

	Button_Device_Driver *__button;
	Uart_GRBW_Led_Device_Driver* __strip;
	Time_Device_Driver* __time;
	Luxmeter_Device_Driver* __lux;
	Wifi_Device_Driver* __wifi;
protected:
	void OnInit() override;
	void DoModuleMessage(Int_Task_Msg message);
	void TimerTick() override;
	void AnimateClock();
	void DoTaskMessage(TaskMessage *message);
	void Build_Discriptor();
};

#endif


