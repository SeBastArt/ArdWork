//
//
//
#include "Led_Button_Module_Driver.h"
#include "Led_Device_Driver.h"
#include "Button_Device_Driver.h"

Led_Button_Module_Driver::Led_Button_Module_Driver(uint8_t priority) : Module_Driver(priority) {};

void Led_Button_Module_Driver::DoUpdate(uint32_t deltaTime)
{
	ThreadMessage* pMessage;
	if (PopMessage(&pMessage))
	{
		if ((pMessage)->Class == MessageClass_Button)
		{
			ButtonMessage* pButton = (ButtonMessage*)(pMessage);
			if (pButton->State == THREAD_MSG_BUTTONSTATE_PRESSED) // any state that is pressed
			{
				if (pButton->Id == resident_button->GetButtonPinID()) {
					resident_Led->Exec_Set_IO_Pin_High();
					resident_oled_display->Exec_Display_Clear();
					for (int16_t i = 0; i < 16; i += 2) {
						resident_oled_display->Exec_Display_DrawCircle(64, 32, i, WHITE);
						/*resident_oled_display->Exec_Display_Flush();
						delay(1);*/
					}
					resident_oled_display->Exec_Display_SetCursor(0, 0);
					resident_oled_display->Exec_Display_SetTextColor(WHITE);
					int act_temp = resident_temp->Get_Temperature();
					String act_temp_string = String(act_temp);
					Serial.println(act_temp_string);
					resident_oled_display->Exec_Display_DrawString(act_temp_string);
				}
			}
			else if (pButton->State == THREAD_MSG_BUTTONSTATE_RELEASED)
			{
				if (pButton->Id == resident_button->GetButtonPinID()) {
					resident_Led->Exec_Set_IO_Pin_Low();
					resident_oled_display->Exec_Display_Flush();
				}
			}
			else if (pButton->State == THREAD_MSG_BUTTONSTATE_AUTOREPEAT)
			{
				if (pButton->Id == resident_button->GetButtonPinID()) {
					resident_Led->Exec_Set_IO_Pin_Blink(500);
				}
			}
		}
	}
}

void Led_Button_Module_Driver::AddComponent(Button_Device_Driver *button) {
	AddDevice(button);
	resident_button = button;
}

void Led_Button_Module_Driver::AddComponent(Led_Device_Driver *led) {
	AddDevice(led);
	resident_Led = led;
}
void Led_Button_Module_Driver::AddComponent(OLED_Display_Device_Driver *oled_display) {
	AddDevice(oled_display);
	resident_oled_display = oled_display;
}

void Led_Button_Module_Driver::AddComponent(Temperature_Device_Driver *temp) {
	AddDevice(temp);
	resident_temp = temp;
}

void Led_Button_Module_Driver::DoBeforeSuspend()
{
}

void Led_Button_Module_Driver::DoBeforeShutdown()
{
}

void Led_Button_Module_Driver::DoAfterInit()
{
	Serial.println("Led-Button-Module-Driver initialized!");
}

void Led_Button_Module_Driver::DoModuleMessage(Int_Thread_Msg message)
{
}

