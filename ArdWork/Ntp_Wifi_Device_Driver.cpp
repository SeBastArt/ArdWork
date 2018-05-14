#include "Ntp_Wifi_Device_Driver.h"
#include "Timezones.h"

//#define DEBUG;

// NTP specifics
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
WiFiUDP udp;  // A UDP instance to let us send and receive packets over UDP
unsigned int ntpPort = 2390;          // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48;       // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets

REGISTERIMPL(Ntp_Wifi_Device_Driver);

bool Ntp_Wifi_Device_Driver::__time_is_set = false;


Ntp_Wifi_Device_Driver::Ntp_Wifi_Device_Driver(Module_Driver * module, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = NTP_WIFI_DEVICE_DRIVER_TYPE;
	__local_time = 0;
	__utc_time = 0;
	__local_counter = 0;
	SetTimerDelay(500);
}


void Ntp_Wifi_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::Build_Descriptor");
#endif // DEBUG
	__descriptor->name = (F("NTP-Driver"));
	__descriptor->descr = (F("Sync with ntp-server for real time"));
	__descriptor->published = true;

	Time_CtrlElem *ctrlElem_utc_time = new Time_CtrlElem(NTP_WIFI_DEVICE_DRIVER_UTC_TIME, &__utc_time, false, F("UTC Time"), F("world time"));

	Time_CtrlElem *ctrlElem_local_time = new Time_CtrlElem(NTP_WIFI_DEVICE_DRIVER_LOCAL_TIME, &__local_time, false, F("Local Time"), F("The Time in your Location"));

	Select_CtrlElem *ctrlElem_timeZone = new Select_CtrlElem(NTP_WIFI_DEVICE_DRIVER_TIMEZONE, &__sv_timezone, F("Timezone"), F("Select the Timezone you want to display"));
	for (uint8_t i = 0; i < sizeof(timezone_Arr) / sizeof(timezone_struct); ++i)
		ctrlElem_timeZone->AddMember(timezone_Arr[i]._name);

	Group_CtrlElem *ctrlElem_SetTime = new Group_CtrlElem(NTP_WIFI_DEVICE_DRIVER_GET_TIME, F("Get new Timestamp"), F("Get a new timestamp from NTP-Server"));
	ctrlElem_SetTime->AddMember(F("Update Time"));


	__descriptor->Add_Descriptor_Element(ctrlElem_timeZone);
	__descriptor->Add_Descriptor_Element(ctrlElem_SetTime);
	__descriptor->Add_Descriptor_Element(ctrlElem_utc_time);
	__descriptor->Add_Descriptor_Element(ctrlElem_local_time);
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::Build_Descriptor");
#endif // DEBUG
}


void Ntp_Wifi_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case NTP_WIFI_DEVICE_DRIVER_UTC_TIME:
	{
#ifdef DEBUG
		Serial.println("Start Ntp_Wifi_Device_Driver::DoDeviceMessage - NTP_WIFI_DEVICE_DRIVER_UTC_TIME");
#endif // DEBUG
//
	}
	break;
	case NTP_WIFI_DEVICE_DRIVER_LOCAL_TIME:
	{
#ifdef DEBUG
		Serial.println("Start Ntp_Wifi_Device_Driver::DoDeviceMessage - NTP_WIFI_DEVICE_DRIVER_LOCAL_TIME");
#endif // DEBUG
//
	}
	break;
	case NTP_WIFI_DEVICE_DRIVER_TIMEZONE:
	{
#ifdef DEBUG
		Serial.println("Start Ntp_Wifi_Device_Driver::DoDeviceMessage - NTP_WIFI_DEVICE_DRIVER_TIMEZONE");
#endif // DEBUG
		int timezone = message.GetIntParamByIndex(0);
		SetTimezone(timezone);
	}
	break;
	case NTP_WIFI_DEVICE_DRIVER_GET_TIME:
	{
#ifdef DEBUG
		Serial.println("Start Ntp_Wifi_Device_Driver::DoDeviceMessage - NTP_WIFI_DEVICE_DRIVER_GET_TIME");
#endif // DEBUG
		StartGetTime();
	}
	break;
	}
}


void Ntp_Wifi_Device_Driver::OnNotifyOnline()
{
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::OnNotifyOnline");
#endif // DEBUG
	StartGetTime();
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::OnNotifyOnline");
#endif // DEBUG
}

void Ntp_Wifi_Device_Driver::OnNotifyConnected(){
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::OnNotifyConnected");
#endif // DEBUG
	//
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::OnNotifyConnected");
#endif // DEBUG
}


void Ntp_Wifi_Device_Driver::OnNotifyConnectionLost() {
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::OnNotifyConnectionLost");
#endif // DEBUG
	//
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::OnNotifyConnectionLost");
#endif // DEBUG
}

void Ntp_Wifi_Device_Driver::SyncTimeWithNTP()
{
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::SyncTimeWithNTP");
#endif // DEBUG
	setSyncProvider(getNTP_UTCTime1970);
	setSyncInterval(86400); // NTP re-sync; i.e. 86400 sec would be once per day
	yield();
	//__utc_time = now();
	//__local_time = CE.toLocal(__utc_time);
	//String sResponse = "";
	//sResponse += (F("local time (Berlin) "));
	//sResponse += hour(__local_time); sResponse += (":");
	//sResponse += minute(__local_time) / 10; sResponse += minute(__local_time) % 10; sResponse += (":");
	//sResponse += second(__local_time) / 10; sResponse += second(__local_time) % 10; sResponse += (" - ");
	//sResponse += day(__local_time); sResponse += ("."); sResponse += month(__local_time); sResponse += ("."); sResponse += year(__local_time);
	//Serial.println(sResponse);
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::SyncTimeWithNTP");
#endif // DEBUG
}

void Ntp_Wifi_Device_Driver::DoUpdate(uint32_t deltaTime)
{
}

void Ntp_Wifi_Device_Driver::TimerTick()
{
	if ((__isOnline) && (!__time_is_set) && (__local_counter > 0)) {
		SyncTimeWithNTP();
	} 
	__local_counter--;
	__utc_time = now();
	__local_time = ((Timezone)timezone_Arr[__sv_timezone]._timezone).toLocal(__utc_time);
}



void Ntp_Wifi_Device_Driver::StartGetTime() {
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::StartGetTime");
#endif // DEBUG
	__local_counter = 10;
	__time_is_set = false;
	if (__isOnline) {
		setTime(0);
	}
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::StartGetTime");
#endif // DEBUG
}

void Ntp_Wifi_Device_Driver::SetTimezone(int _timezone) {
	__sv_timezone = _timezone;
}


time_t Ntp_Wifi_Device_Driver::GetLocalTime() const 
{ 
	time_t utc_time;
	time_t local_time;
	utc_time = now();
	local_time = ((Timezone)timezone_Arr[__sv_timezone]._timezone).toLocal(utc_time);
	return local_time;
}

time_t Ntp_Wifi_Device_Driver::GetUtcTime() const
{ 
	time_t utc_time;
	utc_time = now();
	return utc_time;
}


void Ntp_Wifi_Device_Driver::Exec_Start_Get_Time() {
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::Exec_Start_Get_Time");
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(NTP_WIFI_DEVICE_DRIVER_GET_TIME);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::Exec_Start_Get_Time");
#endif // DEBUG
}

void Ntp_Wifi_Device_Driver::Exec_Set_Timezone(int _timezone) {
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::Exec_Set_Timezone");
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(NTP_WIFI_DEVICE_DRIVER_TIMEZONE);
	message->AddParam(_timezone);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::Exec_Set_Timezone");
#endif // DEBUG
}

time_t Ntp_Wifi_Device_Driver::getNTP_UTCTime1970()
{
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::getNTP_UTCTime1970");
#endif // DEBUG
	__time_is_set = false;
	unsigned long t = getNTPTimestamp();
	if (t == 0) {
		
		return(0);
	}
	// scale to 1970 
	// may look like back & forth with ntp code; wrote it to make needed conversions more clear
#ifdef DEBUG
	Serial.println("Ende Ntp_Wifi_Device_Driver::getNTP_UTCTime1970");
#endif // DEBUG
	__time_is_set = true;
	return(t + 946684800UL);
}

unsigned long Ntp_Wifi_Device_Driver::getNTPTimestamp()
{
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::getNTPTimestamp");
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
	Serial.println("Ende Ntp_Wifi_Device_Driver::getNTPTimestamp");
#endif // DEBUG
	return(ulSecs2000);

}

// send an NTP request to the time server at the given address
unsigned long Ntp_Wifi_Device_Driver::sendNTPpacket(IPAddress& address)
{
#ifdef DEBUG
	Serial.println("Start Ntp_Wifi_Device_Driver::sendNTPpacket");
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
	Serial.println("Ende Ntp_Wifi_Device_Driver::sendNTPpacket");
#endif // DEBUG
}

