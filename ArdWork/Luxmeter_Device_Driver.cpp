// 
// 
// 

#include "Luxmeter_Device_Driver.h"

Luxmeter_Device_Driver::Luxmeter_Device_Driver(Module_Driver* module, uint8_t adress, uint8_t priority) :
	Device_Driver(module, priority)
{
	driver_name = "Luxmeter_Device_Driver";
	(*ctrl_List)[0]->Name = "Luxmeter";
	(*ctrl_List)[0]->Description = "Gibt die aktuelle Helligkeit an";
	(*ctrl_List)[0]->Style = Icon_Kind_Integer;
	(*ctrl_List)[0]->Data = "0 Lux";

	tsl = new Adafruit_TSL2561_Unified(adress, 1);
}

void Luxmeter_Device_Driver::DoAfterInit()
{
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

											  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
		tsl->setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
		// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
		// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

																   /* Update these values depending on what you've set above! */
		Serial.println("------------------------------------");
		Serial.print("Gain:         "); Serial.println("Auto");
		Serial.print("Timing:       "); Serial.println("13 ms");
		Serial.println("------------------------------------");
		Serial.println("");

	}
	accuracy_delay = 2000;
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
				(*ctrl_List)[0]->Data = String(lastLux) + "Lux" ;
				FloatMessage* message = new FloatMessage((*ctrl_List)[0]->Id, event.light);
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
