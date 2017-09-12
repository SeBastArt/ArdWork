// 
// 
// 
#include "Distance_Device_Driver.h"
#include "Module_Driver.h"

Distance_Device_Driver::Distance_Device_Driver(Module_Driver* module, uint8_t pin_trig, uint8_t pin_echo, uint8_t priority) :
	Device_Driver(module, priority),
	__pin_t(pin_trig),
	__pin_e(pin_echo)
{
	driver_name = "Distance_Device_Driver";
	__distance = 0.0;
}

void Distance_Device_Driver::Build_Descriptor() {
	__descriptor->name = "SuperSonic Distance";
	__descriptor->descr = "Distance to target";
	__descriptor->published = true;

	Ctrl_Elem *ctrl_elem = new Ctrl_Elem(0, "Distance", value, "measure the distance to a target via supersonic");
	ctrl_elem->published = true;

	Atomic<float> *atomic_distance = new Atomic<float>(0, &__distance, "cm");

	ctrl_elem->AddAtomic(atomic_distance);

	__descriptor->Add_Descriptor_Element(ctrl_elem);
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
