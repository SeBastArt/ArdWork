#include "Device_Driver.h"

//#define DEBUG

Device_Driver::Device_Driver(Module_Driver* _module, uint8_t _priority) :
	Driver(_priority)
{
#ifdef  DEBUG
	Serial.print("Start Constructor Device_Driver with ID: ");
	Serial.println(this->DriverId);
#endif //  DEBUG
	parentModule = _module;
#ifdef  DEBUG
	Serial.print("Ende Constructor Device_Driver");
#endif //  DEBUG
}

void Device_Driver::DoMessage(Int_Task_Msg message) {
#ifdef  DEBUG
	Serial.print("Start Device_Driver::DoMessage");
#endif //  DEBUG

	int messageID = message.id;
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
#ifdef  DEBUG
	Serial.print("Ende Device_Driver::DoMessage");
#endif //  DEBUG
}

void Device_Driver::DoInit() {
#ifdef  DEBUG
	Serial.print("Start Device_Driver::DoInit with DriverID: ");
	Serial.println(this->DriverId);
#endif //  DEBUG

	//Anmelden an Observer

#ifdef  DEBUG
	Serial.println("Ende Device_Driver::DoInit");
#endif //  DEBUG
}

void Device_Driver::DoShutdown() {
	DoBeforeShutdown();
}

void Device_Driver::DoSuspend() {
	DoBeforeSuspend();
}



