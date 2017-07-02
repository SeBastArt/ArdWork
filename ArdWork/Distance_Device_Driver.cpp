// 
// 
// 
#include "Distance_Device_Driver.h"
#include "Module_Driver.h"

Distance_Device_Driver::Distance_Device_Driver(Module_Driver* module, uint8_t pin_trig, uint8_t pin_echo, uint8_t priority) :
	Device_Driver(module, priority),
	pin_t(pin_trig),
	pin_e(pin_echo)
{
	driver_name = "Distance_Device_Driver";

	publisher->name = "SuperSonic Distance";
	publisher->descr = "Distance to target";
	Value_Publisher *elem = new Value_Publisher("Distance", "Distance to Target", 0, "cm");
	publisher->Add_Publisher_Element(elem);
	publisher->published = true;
}


void Distance_Device_Driver::DoAfterInit() {
	Serial.println("Distance-Driver initialized!");
}

void Distance_Device_Driver::DoBeforeShutdown() {

}

void Distance_Device_Driver::DoBeforeSuspend() {

}

void Distance_Device_Driver::DoDeviceMessage(Int_Thread_Msg message) {
	int messageID = message.GetID();
	switch (messageID)
	{
		break;
	}
}

void Distance_Device_Driver::DoUpdate(uint32_t deltaTime) {

}

void Distance_Device_Driver::DoExecuteCommand(String _command)
{
}
