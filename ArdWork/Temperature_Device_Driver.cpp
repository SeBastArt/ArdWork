// 
// 
// 

#include "Temperature_Device_Driver.h"

Temperature_Device_Driver::Temperature_Device_Driver(Module_Driver* module, IO_Pin* _pin, bool isFahrenheit, uint8_t priority) :
	Device_Driver(module, priority),
	pin(_pin),
	fahrenheit(isFahrenheit)
{
	__DriverType = TEMPERATURE_DEVICE_DRIVER_TYPE;

	pin->SetPinState(LOW);
	dht = new DHT(pin->PinNumber(), DHT11);
	dht->begin();
};

void Temperature_Device_Driver::Build_Descriptor() {
	__descriptor->name = "Temperatur Device";
	__descriptor->descr = "Measure the temperatur of the environment";
	__descriptor->published = false;

	Value_CtrlElem *ctrlElem_temp = new Value_CtrlElem(TEMPERATURE_DEVICE_DRIVER_SI_TEMPERATURE, &act_temp, F("Temperatur"), F("actual temperature"));
	ctrlElem_temp->published = true;
	ctrlElem_temp->unit = F("°C");

	__descriptor->Add_Descriptor_Element(ctrlElem_temp);
}

void Temperature_Device_Driver::DoAfterInit()
{
	act_temp = 20.0;
	act_hum = 50.0;
	accuracy_delay = 2000;
	accuracy_delta = 0;
	Serial.println(F("Temperature-Driver initialized!"));
}

void Temperature_Device_Driver::DoBeforeShutdown() {
	//
}

void Temperature_Device_Driver::DoBeforeSuspend() {
	//
}

void Temperature_Device_Driver::DoDeviceMessage(Int_Thread_Msg message) {
	int messageID = message.id;
	switch (messageID)
	{
	case TEMPERATURE_DEVICE_DRIVER_SI_FAHRENHEIT:
	{
		Set_SI_To_Fahrenheit();
	}
	break;
	case TEMPERATURE_DEVICE_DRIVER_SI_CELSIUS:
	{
		Set_SI_To_Celsius();
	}
	break;
	case TEMPERATURE_DEVICE_DRIVER_SET_ACCURACY:
	{
		int _accuracy = message.GetIntParamByIndex(0);
		Set_Accuracy_Time_Ms(_accuracy);
	}
	break;
	}
}

void Temperature_Device_Driver::DoUpdate(uint32_t deltaTime) {
	
	accuracy_delta += deltaTime;
	if (accuracy_delta > accuracy_delay) {
		accuracy_delta = 0;
		float h = dht->readHumidity();
		if (!isnan(h)) {
			act_hum = h;
		}

		float t = dht->readTemperature();
		if (!isnan(t)) {
			act_temp = t;
		}
	}
}


void Temperature_Device_Driver::Set_SI_To_Fahrenheit() {
	fahrenheit = true;
}

void Temperature_Device_Driver::Set_SI_To_Celsius() {
	fahrenheit = false;
}

void Temperature_Device_Driver::Set_Accuracy_Time_Ms(uint16_t time) {
	accuracy_delay = time;
}


void Temperature_Device_Driver::Exec_Set_SI_To_Fahrenheit() {
	Int_Thread_Msg *message = new Int_Thread_Msg(TEMPERATURE_DEVICE_DRIVER_SI_FAHRENHEIT);
	PostMessage(&message);
}

void Temperature_Device_Driver::Exec_Set_SI_To_Celsius() {
	Int_Thread_Msg *message = new Int_Thread_Msg(TEMPERATURE_DEVICE_DRIVER_SI_CELSIUS);
	PostMessage(&message);
}

void Temperature_Device_Driver::Exec_Set_Accuracy_Time_Ms(uint16_t time) {
	Int_Thread_Msg *message = new Int_Thread_Msg(TEMPERATURE_DEVICE_DRIVER_SET_ACCURACY);
	message->AddParam(time);
	PostMessage(&message);
}


float Temperature_Device_Driver::Get_Temperature() {
	return act_temp;
}

float Temperature_Device_Driver::Get_Humidity() {
	return act_hum;
}





