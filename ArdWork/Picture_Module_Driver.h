// Picture_Module_Driver.h

#ifndef _PICTURE_MODULE_DRIVER_h
#define _PICTURE_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Module_Driver.h"

class Picture_Module_Driver : public Module_Driver
{
public:
	Picture_Module_Driver(uint8_t priority = THREAD_PRIORITY_NORMAL);

private:
	int __sv_autoBrightness = 0;
	float __sv_relBrightness = 100;
	Color __sv_color = { 55, 111, 222 };
	int __absBrightness;
	void Pattern_Next();
	void Pattern_Prev();
	void Pattern_Off();
	void SwitchPattern(uint8_t _control);
	void SetStripBrightness();
	void SetRelBrightness(uint8_t _relBrightness);
	void Set_Auto_Brightness(bool _state);
	void Set_Pattern_Color(int _r, int _g, int _b);

protected:
	void DoBeforeSuspend();
	void DoBeforeShutdown();
	void DoAfterInit();
	void DoModuleMessage(Int_Thread_Msg message);

	void DoThreadMessage(ThreadMessage *message);
	void Build_Module_Discriptor();
public:
	void Exec_Pattern_Next();
	void Exec_Pattern_Prev();
	void Exec_Pattern_Off();
};

#endif


