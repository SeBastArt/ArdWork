// 
// 
// 

#include "Luxmeter_Device_Driver.h"

Luxmeter_Device_Driver::Luxmeter_Device_Driver(Module_Driver* module, uint8_t adress, uint8_t priority) :
	Device_Driver(module, priority)
{
	driver_name = "Luxmeter_Device_Driver";
	tsl = new Adafruit_TSL2561_Unified(adress, 1);
}


void Luxmeter_Device_Driver::Build_Descriptor() {
	__descriptor->name = "Luxmeter";
	__descriptor->descr = "Luxmeter stellt die Steuerung des Lichtsensors bereit \nes erlaubt die Kontrolle über die Ausleseparameter \nund stellt Live - Werte sowie Diagramme bereit";
	__descriptor->published = true;

	Ctrl_Elem *ctrl_elem_rate = new Ctrl_Elem(LUXMETER_DEVICE_DRIVER_FIRST_MESSAGE + 1, "update rate", select, "select rate you want the value be updated");
	ctrl_elem_rate->published = true;

	Atomic<int> *atomic_1000 = new Atomic<int>(0, 1000, "ms");
	Atomic<int> *atomic_2000 = new Atomic<int>(1, 2000, "ms");
	Atomic<int> *atomic_5000 = new Atomic<int>(2, 5000, "ms");
	Atomic<int> *atomic_10000 = new Atomic<int>(3, 10000, "ms");

	ctrl_elem_rate->AddAtomic(atomic_1000);
	ctrl_elem_rate->AddAtomic(atomic_2000);
	ctrl_elem_rate->AddAtomic(atomic_5000);
	ctrl_elem_rate->AddAtomic(atomic_10000);

	Ctrl_Elem *ctrl_elem_integr = new Ctrl_Elem(LUXMETER_DEVICE_DRIVER_FIRST_MESSAGE + 2, "Integrationtime", select, "set the time the sensor collect light");
	ctrl_elem_integr->published = true;

	Atomic<String> *atomic_13ms = new Atomic<String>(0, "13", "ms");
	Atomic<String> *atomic_101ms = new Atomic<String>(1, "101", "ms");
	Atomic<String> *atomic_402ms = new Atomic<String>(2, "402", "ms");

	ctrl_elem_integr->AddAtomic(atomic_13ms);
	ctrl_elem_integr->AddAtomic(atomic_101ms);
	ctrl_elem_integr->AddAtomic(atomic_402ms);

	Ctrl_Elem *ctrl_elem_gain = new Ctrl_Elem(LUXMETER_DEVICE_DRIVER_FIRST_MESSAGE + 3, "Gain", select, "select the Gain for make values better fit");
	ctrl_elem_gain->published = true;

	Atomic<String> *atomic_auto = new Atomic<String>(0, "Auto");
	Atomic<String> *atomic_1x = new Atomic<String>(1, "1X");
	Atomic<String> *atomic_16x = new Atomic<String>(2, "16X");

	ctrl_elem_gain->AddAtomic(atomic_auto);
	ctrl_elem_gain->AddAtomic(atomic_1x);
	ctrl_elem_gain->AddAtomic(atomic_16x);

	__descriptor->Add_Descriptor_Element(ctrl_elem_rate);
	__descriptor->Add_Descriptor_Element(ctrl_elem_integr);
	__descriptor->Add_Descriptor_Element(ctrl_elem_gain);
}

void Luxmeter_Device_Driver::DoAfterInit()
{
	Serial.println("Luxmeter_Device_Driver::DoAfterInit()");
	if (!tsl->begin()) {
		Serial.println("------------------------------------");
		Serial.println("Luxsensor not initialised");
		Serial.println("maybe wrong adress of i2c");
		Serial.println("");
	}
	else {
		DisplaySensorDetails();
		Serial.println("Luxmeter-Driver initialized!");
		/* You can also manually set the gain or enable auto-gain support */
		// tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
		// tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
		tsl->enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

		Set_Integration_Time(TSL2561_INTEGRATIONTIME_402MS);
		/* Update these values depending on what you've set above! */
		Serial.println("------------------------------------");
		Serial.print("Gain:         "); Serial.println("Auto");
		Serial.print("Timing:       "); Serial.println("402 ms");
		Serial.println("------------------------------------");
		Serial.println("");

	}
	accuracy_delay = 1000;
	accuracy_delta = 0;
	lastLux = 0.0;
}

void Luxmeter_Device_Driver::DoBeforeShutdown() {
	//
}

void Luxmeter_Device_Driver::DoBeforeSuspend() {
	//
}

void Luxmeter_Device_Driver::DoDeviceMessage(Int_Thread_Msg message) {
	int messageID = message.GetID();
	switch (messageID)
	{
	case LUXMETER_DEVICE_DRIVER_SET_ACCURACY_DELAY:
	{
		uint16 _milliseconds = message.GetIntParamByIndex(1);
		Set_Accuracy_Delay(_milliseconds);
	}
	break;
	case LUXMETER_DEVICE_DRIVER_SET_INTEGRATION_TIME:
	{
		uint16 _integrationTime = message.GetIntParamByIndex(1);
		Set_Integration_Time((tsl2561IntegrationTime_t)_integrationTime);
	}
	break;
	case LUXMETER_DEVICE_DRIVER_SET_GAIN:
	{
		uint16 _gain = message.GetIntParamByIndex(1);
		Set_Set_Gain((tsl2561Gain_t)_gain);
	}
	break;
	case LUXMETER_DEVICE_DRIVER_ENABLE_AUTORANGE:
	{
		uint16 _autoRange = message.GetBoolParamByIndex(1);
		Set_Set_Enable_AutoRange(_autoRange);
	}
	break;
	}
}

void Luxmeter_Device_Driver::DoUpdate(uint32_t deltaTime) {
	accuracy_delta += deltaTime;
	if (accuracy_delta > accuracy_delay) {
		accuracy_delta = 0;
		sensor_t sensor;
		tsl->getSensor(&sensor);
		sensors_event_t event;
		tsl->getEvent(&event);
		if (event.light) {
			if (fabs(event.light - lastLux) > EPSILON) {
				lastLux = event.light;
				FloatMessage* message = new FloatMessage(DriverId, event.light);
				if (!parentModule->SendAsyncThreadMessage(message))
				{
					Serial.println(">> message buffer overflow <<");
				}
			}
		}
		else
		{
			/* If event.light = 0 lux the sensor is probably saturated
			and no reliable data could be generated! */
			Serial.println("Sensor overload");
		}
	}
}


void Luxmeter_Device_Driver::DisplaySensorDetails(void)
{
	sensor_t sensor;
	tsl->getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.print("Sensor:       "); Serial.println(sensor.name);
	Serial.print("Driver Ver:   "); Serial.println(sensor.version);
	Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
	Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
	Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
	Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");
	Serial.println("------------------------------------");
	Serial.println("");
}


void Luxmeter_Device_Driver::Set_Accuracy_Delay(uint16 _milliseconds) {
	if (_milliseconds > 0) {
		accuracy_delta = _milliseconds;
	}
}

void Luxmeter_Device_Driver::Set_Integration_Time(tsl2561IntegrationTime_t _integrationTime) {
	tsl->setIntegrationTime(_integrationTime);
	/* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
	// tsl->setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */
}

void Luxmeter_Device_Driver::Set_Set_Gain(tsl2561Gain_t _gain) {
	tsl->setGain(_gain);
}

void Luxmeter_Device_Driver::Set_Set_Enable_AutoRange(bool _autoRange) {
	tsl->enableAutoRange(_autoRange);
}

void Luxmeter_Device_Driver::Exec_Set_Accuracy_Delay(uint16 _milliseconds) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LUXMETER_DEVICE_DRIVER_SET_ACCURACY_DELAY);
	message->AddParam(_milliseconds);
	PostMessage(&message);
}

void Luxmeter_Device_Driver::Exec_Set_Integration_Time(tsl2561IntegrationTime_t _integrationTime) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LUXMETER_DEVICE_DRIVER_SET_INTEGRATION_TIME);
	message->AddParam(_integrationTime);
	PostMessage(&message);
}

void Luxmeter_Device_Driver::Exec_Set_Gain(tsl2561Gain_t _gain) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LUXMETER_DEVICE_DRIVER_SET_GAIN);
	message->AddParam(_gain);
	PostMessage(&message);
}

void Luxmeter_Device_Driver::Exec_Set_Enable_AutoRange(bool _autoRange) {
	Int_Thread_Msg *message = new Int_Thread_Msg(LUXMETER_DEVICE_DRIVER_ENABLE_AUTORANGE);
	message->AddParam(_autoRange);
	PostMessage(&message);
}
