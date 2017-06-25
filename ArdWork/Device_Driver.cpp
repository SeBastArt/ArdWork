#include "Device_Driver.h"

Device_Driver::Device_Driver(Module_Driver* module, uint8_t priority) :
	Driver(priority),
	parentModule(module)
{
	ctrl_List = new Vector<Control*>;
	ctrl_List->PushBack(new Control(device_count));
	__Id = device_count;
}

void Device_Driver::DoMessage(Int_Thread_Msg message) {

	int messageID = message.GetID();
	switch (messageID)
	{
	case DEVICE_DRIVER_EXECUTE_COMMAND:
	{
		String command = message.GetStringParamByIndex(1);
		Serial.println(command);
		DoExecuteCommand(command);
	}
	break;
	}

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

void Device_Driver::Exec_Command(String _command)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DEVICE_DRIVER_EXECUTE_COMMAND);
	message->AddParam(_command);
	PostMessage(&message);
}
