#include "Time_Device_Driver.h"
#include "Timezones.h"

// NTP specifics
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
WiFiUDP udp;  // A UDP instance to let us send and receive packets over UDP
unsigned int ntpPort = 2390;          // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48;       // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets

REGISTERIMPL(Time_Device_Driver);

bool Time_Device_Driver::__time_is_set;

//#define DEBUG

Time_Device_Driver::Time_Device_Driver(Module_Driver * module, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = TIME_DEVICE_DRIVER_TYPE;
	__local_time = 0;
	__utc_time = 0;
	__ntp_counter = 0;
	__local_timer = 0;
	__GPSactive = false;
	__time_is_set = false;
	__GPSactive = false;
	__hasGPSModule = false;
	__rx = nullptr;
	__tx = nullptr;
	SetTimerDelay(500);
}


void Time_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::Build_Descriptor"));
#endif // DEBUG
	__descriptor->name = (F("Time"));
	__descriptor->descr = (F("Sync Time"));
	__descriptor->published = true;

	Time_CtrlElem *ctrlElem_utc_time = new Time_CtrlElem(TIME_DEVICE_DRIVER_UTC_TIME, &__utc_time, false, F("UTC Time"), F("world time"));

	Time_CtrlElem *ctrlElem_local_time = new Time_CtrlElem(TIME_DEVICE_DRIVER_LOCAL_TIME, &__local_time, false, F("Local Time"), F("The Time in your Location"));

	Select_CtrlElem *ctrlElem_timeZone = new Select_CtrlElem(TIME_DEVICE_DRIVER_TIMEZONE, &__sv_timezone, F("Timezone"), F("Select Timezone"));
	for (uint8_t i = 0; i < sizeof(timezone_Arr) / sizeof(timezone_struct); ++i)
		ctrlElem_timeZone->AddMember(timezone_Arr[i]._name);

	Select_CtrlElem *ctrlElem_time_source = new Select_CtrlElem(TIME_DEVICE_DRIVER_TIME_SOURCE, &__sv_time_source, F("Time Source"), F("Select source time"));
	ctrlElem_time_source->AddMember(F("NTP-Server"));
	if (__hasGPSModule)
		ctrlElem_time_source->AddMember(F("GPS-Time"));

	Group_CtrlElem *ctrlElem_SetTime = new Group_CtrlElem(TIME_DEVICE_DRIVER_GET_TIME, F("Get Timestamp"), F("Get a new timestamp"));
	ctrlElem_SetTime->AddMember(F("Update Time"));

	__descriptor->Add_Descriptor_Element(ctrlElem_timeZone);
	__descriptor->Add_Descriptor_Element(ctrlElem_time_source);
	__descriptor->Add_Descriptor_Element(ctrlElem_SetTime);
	__descriptor->Add_Descriptor_Element(ctrlElem_utc_time);
	__descriptor->Add_Descriptor_Element(ctrlElem_local_time);
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::Build_Descriptor"));
#endif // DEBUG
}


void Time_Device_Driver::SetGPSModule(IO_Pin * _rx, IO_Pin * _tx) {
	__rx = _rx;
	__tx = _tx;
	__hasGPSModule = true;
}


void Time_Device_Driver::OnInit()
{
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::OnInit"));
#endif // DEBUG
	Device_Driver::OnInit();
	//StartGetTime();
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::OnInit"));
#endif // DEBUG
}

void Time_Device_Driver::OnNotifyOnline()
{
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::OnNotifyOnline"));
#endif // DEBUG
	StartGetNTPTime();
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::OnNotifyOnline"));
#endif // DEBUG
}

void Time_Device_Driver::OnNotifyConnected() {
#ifdef DEBUG
	Serial.println(F("Start Ntp_Wifi_Device_Driver::OnNotifyConnected"));
#endif // DEBUG
	//
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::OnNotifyConnected"));
#endif // DEBUG
}

time_t Time_Device_Driver::GetLocalTime() const {
	time_t utc_time;
	time_t local_time;
	utc_time = now();
	local_time = ((Timezone)timezone_Arr[__sv_timezone]._timezone).toLocal(utc_time);
	return local_time;
}

time_t Time_Device_Driver::GetUtcTime() const {
	time_t utc_time;
	utc_time = now();
	return utc_time;
}

void Time_Device_Driver::OnNotifyConnectionLost() {
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::OnNotifyConnectionLost"));
#endif // DEBUG
	//
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::OnNotifyConnectionLost"));
#endif // DEBUG
}

void Time_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case TIME_DEVICE_DRIVER_UTC_TIME:
	{
#ifdef DEBUG
		Serial.println(F("Start Time_Device_Driver::DoDeviceMessage - TIME_DEVICE_DRIVER_UTC_TIME"));
#endif // DEBUG
		//
	}
	break;
	case TIME_DEVICE_DRIVER_LOCAL_TIME:
	{
#ifdef DEBUG
		Serial.println(F("Start Time_Device_Driver::DoDeviceMessage - TIME_DEVICE_DRIVER_LOCAL_TIME"));
#endif // DEBUG
		//
	}
	break;
	case TIME_DEVICE_DRIVER_TIMEZONE:
	{
#ifdef DEBUG
		Serial.println(F("Start Time_Device_Driver::DoDeviceMessage - TIME_DEVICE_DRIVER_TIMEZONE"));
#endif // DEBUG
		int timezone = message.GetIntParamByIndex(0);
		SetTimezone(timezone);
	}
	break;
	case TIME_DEVICE_DRIVER_TIME_SOURCE:
	{
#ifdef DEBUG
		Serial.println(F("Start Nixie_Module_Driver::DoModuleMessage - NIXIE_MODULE_DRIVER_PATTERN_TIME_SOURCE"));
#endif // DEBUG
		int number = message.GetIntParamByIndex(0);
		SetTimeSource(number);
	}
	break;
	case TIME_DEVICE_DRIVER_GET_TIME:
	{
#ifdef DEBUG
		Serial.println(F("Start Time_Device_Driver::DoDeviceMessage - TIME_DEVICE_DRIVER_GET_TIME"));
#endif // DEBUG
		StartGetTime();
	}
	break;
	}
}


void Time_Device_Driver::StartGetTime() {
#ifdef DEBUG
	Serial.println(F("Start Ntp_Wifi_Device_Driver::StartGetTime"));
#endif // DEBUG
	setTime(0);
	switch (__sv_time_source)
	{
	case 0:
	{
		StartGetNTPTime();
	}
	break;
	case 1:
	{
		if (!__hasGPSModule)
			break;
		StartGetGPSTime();
	}
	break;
	}
#ifdef DEBUG
	Serial.println(F("Start Ntp_Wifi_Device_Driver::StartGetTime"));
#endif // DEBUG
}

void Time_Device_Driver::SetTimezone(int _timezone) {
	__sv_timezone = _timezone;
}


void Time_Device_Driver::SetTimeSource(int _number) {
#ifdef DEBUG
	Serial.println(F("Start Nixie_Module_Driver::SetTimeSource"));
#endif // DEBUG
	__sv_time_source = _number;
	StartGetTime();
#ifdef DEBUG
	Serial.println(F("Ende Nixie_Module_Driver::SetTimeSource"));
#endif // DEBUG
}

void Time_Device_Driver::StartGetNTPTime() {
#ifdef DEBUG
	Serial.println(F("Start Ntp_Wifi_Device_Driver::StartGetTime"));
#endif // DEBUG
	if (__isOnline) {
		__ntp_counter = 10;
		__time_is_set = false;
	}
#ifdef DEBUG
	Serial.println(F("Ende Ntp_Wifi_Device_Driver::StartGetTime"));
#endif // DEBUG
}


void Time_Device_Driver::StartGetGPSTime() {
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::StartGetTime"));
#endif // DEBUG
	if (!__hasGPSModule)
		return;

	EndGPS();
	SerialGPS = new SoftwareSerial(__rx->pinGPIO, __tx->pinGPIO);
	gps = new TinyGPSPlus();
	SerialGPS->begin(9600);
	__GPSactive = true;
	__time_is_set = false;
	__local_timer = 10000;  // try it for 10 seconds
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::StartGetTime"));
#endif // DEBUG
}


void Time_Device_Driver::EndGPS() {
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::EndGPS"));
#endif // DEBUG
	if (SerialGPS != nullptr) {
		SerialGPS->end();
		delete SerialGPS;
		SerialGPS = nullptr;
}
	if (gps != nullptr) {
		delete gps;
		gps = nullptr;
	}
	__GPSactive = false;
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::EndGPS"));
#endif // DEBUG
}

void Time_Device_Driver::DoUpdate(uint32_t deltaTime) {
	if (!__isIdle)
		return;

	if (__time_is_set && __GPSactive) {
		EndGPS();
	}

	if (__local_timer <= 0) {
		__time_is_set = true;
		return;
	}
	if (!__GPSactive)
		return;
	if (!__time_is_set) {
		__local_timer -= deltaTime;
		while (SerialGPS->available() > 0) {
			if (gps->encode(SerialGPS->read())) { // process gps messages
				setTime(gps->time.hour(), gps->time.minute(), gps->time.second(), gps->date.day(), gps->date.month(), gps->date.year());
				__time_is_set = true;
			}
		}
	}
}


void Time_Device_Driver::TimerTick()
{
	if ((__isOnline) && (!__time_is_set) && (__ntp_counter > 0)) {
		SyncTimeWithNTP();
	}
	__ntp_counter--;
	__utc_time = now();
	__local_time = ((Timezone)timezone_Arr[__sv_timezone]._timezone).toLocal(__utc_time);
}



void Time_Device_Driver::Exec_Start_Get_Time() {
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::Exec_Start_Get_Time"));
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(TIME_DEVICE_DRIVER_GET_TIME);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::Exec_Start_Get_Time"));
#endif // DEBUG
}

void Time_Device_Driver::Exec_Set_Timezone(int _timezone) {
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::Exec_Set_Timezone"));
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(TIME_DEVICE_DRIVER_TIMEZONE);
	message->AddParam(_timezone);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::Exec_Set_Timezone"));
#endif // DEBUG
}


void Time_Device_Driver::Exec_Set_Timesource(int _timesource) {
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::Exec_Set_Timezone"));
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(TIME_DEVICE_DRIVER_TIME_SOURCE);
	message->AddParam(_timesource);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::Exec_Set_Timezone"));
#endif // DEBUG
}


void Time_Device_Driver::SyncTimeWithNTP()
{
#ifdef DEBUG
	Serial.println(F("Start Ntp_Wifi_Device_Driver::SyncTimeWithNTP"));
#endif // DEBUG
	setSyncProvider(getNTP_UTCTime1970);
	setSyncInterval(86400); // NTP re-sync; i.e. 86400 sec would be once per day
	yield();
#ifdef DEBUG
	Serial.println(F("Ende Ntp_Wifi_Device_Driver::SyncTimeWithNTP"));
#endif // DEBUG
}

time_t Time_Device_Driver::getNTP_UTCTime1970()
{
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::getNTP_UTCTime1970"));
#endif // DEBUG
	__time_is_set = false;
	unsigned long t = getNTPTimestamp();
	if (t == 0) {

		return(0);
	}
	// scale to 1970 
	// may look like back & forth with ntp code; wrote it to make needed conversions more clear
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::getNTP_UTCTime1970"));
#endif // DEBUG
	__time_is_set = true;
	return(t + 946684800UL);
}

unsigned long Time_Device_Driver::getNTPTimestamp()
{
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::getNTPTimestamp"));
#endif // DEBUG
	unsigned long ulSecs2000;

	udp.begin(ntpPort);
	sendNTPpacket(timeServer); // send an NTP packet to a time server
	delay(1000);    // wait to see if a reply is available
	int cb = 0, repeat = 0;
	while (!cb && repeat < 20)  // try for 2 sec
	{
		cb = udp.parsePacket();
		delay(100);
		repeat++;
	}

	if (!cb)
	{
		ulSecs2000 = 0;
	}
	else
	{
		udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

												 //the timestamp starts at byte 40 of the received packet and is four bytes,
												 // or two words, long. First, esxtract the two words:
		unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
		unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

		// combine the four bytes (two words) into a long integer
		// this is NTP time (seconds since Jan 1 1900):
		ulSecs2000 = highWord << 16 | lowWord;
		ulSecs2000 -= 2208988800UL; // go from 1900 to 1970
		ulSecs2000 -= 946684800UL; // go from 1970 to 2000
	}
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::getNTPTimestamp"));
#endif // DEBUG
	return(ulSecs2000);

}

// send an NTP request to the time server at the given address
unsigned long Time_Device_Driver::sendNTPpacket(IPAddress& address)
{
#ifdef DEBUG
	Serial.println(F("Start Time_Device_Driver::sendNTPpacket"));
#endif // DEBUG
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision

							 // 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	udp.beginPacket(address, 123); //NTP requests are to port 123
	udp.write(packetBuffer, NTP_PACKET_SIZE);
	udp.endPacket();
#ifdef DEBUG
	Serial.println(F("Ende Time_Device_Driver::sendNTPpacket"));
#endif // DEBUG
}
