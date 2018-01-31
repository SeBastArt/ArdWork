#include "Ntp_Wifi_Device_Driver.h"

// NTP specifics
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
WiFiUDP udp;  // A UDP instance to let us send and receive packets over UDP
unsigned int ntpPort = 2390;          // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48;       // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets
bool bNTPStarted = false;

TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };     //Central European Summer Time
TimeChangeRule CET = { "CET ", Last, Sun, Oct, 3, 60 };       //Central European Standard Time
Timezone CE(CEST, CET);

Ntp_Wifi_Device_Driver::Ntp_Wifi_Device_Driver(Module_Driver * module, uint8_t priority):
	Device_Driver(module, priority)
{
	accuracy_delta = 0;
	accuracy_delay = 2000;
}


void Ntp_Wifi_Device_Driver::OnNotifyConnected()
{
}

void Ntp_Wifi_Device_Driver::OnNotifyConnectionLost()
{
}

void Ntp_Wifi_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
}

void Ntp_Wifi_Device_Driver::DoUpdate(uint32_t deltaTime)
{
	if (__isOnline) {
		accuracy_delta += deltaTime;
		if (accuracy_delta > accuracy_delay) {
			accuracy_delta = 0;

			if (timeStatus() == timeNotSet)
			{
				bNTPStarted = false;
				Serial.println("Setup sync with NTP service.");
				setSyncProvider(getNTP_UTCTime1970);
				setSyncInterval(86400); // NTP re-sync; i.e. 86400 sec would be once per day
			}
			else
			{
				bNTPStarted = true;   // only position, where we set this flag to true
			}
			yield();
			String sResponse;
			time_t tT = now();
			sResponse += F("UTC (Coordinated Universal Time) ");
			sResponse += hour(tT); sResponse += F(":");
			sResponse += minute(tT) / 10; sResponse += minute(tT) % 10; sResponse += F(":");
			sResponse += second(tT) / 10; sResponse += second(tT) % 10; sResponse += F(" - ");
			sResponse += day(tT); sResponse += F("."); sResponse += month(tT); sResponse += F("."); sResponse += year(tT);
			Serial.println(sResponse);

			if (bNTPStarted) {
				sResponse = "";
				time_t tTlocal = CE.toLocal(tT);
				sResponse += F("Lokale Zeit (Mitteleuropa) ");
				sResponse += hour(tTlocal); sResponse += F(":");
				sResponse += minute(tTlocal) / 10; sResponse += minute(tTlocal) % 10; sResponse += F(":");
				sResponse += second(tTlocal) / 10; sResponse += second(tTlocal) % 10; sResponse += F(" - ");
				sResponse += day(tTlocal); sResponse += F("."); sResponse += month(tTlocal); sResponse += F("."); sResponse += year(tTlocal);
				Serial.println(sResponse);
			}
			Serial.println();
		}
	}
}

time_t Ntp_Wifi_Device_Driver::getNTP_UTCTime1970()
{
	bNTPStarted = false;  // invalidate; time-lib functions crash, if not initalized poperly
	unsigned long t = getNTPTimestamp();
	if (t == 0) return(0);

	// scale to 1970 
	// may look like back & forth with ntp code; wrote it to make needed conversions more clear
	return(t + 946684800UL);
}


unsigned long Ntp_Wifi_Device_Driver::getNTPTimestamp()
{
	unsigned long ulSecs2000;

	udp.begin(ntpPort);
	sendNTPpacket(timeServer); // send an NTP packet to a time server
	delay(1000);    // wait to see if a reply is available
	int cb = 0, repeat = 0;
	while (!cb && repeat<20)  // try for 2 sec
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
	return(ulSecs2000);
}

// send an NTP request to the time server at the given address
unsigned long Ntp_Wifi_Device_Driver::sendNTPpacket(IPAddress& address)
{
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
}

