// Nixie_Module_Driver.h

#ifndef _NIXIE_MODULE_DRIVER_h
#define _NIXIE_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Module_Driver.h"

class Nixie_Module_Driver : public Module_Driver
{
public:
	Nixie_Module_Driver(uint8_t priority = TASK_PRIORITY_NORMAL);
private:
	Color __sv_color = { 55, 111, 222 };
	uint8_t __AnimationCount;
	int __activeAnimaton;
	void SwitchPattern(uint8_t _control);
	void Set_Pattern_Color(int _r, int _g, int _b);
	void Pattern_Prev();
	void Pattern_Next();
	void Pattern_Off();
	void DigiClock();
protected:
	void DoModuleMessage(Int_Task_Msg message);
	void TimerTick() override;
	void DoTaskMessage(TaskMessage *message);
	void Build_Discriptor();
};


#endif
