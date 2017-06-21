// 
// 
// 

#include "Module_Driver.h"
#include "Device_Driver.h"

extern "C" {
#include "user_interface.h"
}

Module_Driver::Module_Driver(uint8_t priority) :
	Driver(priority) {
	device_count = 0;

	button_index = -1;
	distance_index = -1;
	led_index = -1;
	luxmeter_index = -1;
	mqqt_wifi_index = -1;
	oled_display_index = -1;
	temperature_index = -1;
	Uart_GRBW_Led_index = -1;
	webserver_wifi_index = -1;

	ctrl_List = new Vector<Control*>;

	button_list = new Vector <Button_Device_Driver*>;
	distance_list = new Vector <Distance_Device_Driver*>;
	led_list = new Vector <Led_Device_Driver*>;
	luxmeter_list = new Vector <Luxmeter_Device_Driver*>;
	mqqt_wifi_list = new Vector <Mqqt_Wifi_Device_Driver*>;
	oled_display_list = new Vector <OLED_Display_Device_Driver*>;
	temperature_list = new Vector <Temperature_Device_Driver*>;
	Uart_GRBW_Led_list = new Vector <Uart_GRBW_Led_Device_Driver*>;
	webserver_wifi_list = new Vector <WebServer_Wifi_Device_Driver*>;
}

Module_Driver::~Module_Driver()
{
	device_list.Clear();
	ctrl_List->Clear();
	queue.Clear();
}

void Module_Driver::AddDevice(Device_Driver* device)
{
	if ((device)->GetDriverName().equals("Button_Device_Driver")) {
		button_list->PushBack((Button_Device_Driver*)(device));
		button_index++;
	}
	if ((device)->GetDriverName().equals("Distance_Device_Driver")) {
		distance_list->PushBack((Distance_Device_Driver*)(device));
		distance_index++;
	}
	if ((device)->GetDriverName().equals("Led_Device_Driver")) {
		led_list->PushBack((Led_Device_Driver*)(device));
		led_index++;
	}
	if ((device)->GetDriverName().equals("Luxmeter_Device_Driver")) {
		luxmeter_list->PushBack((Luxmeter_Device_Driver*)(device));
		luxmeter_index++;
	}
	if ((device)->GetDriverName().equals("Mqqt_Wifi_Device_Driver")) {
		mqqt_wifi_list->PushBack((Mqqt_Wifi_Device_Driver*)(device));
		mqqt_wifi_index++;
	}
	if ((device)->GetDriverName().equals("OLED_Display_Device_Driver")) {
		oled_display_list->PushBack((OLED_Display_Device_Driver*)(device));
		oled_display_index++;
	}
	if ((device)->GetDriverName().equals("Temperature_Device_Driver")) {
		temperature_list->PushBack((Temperature_Device_Driver*)(device));
		temperature_index++;
	}
	if ((device)->GetDriverName().equals("Uart_GRBW_Led_Device_Driver")) {
		Uart_GRBW_Led_list->PushBack((Uart_GRBW_Led_Device_Driver*)(device));
		Uart_GRBW_Led_index++;
	}
	if ((device)->GetDriverName().equals("WebServer_Wifi_Device_Driver")) {
		webserver_wifi_list->PushBack((WebServer_Wifi_Device_Driver*)(device));
		webserver_wifi_index++;
	}
	device_list.PushBack(device);
	UpdateControls();
	device_count++;
}


bool Module_Driver::PopMessage(ThreadMessage** message) {
	//
	bool messagePopped = false;

	noInterrupts();
	ThreadMessage* temp_message;
	if (!queue.Empty()) {
		temp_message = queue.Front();
		*message = temp_message;
		queue.PopBack();
		if (temp_message->size > 0) {
			messagePopped = true;
		}
	}
	interrupts();
	return messagePopped;
}

void Module_Driver::DoMessage(Int_Thread_Msg message) {
	DoModuleMessage(message);
}

void Module_Driver::DoInit() {
	for (int i = 0; i < device_list.Size(); i++) {
		device_list[i]->ExecInit();
	}
	GetControlsList();
	DoAfterInit();
}

void Module_Driver::DoShutdown() {
	for (int i = 0; i < device_list.Size(); i++) {
		device_list[i]->ExecShutdown();
	}
}

void Module_Driver::DoSuspend() {
	for (int i = 0; i < device_list.Size(); i++) {
		device_list[i]->ExecSuspend();
	}
}

Vector<Control*>* Module_Driver::GetControlsList()
{
	return ctrl_List;
}


bool Module_Driver::SendAsyncThreadMessage(ThreadMessage* message, bool withinIsr) {
	bool result = false;
	if (!withinIsr)
	{
		noInterrupts();
	}
	if (message->size > 0) {
		queue.PushBack(message);
		result = true;
	}
	if (!withinIsr)
	{
		interrupts();
	}
	return result;
}

void Module_Driver::UpdateControls()
{
	ctrl_List->Clear();
	for (int i = 0; i < device_list.Size(); i++) {
		Vector<Control*> *temp_ctrl_List;
		temp_ctrl_List = device_list[i]->GetControls();
		for (size_t j = 0; j < temp_ctrl_List->Size(); j++)
		{
			ctrl_List->PushBack((*temp_ctrl_List)[j]);
		}
	}
}

Button_Device_Driver *Module_Driver::Get_Selected_Button_Device() const
{
	if ((button_index > -1) && (button_index < button_list->Size())) {
		return (*button_list)[button_index];
	}
}

Distance_Device_Driver *Module_Driver::Get_Selected_Distance_Device() const
{
	if ((distance_index > -1) && (distance_index < distance_list->Size())) {
		return (*distance_list)[distance_index];
	}
}

Led_Device_Driver *Module_Driver::Get_Selected_Led_Device() const
{
	if ((led_index > -1) && (led_index < led_list->Size())) {
		return (*led_list)[led_index];
	}
}

Luxmeter_Device_Driver *Module_Driver::Get_Selected_Luxmeter_Device() const
{
	if ((luxmeter_index > -1) && (luxmeter_index < luxmeter_list->Size())) {
		return (*luxmeter_list)[luxmeter_index];
	}
}

Mqqt_Wifi_Device_Driver *Module_Driver::Get_Selected_Mqqt_Wifi_Device() const
{
	if ((mqqt_wifi_index > -1) && (mqqt_wifi_index < mqqt_wifi_list->Size())) {
		return (*mqqt_wifi_list)[mqqt_wifi_index];
	}
}

OLED_Display_Device_Driver *Module_Driver::Get_Selected_OLED_Display_Device() const
{
	if ((oled_display_index > -1) && (oled_display_index < oled_display_list->Size())) {
		return (*oled_display_list)[oled_display_index];
	}
}

Temperature_Device_Driver *Module_Driver::Get_Selected_Temperature_Device() const
{
	if ((temperature_index > -1) && (temperature_index < temperature_list->Size())) {
		return (*temperature_list)[temperature_index];
	}
}

Uart_GRBW_Led_Device_Driver *Module_Driver::Get_Selected_Uart_GRBW_Led_Device() const
{
	if ((Uart_GRBW_Led_index > -1) && (Uart_GRBW_Led_index < Uart_GRBW_Led_list->Size())) {
		return (*Uart_GRBW_Led_list)[Uart_GRBW_Led_index];
	}
}

WebServer_Wifi_Device_Driver *Module_Driver::Get_Selected_WebServer_Wifi_Device() const
{
	if ((webserver_wifi_index > -1) && (webserver_wifi_index < webserver_wifi_list->Size())) {
		return (*webserver_wifi_list)[webserver_wifi_index];
	}
}
