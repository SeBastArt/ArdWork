// Led_Button_Module_Driver.h

#ifndef _LED_BUTTON_MODULE_DRIVER_h
#define _LED_BUTTON_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Module_Driver.h"
#include "Led_Button_Module_Driver_Consts.h"
#include "OLED_Display_Device_Driver.h"
#include "Temperature_Device_Driver.h"

class Button_Device_Driver;
class Led_Device_Driver;

class Led_Button_Module_Driver : public Module_Driver
{
private:
	Button_Device_Driver *resident_button;
	Led_Device_Driver *resident_Led;
	OLED_Display_Device_Driver *resident_oled_display;
	Temperature_Device_Driver *resident_temp;
 protected:
	 void DoUpdate(uint32_t deltaTime);
	 void DoBeforeSuspend();
	 void DoBeforeShutdown();
	 void DoAfterInit();
	 void DoModuleMessage(Int_Thread_Msg message);

public:
	Led_Button_Module_Driver(uint8_t priority = THREAD_PRIORITY_NORMAL);
	
	void AddComponent(OLED_Display_Device_Driver *oled_display);
	void AddComponent(Led_Device_Driver *led);
	void AddComponent(Button_Device_Driver *button);
	void AddComponent(Temperature_Device_Driver *temp);
};

//extern Led_Button_Module_DriverClass Led_Button_Module_Driver;

#endif

