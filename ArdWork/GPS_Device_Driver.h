#ifndef _GPS_DEVICE_DRIVER_h
#define _GPS_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include <Time.h> //https://github.com/PaulStoffregen/Time
#include "Timezone.h" //https://github.com/JChristensen/Timezone
#include "Device_Driver.h"
#include "GPS_Device_Driver_Consts.h"
#include "TinyGPS++.h"
#include <SoftwareSerial.h>

class GPS_Device_Driver : public Device_Driver
{
	REGISTER(GPS_Device_Driver);
private:
	bool __time_is_set;
	bool __active;
	SoftwareSerial *SerialGPS;
	TinyGPSPlus *gps;
	time_t __local_time;
	time_t __utc_time;

	uint __local_timer_delta;
	uint __local_timer_delay;
protected:
	void DoUpdate(uint32_t deltaTime);
	void displayInfo();
	void OnInit() override;
	void TimerTick() override;
	void DoDeviceMessage(Int_Task_Msg message);
	void OnBuild_Descriptor();
public:
	GPS_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);

};

#endif