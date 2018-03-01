// 
// 
// 
#include "Distance_Device_Driver.h"
#include "Module_Driver.h"

REGISTERIMPL(Distance_Device_Driver);

Distance_Device_Driver::Distance_Device_Driver(Module_Driver* module, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = DISTANCE_DEVICE_DRIVER_TYPE;
	__threePins = __pin_t == __pin_e ? true : false;
	pinMode(__pin_t, OUTPUT);
	pinMode(__pin_e, INPUT);
	__timeout = 20000UL;
	__accuracy_delay = 2000;
	__accuracy_delta = 0;
	__distance = 0.0;
}


void Distance_Device_Driver::OnBuild_Descriptor() {
	__descriptor->name = F("Ultrasonic Distance");
	__descriptor->descr = F("estimate distance");
	__descriptor->published = true;

	/*Ctrl_Elem *ctrl_elem_dist = new Ctrl_Elem(DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE, F("Distance"), value, F("measure the distance to a target via ultrasonic"));
	ctrl_elem_dist->published = true;
	Atomic<float> *atomic_distance = new Atomic<float>(0, &__distance, CM_STRING);
	ctrl_elem_dist->AddAtomic(atomic_distance);


	Ctrl_Elem *ctrl_elem_unit = new Ctrl_Elem(DISTANCE_DEVICE_DRIVER_SET_UNIT, F("Unit"), select, F("select the unit for distance"));
	ctrl_elem_unit->published = true;
	Atomic<String> *atomic_cm = new Atomic<String>(dist_unit::cm, CM_STRING);
	Atomic<String> *atomic_inch = new Atomic<String>(dist_unit::inch, INCH_STRING);
	ctrl_elem_unit->AddAtomic(atomic_cm);
	ctrl_elem_unit->AddAtomic(atomic_inch);

	Ctrl_Elem *ctrl_elem_timeout = new Ctrl_Elem(DISTANCE_DEVICE_DRIVER_SET_TIMEOUT, F("Timeout"), edtvalue, F("timeout for define max distance and max integrationtime"));
	ctrl_elem_timeout->published = true;
	Atomic<unsigned long> *atomic_timeout = new Atomic<unsigned long>(0, &__timeout, F("ms"));
	ctrl_elem_timeout->AddAtomic(atomic_timeout);

	__descriptor->Add_Descriptor_Element(ctrl_elem_unit);
	__descriptor->Add_Descriptor_Element(ctrl_elem_timeout);
	__descriptor->Add_Descriptor_Element(ctrl_elem_dist);*/
}


void Distance_Device_Driver::OnInit() {
	Device_Driver::OnInit();
	Serial.println(F("Distance-Driver initialized!"));
}

void Distance_Device_Driver::DoDeviceMessage(Int_Task_Msg message) {
	int messageID = message.id;
	switch (messageID)
	{
	case DISTANCE_DEVICE_DRIVER_SET_UNIT:
	{
		uint16 unit = message.GetIntParamByIndex(0);
		Set_Unit((dist_unit)unit);
	}
	break;
	case DISTANCE_DEVICE_DRIVER_SET_TIMEOUT:
	{
		int timeout = message.GetIntParamByIndex(0);
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
		if (!__parentModule->SendAsyncTaskMessage(message))
		{
			Serial.println(F(">> message buffer overflow <<"));
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
		__descriptor->GetCtrlElemByIndex(DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE)->unit = CM_STRING;
	}
	else {
		__descriptor->GetCtrlElemByIndex(DISTANCE_DEVICE_DRIVER_EXTERN_DISTANCE)->unit = INCH_STRING;
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
	Int_Task_Msg *message = new Int_Task_Msg(DISTANCE_DEVICE_DRIVER_SET_UNIT);
	message->AddParam(_unit);
	PostMessage(&message);
}

void Distance_Device_Driver::Exec_Set_Timeout(int _timeout)
{
	Int_Task_Msg *message = new Int_Task_Msg(DISTANCE_DEVICE_DRIVER_SET_TIMEOUT);
	message->AddParam(_timeout);
	PostMessage(&message);
}

