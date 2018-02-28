#include "Device_Driver.h"

//#define DEBUG

Device_Driver::Device_Driver(Module_Driver* _module, uint8_t _priority) :
	Driver(_priority)
{
#ifdef DEBUG
	Serial.print("Start Constructor Device_Driver with ID: ");
	Serial.println(this->DriverId);
#endif // DEBUG
	__parentModule = _module;
}

void Device_Driver::OnMessage(Int_Task_Msg message) {
#ifdef DEBUG
	Serial.print("Start Device_Driver::DoMessage");
#endif // DEBUG

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
#ifdef DEBUG
	Serial.print("Ende Device_Driver::DoMessage");
#endif // DEBUG
}


void Device_Driver::OnStartup() {
	AddObserver(__parentModule);
	NotifyModuleStartupDone(this->DriverId);
}


void Device_Driver::OnInit() {
#ifdef DEBUG
	Serial.print("Start Device_Driver::DoInit with DriverID: ");
	Serial.println(this->DriverId);
#endif // DEBUG
	NotifyModuleInitDone(this->DriverId);
#ifdef DEBUG
	Serial.println("Ende Device_Driver::DoInit");
#endif // DEBUG
}


void Device_Driver::OnBuild_Descriptor() {
	__descriptor->name = String("Driver #") +  String(DriverId);
	__descriptor->published = false;
}


void Device_Driver::OnShutdown() {

}

void Device_Driver::OnSuspend() {

}

void Device_Driver::OnStop() {

}




