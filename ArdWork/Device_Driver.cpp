#include "Device_Driver.h"

Device_Driver::Device_Driver(Module_Driver* _module, uint8_t _priority) :
	Driver(_priority)
{
	parentModule = _module;
}

void Device_Driver::DoMessage(Int_Thread_Msg message) {

	int messageID = message.GetID();
	switch (messageID)
	{
	case DEVICE_DRIVER_INIT:
	{
		//
	}
	break;
	case DEVICE_DRIVER_SHUTDOWN:
	{
		//
	}
	break;
	case DEVICE_DRIVER_SUSPEND:
	{
		//
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


