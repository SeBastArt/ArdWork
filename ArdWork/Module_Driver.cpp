// 
// 
// 

#include "Module_Driver.h"
#include "Device_Driver.h"

extern "C" {
#include "user_interface.h"
}

#include "Button_Device_Driver.h"
#include "Led_Device_Driver.h"
#include "Luxmeter_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver.h"
#include "WebSocket_Wifi_Device_Driver.h"
#include "Uart_RGB_Led_Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver.h"
#include "OLED_Display_Device_Driver.h"
#include "Distance_Device_Driver.h"
#include "Temperature_Device_Driver.h"

Module_Driver::Module_Driver(uint8_t priority) :
	Driver(priority) {
	isdebug = false;
	device_list = new Vector <Driver*>;
}

Module_Driver::~Module_Driver()
{
	__descriptor_list->Clear();
	device_list->Clear();
	queue.Clear();
}

void Module_Driver::Build_Descriptor() {
	__descriptor->name = F("Module");
	__descriptor->descr = F("the main module");
	__descriptor->published = true;

	Build_Module_Discriptor();

	Ctrl_Elem *ctrl_elem_debug = new Ctrl_Elem(MODULE_DRIVER_SET_DEBUG_MODE_EXTERN, F("Debug Mode"), select, F("Debug Mode On or Off"));
	ctrl_elem_debug->published = true;

	Atomic<String> *atomic_debug_off = new Atomic<String>(0, F("Off"));
	Atomic<String> *atomic_debug_on = new Atomic<String>(1, F("On"));
	ctrl_elem_debug->AddAtomic(atomic_debug_off);
	ctrl_elem_debug->AddAtomic(atomic_debug_on);

	__descriptor->Add_Descriptor_Element(ctrl_elem_debug);

}

void Module_Driver::AddDevice(Device_Driver* device)
{

	if ((device)->DriverType == MQQT_WIFI_DEVICE_DRIVER_TYPE) {
		AddObserver((Mqqt_Wifi_Device_Driver*)(device));
	}
	if ((device)->DriverType == WEBSOCKET_WIFI_DEVICE_DRIVER_TYPE) {
		AddObserver((WebSocket_Wifi_Device_Driver*)(device));
	}
	device_list->PushBack(device);
}

WebSocket_Wifi_Device_Driver *Module_Driver::Get_WebSocket_Wifi_DevDrv(uint8_t _index) {
	WebSocket_Wifi_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == WEBSOCKET_WIFI_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (WebSocket_Wifi_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}


Button_Device_Driver * Module_Driver::Get_Button_DevDrv(uint8_t _index)
{
	Button_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == BUTTON_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (Button_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}

Distance_Device_Driver * Module_Driver::Get_Distance_DevDrv(uint8_t _index)
{
	Distance_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == DISTANCE_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (Distance_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}


Led_Device_Driver * Module_Driver::Get_LED_DevDrv(uint8_t _index)
{
	Led_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == LED_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (Led_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}

Luxmeter_Device_Driver * Module_Driver::Get_Luxmeter_DevDrv(uint8_t _index)
{
	Luxmeter_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == LUXMETER_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (Luxmeter_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}

Mqqt_Wifi_Device_Driver * Module_Driver::Get_Mqqt_Wifi_DevDrv(uint8_t _index)
{
	Mqqt_Wifi_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == MQQT_WIFI_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (Mqqt_Wifi_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}

OLED_Display_Device_Driver * Module_Driver::Get_OLED_Display_DevDrv(uint8_t _index)
{
	OLED_Display_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == OLED_DISPLAY_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (OLED_Display_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}

Temperature_Device_Driver * Module_Driver::Get_Temperatur_DevDrv(uint8_t _index)
{
	Temperature_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == TEMPERATURE_DEVICE_DRIVER_TYPE) {
			match++;
			if (match == _index) {
				result = (Temperature_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}

Uart_RGB_Led_Device_Driver * Module_Driver::Get_Uart_RGB_Led_DevDrv(uint8_t _index)
{
	Uart_RGB_Led_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == UART_RGB_LED_DEVICE_TYPE) {
			match++;
			if (match == _index) {
				result = (Uart_RGB_Led_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}

Uart_GRBW_Led_Device_Driver *Module_Driver::Get_Uart_GRBW_Led_DevDrv(uint8_t _index) {
	Uart_GRBW_Led_Device_Driver *result = nullptr;
	uint8_t match = -1;
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverType == UART_GRBW_LED_DEVICE_TYPE) {
			match++;
			if (match == _index) {
				result = (Uart_GRBW_Led_Device_Driver *)(*device_list)[i];
			}
		}
	}
	return result;
}


Driver *Module_Driver::GetDeviceById(int Id)
{
	Driver* result = nullptr;

	if (DriverId == Id) {
		result = this;
	}
	else {
		for (int i = 0; i < device_list->Size(); i++) {
			if ((*device_list)[i]->DriverId == Id) {
				result = (*device_list)[i];
			}
		}
	}
	return result;
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

	int messageID = message.GetID();
	switch (messageID)
	{
	case MODULE_DRIVER_SET_DEBUG_MODE:
	{
		bool state = message.GetBoolParamByIndex(1);
		Set_Debug_Mode(state);
	}
	break;
	case MODULE_DRIVER_SET_DEBUG_MODE_EXTERN:
	{
		bool state = message.GetBoolParamByIndex(1);
		Set_Debug_Mode(state);
	}
	break;
	}

	DoModuleMessage(message);
}

void Module_Driver::DoInit() {
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecInit();
	}
	DoAfterInit();

}

void Module_Driver::DoShutdown() {
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecShutdown();
	}
}

void Module_Driver::DoSuspend() {
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecSuspend();
	}
}
Descriptor_List* Module_Driver::GetDescriptrorList()
{
	UpdateControls();
	return __descriptor_list;
}


void Module_Driver::DoUpdate(uint32_t deltaTime) {
	ThreadMessage *pMessage;
	NotifyObservers(GetDescriptrorList());
	if (PopMessage(&pMessage))
	{
		switch ((pMessage)->Class) {
		case MessageClass_Communication:
		{
			CommunicationMessage* pCommunication = (CommunicationMessage*)(pMessage);
			Driver* device = nullptr;
			device = GetDeviceById(pCommunication->__Id);
			if (device != nullptr) {
				device->Exec_Command(pCommunication->__CmdId, pCommunication->__Value);
			}
			break;
		}
		}
		DoThreadMessage(pMessage);

		if (pMessage != NULL) {
			delete pMessage; //objekt löschen 
			pMessage = NULL;
		}
	}
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
	__descriptor_list->Clear();
	if (isdebug) {
		for (int i = 0; i < device_list->Size(); i++) {
			__descriptor_list->Add_Descriptor((*device_list)[i]->GetDescriptor());
		}
	}
	__descriptor_list->Add_Descriptor(GetDescriptor());
}

void Module_Driver::Set_Debug_Mode(bool _state)
{
	isdebug = _state;
}

void Module_Driver::Exec_Set_Debug_Mode(bool _state)
{

	Int_Thread_Msg *message = new Int_Thread_Msg(MODULE_DRIVER_SET_DEBUG_MODE);
	message->AddParam(_state);
	PostMessage(&message);
}