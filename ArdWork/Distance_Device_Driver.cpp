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
	__threePins = __pin_t == __pin_e ? true : false;
	pinMode(__pin_t, OUTPUT);
	pinMode(__pin_e, INPUT);
	__timeout = 20000UL;
	__accuracy_delay = 2000;
	__accuracy_delta = 0;
	__distance = 0.0;
}


void Distance_Device_Driver::Build_Descriptor() {
	__descriptor->name = "Ultrasonic Distance";
	__descriptor->descr = "estimate the distance to a target";
	__descriptor->published = true;

	Ctrl_Elem *ctrl_elem_dist = new Ctrl_Elem(DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE, "Distance", value, "measure the distance to a target via ultrasonic");
	ctrl_elem_dist->published = true;
	Atomic<float> *atomic_distance = new Atomic<float>(0, &__distance, CM_STRING);
	ctrl_elem_dist->AddAtomic(atomic_distance);


	Ctrl_Elem *ctrl_elem_unit = new Ctrl_Elem(DISTANCE_DEVICE_DRIVER_SET_UNIT, "Unit", select, "select the unit for distance");
	ctrl_elem_unit->published = true;
	Atomic<String> *atomic_cm = new Atomic<String>(dist_unit::cm, CM_STRING);
	Atomic<String> *atomic_inch = new Atomic<String>(dist_unit::inch, INCH_STRING);
	ctrl_elem_unit->AddAtomic(atomic_cm);
	ctrl_elem_unit->AddAtomic(atomic_inch);

	Ctrl_Elem *ctrl_elem_timeout = new Ctrl_Elem(DISTANCE_DEVICE_DRIVER_SET_TIMEOUT, "Timeout", edtvalue, "timeout for define max distance and max integrationtime");
	ctrl_elem_timeout->published = true;
	Atomic<unsigned long> *atomic_timeout = new Atomic<unsigned long>(0, &__timeout, "ms");
	ctrl_elem_timeout->AddAtomic(atomic_timeout);

	__descriptor->Add_Descriptor_Element(ctrl_elem_unit);
	__descriptor->Add_Descriptor_Element(ctrl_elem_timeout);
	__descriptor->Add_Descriptor_Element(ctrl_elem_dist);
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
	case DISTANCE_DEVICE_DRIVER_SET_UNIT:
	{
		uint16 unit = message.GetIntParamByIndex(1);
		Set_Unit((dist_unit)unit);
	}
	break;
	case DISTANCE_DEVICE_DRIVER_SET_TIMEOUT:
	{
		int timeout = message.GetIntParamByIndex(1);
		Set_Timeout(timeout);
	}
	break;
	}	
}

void Distance_Device_Driver::DoUpdate(uint32_t deltaTime) {
	__accuracy_delta += deltaTime;
	if (__accuracy_delta > __accuracy_delay) {
		__accuracy_delta = 0;
		__distance = 0.0;
		if (__unit == dist_unit::cm) {
			__distance = DistanceRead_cm();
		}
		else {
			__distance = DistanceRead_inch();
		}
		FloatMessage* message = new FloatMessage(DriverId, __distance);
		if (!parentModule->SendAsyncThreadMessage(message))
		{
			Serial.println(">> message buffer overflow <<");
		}
	}
}

float Distance_Device_Driver::DistanceRead_cm() {
	return round(Timing() / 28 / 2 * 10) / 10;
}

float Distance_Device_Driver::DistanceRead_inch() {
	return round(Timing() / 71 / 2 * 10) / 10;
}

void Distance_Device_Driver::Set_Unit(dist_unit _unit)
{
	__unit = _unit;
	if (__unit == dist_unit::cm) {
		__descriptor->GetCtrlElemByIndex(DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE)->GetAtomicByIndex(0)->unit = CM_STRING;
	}
	else {
		__descriptor->GetCtrlElemByIndex(DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE)->GetAtomicByIndex(0)->unit = INCH_STRING;
	}
}

void Distance_Device_Driver::Set_Timeout(int _timeout)
{
	__timeout = _timeout;
}


float Distance_Device_Driver::Timing() {
	if (__threePins)
		pinMode(__pin_t, OUTPUT);

	digitalWrite(__pin_t, LOW);
	delayMicroseconds(2);
	digitalWrite(__pin_t, HIGH);
	delayMicroseconds(10);
	digitalWrite(__pin_t, LOW);

	if (__threePins)
		pinMode(__pin_t, INPUT);

	return pulseIn(__pin_e, HIGH, __timeout); // duration
}


void Distance_Device_Driver::Exec_Set_Unit(dist_unit _unit)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISTANCE_DEVICE_DRIVER_SET_UNIT);
	message->AddParam(_unit);
	PostMessage(&message);
}

void Distance_Device_Driver::Exec_Set_Timeout(int _timeout)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(DISTANCE_DEVICE_DRIVER_SET_TIMEOUT);
	message->AddParam(_timeout);
	PostMessage(&message);
}