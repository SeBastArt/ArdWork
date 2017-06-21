#include "Device_Driver.h"

Device_Driver::Device_Driver(Module_Driver* module, uint8_t priority) :
	Driver(priority),
	parentModule(module)
{
	ctrl_List = new Vector<Control*>;
	ctrl_List->PushBack(new Control(device_count));
}

void Device_Driver::DoMessage(Int_Thread_Msg message) {
	DoDeviceMessage(message);
}

void Device_Driver::DoInit() {
	DoAfterInit();
}

void Device_Driver::DoShutdown() {
	DoBeforeShutdown();
}

void Device_Driver::DoSuspend() {
	DoBeforeSuspend();
}

Vector<Control*> *Device_Driver::GetControls()
{
	return ctrl_List;
}
