#include "Device_Driver.h"

Device_Driver::Device_Driver(Module_Driver* module, uint8_t priority) :
	Driver(priority),
	parentModule(module)
{
	publisher = new Publisher(device_count);
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


Publisher *Device_Driver::GetPublisher()
{
	return publisher;
}

void Device_Driver::Exec_Command(uint32_t _cmdId, String _command)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(_cmdId);
	message->AddParam(_command);
	PostMessage(&message);
}
