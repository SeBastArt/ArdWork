// 
// 
// 

#include "Module_Driver.h"
#include "Device_Driver.h"
#include "Creator.h"
#include "Button_Device_Driver.h"
//#define DEBUG

extern "C" {
#include "user_interface.h"
}

void Module_Driver::registerit(const std::string& classname, Creator* creator)
{
	Serial.print("register: ");
	Serial.println(classname.c_str());
	get_table()[classname.c_str()] = creator;
}

Device_Driver* Module_Driver::create(const std::string& classname)
{
	for (std::map<std::string, Creator*>::iterator it = get_table().begin(); it != get_table().end(); ++it) {
		if (String(it->first.c_str()).equals(classname.c_str()))
		{
			Device_Driver* temp = it->second->create(this);
			this->AddDevice(temp);
			return temp;
		}
	}
	Serial.println("Class not found");
	return (Device_Driver*)NULL;
}

std::map<std::string, Creator*>& Module_Driver::get_table()
{
	static std::map<std::string, Creator*> table;
	return table;
}


void Module_Driver::OnModuleUpdate(uint32_t deltaTime)
{
	//
}

Module_Driver::Module_Driver(uint8_t priority) :
	Driver(priority) {
	__isdebug = false;
	device_list = new Vector <Driver*>;
}

Module_Driver::~Module_Driver()
{
	device_list->Clear();
	queue.Clear();
}

void Module_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println("Start Module_Driver::Build_Descriptor");
#endif // DEBUG
	__descriptor->name = F("Module");
	__descriptor->descr = F("the main module");
	__descriptor->published = true;

	Build_Discriptor();

	Select_CtrlElem *ctrlElem_debug = new Select_CtrlElem(MODULE_DRIVER_SET_DEBUG_MODE, &__isdebug, F("Debug Mode"), F("Debug Mode On or Off - need a reload of HtmlPage"));
	ctrlElem_debug->AddMember("Off");
	ctrlElem_debug->AddMember("On");

	Group_CtrlElem *ctrlElem_save = new Group_CtrlElem(MODULE_DRIVER_SAVE, F("Save Config"), F("Save the configuration from all devices"));
	ctrlElem_save->AddMember("Save");

	__descriptor->Add_Descriptor_Element(ctrlElem_debug);
	__descriptor->Add_Descriptor_Element(ctrlElem_save);
#ifdef DEBUG
	Serial.println("Ende Module_Driver::Build_Descriptor");
#endif // DEBUG
}


bool Module_Driver::PopMessage(TaskMessage** message) {
	bool messagePopped = false;

	noInterrupts();
	TaskMessage* temp_message;
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

void Module_Driver::OnMessage(Int_Task_Msg message) {
	int messageID = message.id;
	switch (messageID)
	{
	case MODULE_DRIVER_SET_DEBUG_MODE:
	{
#ifdef DEBUG
		Serial.println("Start Module_Driver::DoMessage - MODULE_DRIVER_SET_DEBUG_MODE");
#endif // DEBUG
		bool state = message.GetBoolParamByIndex(0);
		__isdebug = (int)state;
		Set_Debug_Mode(state);
	}
	break;
	case MODULE_DRIVER_SAVE:
	{
#ifdef DEBUG
		Serial.println("Start Module_Driver::DoMessage- MODULE_DRIVER_SAVE");
#endif // DEBUG
		__descriptor_list->Save();
}
	break;
	}

	DoModuleMessage(message);
#ifdef DEBUG
	Serial.println("Ende Module_Driver::DoMessage");
#endif // DEBUG
}



void Module_Driver::OnStartup() {
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecStart();
	}
	ExecInit();
}

void Module_Driver::OnInit() {
#ifdef DEBUG
	Serial.println("Start Module_Driver::DoInit");
#endif // DEBUG
	pinManager.SetTaskContext(&taskManager);
#ifdef DEBUG
	Serial.println("Ende Module_Driver::DoInit");
#endif // DEBUG
}


void Module_Driver::OnShutdown() {
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecShutdown();
	}
}

void Module_Driver::OnSuspend() {
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecSuspend();
	}
}


void Module_Driver::OnStop() {
	for (int i = 0; i < device_list->Size(); i++) {
		(*device_list)[i]->ExecStop();
	}
}


void Module_Driver::OnNotifyStartupDone(int _DriverId) {
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverId == _DriverId)
			(*device_list)[i]->ExecInit();
	}
}

void Module_Driver::OnNotifyInitDone(int _DriverId) {
	for (int i = 0; i < device_list->Size(); i++) {
		if ((*device_list)[i]->DriverId == _DriverId)
			(*device_list)[i]->ExecLoadPresets();
	}
}


void Module_Driver::Set_Debug_Mode(bool _state)
{
#ifdef DEBUG
	Serial.println("Start Module_Driver::Set_Debug_Mode");
#endif // DEBUG
	if (_state) {
		for (uint8_t i = 0; i < __descriptor_list->count; i++)
			__descriptor_list->GetElemByIndex(i)->published = true;
	}
	else {
		for (uint8_t i = 0; i < device_list->Size(); i++) {
			Driver *device = (*device_list)[i];
			for (size_t j = 0; j < __descriptor_list->count; j++) {
				Descriptor *t_descr = __descriptor_list->GetElemByIndex(j);
				if (t_descr->id == device->DriverId) {
					t_descr->published = device->isPublished();
					break;
				}
			}
		}
	}
#ifdef DEBUG
	Serial.println("Ende Module_Driver::Set_Debug_Mode");
#endif // DEBUG
	}


void Module_Driver::DoUpdate(uint32_t deltaTime) {
	TaskMessage *pMessage;

	if (!isIdle()) {
		bool busyFlag = false;
		for (int i = 0; i < device_list->Size(); i++) {
			if ((*device_list)[i]->isBusy() || (*device_list)[i]->isInactive() || (!(*device_list)[i]->isIdle()))
				busyFlag = true;
		}
		if (busyFlag == false) {
			ExecLoadPresets();
		}
	}


	if (PopMessage(&pMessage))
	{
		switch ((pMessage)->Class) {
		case MessageClass_Extern:
		{
#ifdef DEBUG
			Serial.println("Start Module_Driver::DoUpdate MessageClass_Extern");
#endif // DEBUG
			ExternMessage* pExtern = (ExternMessage*)(pMessage);
			if (HasDeviceWithId(pExtern->__Id)) {
				Driver* device = GetDeviceById(pExtern->__Id);
				device->Exec_Command(pExtern->__CmdId, pExtern->__Value);
			}
#ifdef DEBUG
			Serial.println("Ende Module_Driver::DoUpdate MessageClass_Extern");
#endif // DEBUG
			break;
			}
		}
#ifdef DEBUG
		Serial.println("Fire Module_Driver::DoUpdate DoTaskMessage");
#endif // DEBUG
		DoTaskMessage(pMessage);

		if (pMessage != NULL) {
			delete pMessage; //objekt löschen 
			pMessage = NULL;
		}
		}
	OnModuleUpdate(deltaTime);
	}


bool Module_Driver::SendAsyncTaskMessage(TaskMessage* message, bool withinIsr) {
#ifdef DEBUG
	Serial.println("Start Module_Driver::SendAsyncTaskMessage");
#endif // DEBUG
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
#ifdef DEBUG
	Serial.println("Ende Module_Driver::SendAsyncTaskMessage");
#endif // DEBUG
	return result;
	}


void Module_Driver::Exec_Set_Debug_Mode(bool _state)
{
#ifdef DEBUG
	Serial.println("Start Module_Driver::Exec_Set_Debug_Mode");
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(MODULE_DRIVER_SET_DEBUG_MODE);
	message->AddParam(_state);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println("Ende Module_Driver::Exec_Set_Debug_Mode");
#endif // DEBUG
}

void Module_Driver::AddDevice(Device_Driver* device)
{
	device_list->PushBack(device);
}

bool Module_Driver::HasDeviceWithId(int _id)
{
#ifdef DEBUG
	Serial.println("Start Module_Driver::HasDeviceWithId");
#endif // DEBUG
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
#ifdef DEBUG
	Serial.println("Ende Module_Driver::HasDeviceWithId");
#endif // DEBUG
	return return_value;
	}


Driver *Module_Driver::GetDeviceById(int _id)
{
#ifdef DEBUG
	Serial.println("Start Module_Driver::GetDeviceById");
#endif // DEBUG
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
#ifdef DEBUG
	Serial.println("Ende Module_Driver::GetDeviceById");
#endif // DEBUG
	return result;
	}