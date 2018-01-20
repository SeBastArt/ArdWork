// 
// 
// 

#include "Module_Driver.h"
#include "Device_Driver.h"

//#define DEBUG

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
	device_list->Clear();
	queue.Clear();
}

void Module_Driver::Build_Descriptor() {
#ifdef  DEBUG
	Serial.println("Start Module_Driver::Build_Descriptor");
#endif //  DEBUG
	__descriptor->name = F("Module");
	__descriptor->descr = F("the main module");
	__descriptor->published = true;

	Build_Module_Discriptor();

	/*Ctrl_Elem *ctrl_elem_debug = new Ctrl_Elem(MODULE_DRIVER_SET_DEBUG_MODE, F("Debug Mode"), select, F("Debug Mode On or Off"));
	ctrl_elem_debug->published = true;

	Atomic<String> *atomic_debug_off = new Atomic<String>(0, F("Off"));
	Atomic<String> *atomic_debug_on = new Atomic<String>(1, F("On"));
	ctrl_elem_debug->AddAtomic(atomic_debug_off);
	ctrl_elem_debug->AddAtomic(atomic_debug_on);

	__descriptor->Add_Descriptor_Element(ctrl_elem_debug);*/
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::Build_Descriptor");
#endif //  DEBUG
}

void Module_Driver::AddDevice(Device_Driver* device)
{

	/*if ((device)->DriverType == MQQT_WIFI_DEVICE_DRIVER_TYPE) {
		AddObserver((Mqqt_Wifi_Device_Driver*)(device));
	}
	if ((device)->DriverType == WEBSOCKET_WIFI_DEVICE_DRIVER_TYPE) {
		AddObserver((WebSocket_Wifi_Device_Driver*)(device));
	}*/
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

bool Module_Driver::HasDeviceWithId(int _id)
{
#ifdef  DEBUG
	Serial.println("Start Module_Driver::HasDeviceWithId");
#endif //  DEBUG
	bool return_value = false;

	if (DriverId == _id) {
		return_value = true;
	}
	else {
		for (int i = 0; i < device_list->Size(); i++) {
			if ((*device_list)[i]->DriverId == _id) {
				return_value = true;
			}
		}
	}
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::HasDeviceWithId");
#endif //  DEBUG
	return return_value;
}


Driver *Module_Driver::GetDeviceById(int _id)
{
#ifdef  DEBUG
	Serial.println("Start Module_Driver::GetDeviceById");
#endif //  DEBUG
	Driver* result = nullptr;

	if (DriverId == _id) {
		result = this;
	}
	else {
		for (int i = 0; i < device_list->Size(); i++) {
			if ((*device_list)[i]->DriverId == _id) {
				result = (*device_list)[i];
			}
		}
	}
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::GetDeviceById");
#endif //  DEBUG
	return result;
}

bool Module_Driver::PopMessage(ThreadMessage** message) {
#ifdef  DEBUG
	Serial.println("Start Module_Driver::PopMessage");
#endif //  DEBUG
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
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::PopMessage");
#endif //  DEBUG
	return messagePopped;
}

void Module_Driver::DoMessage(Int_Thread_Msg message) {
#ifdef  DEBUG
	Serial.println("Start Module_Driver::DoMessage");
#endif //  DEBUG
	int messageID = message.id;
	switch (messageID)
	{
	case MODULE_DRIVER_SET_DEBUG_MODE:
	{
		bool state = message.GetBoolParamByIndex(0);
		Set_Debug_Mode(state);
	}
	break;
	}

	DoModuleMessage(message);
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::DoMessage");
#endif //  DEBUG
}

void Module_Driver::DoInit() {
#ifdef  DEBUG
	Serial.println("Start Module_Driver::DoInit");
#endif //  DEBUG
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecInit();
	}
	DoAfterInit();
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::DoInit");
#endif //  DEBUG
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


void Module_Driver::DoUpdate(uint32_t deltaTime) {
#ifdef  DEBUG
	Serial.println("Start Module_Driver::DoUpdate");
#endif //  DEBUG
	ThreadMessage *pMessage;
	
	if (PopMessage(&pMessage))
	{
		switch ((pMessage)->Class) {
		case MessageClass_Extern:
		{
#ifdef  DEBUG
			Serial.println("Start Module_Driver::DoUpdate MessageClass_Extern");
#endif //  DEBUG
			ExternMessage* pExtern = (ExternMessage*)(pMessage);
			if (HasDeviceWithId(pExtern->__Id)) {
				Driver* device = GetDeviceById(pExtern->__Id);
				device->Exec_Command(pExtern->__CmdId, pExtern->__Value);
			}
#ifdef  DEBUG
			Serial.println("Ende Module_Driver::DoUpdate MessageClass_Extern");
#endif //  DEBUG
			break;
		}
		}
#ifdef  DEBUG
		Serial.println("Fire Module_Driver::DoUpdate DoThreadMessage");
#endif //  DEBUG
		DoThreadMessage(pMessage);

		if (pMessage != NULL) {
			delete pMessage; //objekt löschen 
			pMessage = NULL;
		}
	}
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::DoUpdate");
#endif //  DEBUG
}


bool Module_Driver::SendAsyncThreadMessage(ThreadMessage* message, bool withinIsr) {
#ifdef  DEBUG
	Serial.println("Start Module_Driver::SendAsyncThreadMessage");
#endif //  DEBUG
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
#ifdef  DEBUG
	Serial.println("Ende Module_Driver::SendAsyncThreadMessage");
#endif //  DEBUG
	return result;
}

//void Module_Driver::UpdateControls()
//{
//	__descriptor_list->Clear();
//	for (int i = 0; i < device_list->Size(); i++) {
//		Descriptor *temp = (*device_list)[i]->GetDescriptor();
//		if (isdebug || temp->published) {
//			__descriptor_list->Add_Descriptor((*device_list)[i]->GetDescriptor());
//		}
//	}
//	__descriptor_list->Add_Descriptor(GetDescriptor());
//}

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