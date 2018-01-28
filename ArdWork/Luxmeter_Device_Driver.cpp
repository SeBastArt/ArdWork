// 
// 
// 

#include "Luxmeter_Device_Driver.h"

//#define DEBUG

Luxmeter_Device_Driver::Luxmeter_Device_Driver(Module_Driver* module, uint8_t adress, uint8_t priority) :
	Device_Driver(module, priority)
{
#ifdef  DEBUG
	Serial.print("Start Constructor Luxmeter_Device_Driver with ID: ");
	Serial.println(this->DriverId);
#endif //  DEBUG
	__DriverType = LUXMETER_DEVICE_DRIVER_TYPE;
	tsl = new Adafruit_TSL2561_Unified(adress, 1);
	Set_Enable_AutoRange(false);
#ifdef  DEBUG
	Serial.print("Ende Constructor Luxmeter_Device_Driver");
#endif //  DEBUG
}


void Luxmeter_Device_Driver::Build_Descriptor() {
#ifdef  DEBUG
	Serial.println("Start Luxmeter_Device_Driver::Build_Descriptor");
#endif //  DEBUG
	__descriptor->name = F("Luxmeter");
	__descriptor->descr = F("Luxmeter stellt die Steuerung des Lichtsensors bereit es erlaubt die Kontrolle &uuml;ber die Ausleseparameter und stellt Live - Werte sowie Diagramme bereit");
	__descriptor->published = false;

#ifdef  DEBUG
	Serial.println("Start Select_CtrlElem *ctrlElem_autorange");
#endif //  DEBUG
	Select_CtrlElem *ctrlElem_autorange = new Select_CtrlElem(LUXMETER_DEVICE_DRIVER_ENABLE_AUTORANGE, &sv_auto_range, F("Auto Range"), F("Autorange on or Off"));
	ctrlElem_autorange->AddMember("On");
	ctrlElem_autorange->AddMember("Off");
#ifdef  DEBUG
	Serial.println("Ende Select_CtrlElem *ctrlElem_autorange");
#endif //  DEBUG
	
	
#ifdef  DEBUG
	Serial.println("Start Select_CtrlElem *ctrlElem_acc_rate");
#endif //  DEBUG
	Select_CtrlElem *ctrlElem_acc_rate = new Select_CtrlElem(LUXMETER_DEVICE_DRIVER_SET_ACCURACY_DELAY, &sv_acc_rate, F("update rate"), F("select rate you want the value be updated"));
	ctrlElem_acc_rate->AddMember("1000ms");
	ctrlElem_acc_rate->AddMember("2000ms");
	ctrlElem_acc_rate->AddMember("5000ms");
	ctrlElem_acc_rate->AddMember("10000ms");
#ifdef  DEBUG
	Serial.println("Ende Select_CtrlElem *ctrlElem_acc_rate");
#endif //  DEBUG

#ifdef  DEBUG
	Serial.println("Start Select_CtrlElem *ctrlElem_integrationtime");
#endif //  DEBUG
	Select_CtrlElem *ctrlElem_integrationtime = new Select_CtrlElem(LUXMETER_DEVICE_DRIVER_SET_INTEGRATION_TIME, &sv_integrationTime, F("Integrationtime"), F("set the time the sensor collect light"));
	ctrlElem_integrationtime->published = false;
	ctrlElem_integrationtime->AddMember("13ms");
	ctrlElem_integrationtime->AddMember("101ms");
	ctrlElem_integrationtime->AddMember("402ms");
#ifdef  DEBUG
	Serial.println("Ende Select_CtrlElem *ctrlElem_integrationtime");
#endif //  DEBUG

#ifdef  DEBUG
	Serial.println("Start Select_CtrlElem *ctrlElem_gain");
#endif //  DEBUG
	Select_CtrlElem *ctrlElem_gain = new Select_CtrlElem(LUXMETER_DEVICE_DRIVER_SET_GAIN, &sv_gain, F("Gain"), F("select the Gain for make values better fit"));
	ctrlElem_gain->AddMember("Auto");
	ctrlElem_gain->AddMember("1X");
	ctrlElem_gain->AddMember("16X");
#ifdef  DEBUG
	Serial.println("Ende Select_CtrlElem *ctrlElem_gain");
#endif //  DEBUG

	Value_CtrlElem *ctrlElem_lux = new Value_CtrlElem(LUXMETER_DEVICE_DRIVER_LUX_VALUE, &lastLux, false, F("Lux-Value"), F("the value of the ambient light"));
	ctrlElem_lux->unit = F("Lux");

	__descriptor->Add_Descriptor_Element(ctrlElem_acc_rate);
	__descriptor->Add_Descriptor_Element(ctrlElem_integrationtime);
	__descriptor->Add_Descriptor_Element(ctrlElem_gain);
	__descriptor->Add_Descriptor_Element(ctrlElem_autorange);
	__descriptor->Add_Descriptor_Element(ctrlElem_lux);
#ifdef  DEBUG
	Serial.println("Ende Luxmeter_Device_Driver::Build_Descriptor");
#endif //  DEBUG
}

void Luxmeter_Device_Driver::DoInit()
{
#ifdef  DEBUG
	Serial.println("Start Luxmeter_Device_Driver::DoAfterInit");
#endif //  DEBUG
	Device_Driver::DoInit();
	if (!tsl->begin()) {
#ifdef  DEBUG
		Serial.println(F("------------------------------------"));
		Serial.println(F("Luxsensor not initialised"));
		Serial.println(F("maybe wrong adress of i2c"));
		Serial.println(F(""));
#endif //  DEBUG
	}
	else {
#ifdef  DEBUG
		DisplaySensorDetails();
		Serial.println(F("Luxmeter-Driver initialized!"));
		/* You can also manually set the gain or enable auto-gain support */
		// tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
		// tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
#endif //  DEBUG
		Set_Enable_AutoRange(true);
		tsl->enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

		Set_Integration_Time(TSL2561_INTEGRATIONTIME_13MS);
#ifdef  DEBUG
		/* Update these values depending on what you've set above! */
		Serial.println(F("------------------------------------"));
		Serial.print(F("Gain:         ")); Serial.println(F("Auto"));
		Serial.print(F("Timing:       ")); Serial.println(F("402 ms"));
		Serial.println(F("------------------------------------"));
		Serial.println(F(""));
#endif //  DEBUG
	}
	accuracy_delay = 1000;
	accuracy_delta = 0;
	lastLux = 0.0;
#ifdef  DEBUG
	Serial.println("Ende Luxmeter_Device_Driver::DoAfterInit");
#endif //  DEBUG
}

void Luxmeter_Device_Driver::DoBeforeShutdown() {
	//
}

void Luxmeter_Device_Driver::DoBeforeSuspend() {
	//
}

void Luxmeter_Device_Driver::DoDeviceMessage(Int_Task_Msg message) {
	int messageID = message.id;
	switch (messageID)
	{
	case LUXMETER_DEVICE_DRIVER_SET_ACCURACY_DELAY:
	{
#ifdef  DEBUG
		Serial.println("Start Luxmeter_Device_Driver::DoDeviceMessage - LUXMETER_DEVICE_DRIVER_SET_ACCURACY_DELAY");
#endif //  DEBUG
		uint16 _milliseconds = message.GetIntParamByIndex(0);
		Set_Accuracy_Delay(_milliseconds);
	}
	break;
	case LUXMETER_DEVICE_DRIVER_SET_INTEGRATION_TIME:
	{
#ifdef  DEBUG
		Serial.println("Start Luxmeter_Device_Driver::DoDeviceMessage - LUXMETER_DEVICE_DRIVER_SET_INTEGRATION_TIME");
#endif //  DEBUG
		uint16 _integrationTime = message.GetIntParamByIndex(0);
		Set_Integration_Time((tsl2561IntegrationTime_t)_integrationTime);
	}
	break;
	case LUXMETER_DEVICE_DRIVER_SET_GAIN:
	{
#ifdef  DEBUG
		Serial.println("Start Luxmeter_Device_Driver::DoDeviceMessage - LUXMETER_DEVICE_DRIVER_SET_GAIN");
#endif //  DEBUG
		uint16 _gain = message.GetIntParamByIndex(0);
		Set_Gain((tsl2561Gain_t)_gain);
	}
	break;
	case LUXMETER_DEVICE_DRIVER_ENABLE_AUTORANGE:
	{
#ifdef  DEBUG
		Serial.println("Start Luxmeter_Device_Driver::DoDeviceMessage - LUXMETER_DEVICE_DRIVER_ENABLE_AUTORANGE");
#endif //  DEBUG
		bool _autoRange = message.GetBoolParamByIndex(0);
		Set_Enable_AutoRange(_autoRange);
	}
	break;
	}
}

void Luxmeter_Device_Driver::DoUpdate(uint32_t deltaTime) {
#ifdef  DEBUG
	Serial.println("Start Luxmeter_Device_Driver::DoUpdate");
#endif //  DEBUG
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
				FloatMessage* message = new FloatMessage(DriverId, lastLux);
#ifdef  DEBUG
				Serial.println("Luxmeter_Device_Driver::DoUpdate SendAsyncTaskMessage");
#endif //  DEBUG
				if (!parentModule->SendAsyncTaskMessage(message))
				{
					Serial.println(F(">> message buffer overflow <<"));
				}
			}
		}
		else
		{
			/* If event.light = 0 lux the sensor is probably saturated
			and no reliable data could be generated! */
			Serial.println(F("Sensor overload"));
		}
	}
#ifdef  DEBUG
	Serial.println("Ende Luxmeter_Device_Driver::DoUpdate");
#endif //  DEBUG
}


void Luxmeter_Device_Driver::DisplaySensorDetails(void)
{
	sensor_t sensor;
	tsl->getSensor(&sensor);
	Serial.println(F("------------------------------------"));
	Serial.print(F("Sensor:       ")); Serial.println(sensor.name);
	Serial.print(F("Driver Ver:   ")); Serial.println(sensor.version);
	Serial.print(F("Unique ID:    ")); Serial.println(sensor.sensor_id);
	Serial.print(F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
	Serial.print(F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
	Serial.print(F("Resolution:   ")); Serial.print(sensor.resolution); Serial.println(F(" lux"));
	Serial.println(F("------------------------------------"));
	Serial.println(F(""));
}


void Luxmeter_Device_Driver::Set_Accuracy_Delay(uint16 _acc_rate) {
#ifdef  DEBUG
	Serial.println("Start Luxmeter_Device_Driver::Set_Accuracy_Delay");
#endif //  DEBUG
	//*sv_acc_rate = _acc_rate;
	switch (_acc_rate)
	{
	case 0:
		accuracy_delay = 1000;
		break;
	case 1:
		accuracy_delay = 2000;
		break;
	case 2:
		accuracy_delay = 5000;
		break;
	case 3:
		accuracy_delay = 10000;
		break;
	default:
		accuracy_delay = 1000;
		break;
	}
#ifdef  DEBUG
	Serial.println("Ende Luxmeter_Device_Driver::Set_Accuracy_Delay");
#endif //  DEBUG
}

void Luxmeter_Device_Driver::Set_Integration_Time(tsl2561IntegrationTime_t _integrationTime) {
#ifdef  DEBUG
	Serial.println("Start Luxmeter_Device_Driver::Set_Integration_Time");
#endif //  DEBUG
	//*sv_integrationTime = (uint8_t)_integrationTime;
	tsl->setIntegrationTime(_integrationTime);
	/* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
	// tsl->setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */
#ifdef  DEBUG
	Serial.println("Ende Luxmeter_Device_Driver::Set_Integration_Time");
#endif //  DEBUG
}

void Luxmeter_Device_Driver::Set_Gain(tsl2561Gain_t _gain) {
#ifdef  DEBUG
	Serial.println("Start Luxmeter_Device_Driver::Set_Gain");
#endif //  DEBUG
	//*sv_gain = _gain;
	tsl->setGain(_gain);
#ifdef  DEBUG
	Serial.println("Ende Luxmeter_Device_Driver::Set_Gain");
#endif //  DEBUG
}

void Luxmeter_Device_Driver::Set_Enable_AutoRange(bool _autoRange) {
#ifdef  DEBUG
	Serial.println("Start Luxmeter_Device_Driver::Set_Enable_AutoRange");
#endif //  DEBUG
	//int temp = _autoRange;
	//sv_auto_range = &temp;
	sv_auto_range = (int)_autoRange;
	tsl->enableAutoRange(_autoRange);
#ifdef  DEBUG
	Serial.println("Ende Luxmeter_Device_Driver::Set_Enable_AutoRange");
#endif //  DEBUG
}

void Luxmeter_Device_Driver::Exec_Set_Accuracy_Delay(uint16 _milliseconds) {
	Int_Task_Msg *message = new Int_Task_Msg(LUXMETER_DEVICE_DRIVER_SET_ACCURACY_DELAY);
	message->AddParam(_milliseconds);
	PostMessage(&message);
}

void Luxmeter_Device_Driver::Exec_Set_Integration_Time(tsl2561IntegrationTime_t _integrationTime) {
	Int_Task_Msg *message = new Int_Task_Msg(LUXMETER_DEVICE_DRIVER_SET_INTEGRATION_TIME);
	message->AddParam(_integrationTime);
	PostMessage(&message);
}

void Luxmeter_Device_Driver::Exec_Set_Gain(tsl2561Gain_t _gain) {
	Int_Task_Msg *message = new Int_Task_Msg(LUXMETER_DEVICE_DRIVER_SET_GAIN);
	message->AddParam(_gain);
	PostMessage(&message);
}

void Luxmeter_Device_Driver::Exec_Set_Enable_AutoRange(bool _autoRange) {
	Int_Task_Msg *message = new Int_Task_Msg(LUXMETER_DEVICE_DRIVER_ENABLE_AUTORANGE);
	message->AddParam(_autoRange);
	PostMessage(&message);
}