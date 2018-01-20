#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include "WEBset.h" // Hier ist die HTML-Webseite als Array abgelegt

#define LEDpin 1 // LED zum Blinken. Bei ESP-07 Pin 2. Bei ESP-01 Pin 1
// #define seriell; // Einschalter für serielle Ausgabe
IPAddress apIP(192, 168, 178, 1); // IP-Adresse für Setup

ADC_MODE(ADC_VCC); // ADC an das Stromversorgung
int z = 0; //Aktuelle EEPROM-Adresse zum lesen
String nachricht = "Testnachricht";
String url = "";
int timout = 0;

char ssid[32] = "fhgdhfghd\0";
char passwort[64] = "gghdfgh\0";
ESP8266WebServer server;
EspClass ESPm;
HTTPClient http;
#include "Setup.h"

extern "C" {
#include "user_interface.h"
}

void setup() {
	char inser;
	EEPROM.begin(250); // EEPROM initialisieren mit 200 Byts

	Serial.begin(115200);
	while (Serial.available())inser = Serial.read();
	Serial.println("Start");
	delay(10);
	inser = Serial.read();
	if (inser != 'S') goto weiter;
	inser = Serial.read();
	if (inser != 't') goto weiter;
	Einstellen(); // TxD ist mit RxD verbunden Setupmodus (Setup.h)
weiter:

	z = 0; // Einstellungen einlesen
	LeseEeprom(ssid, sizeof(ssid));
	LeseEeprom(passwort, sizeof(passwort));
	LeseEepromStr(&url, 100);
	LeseEepromStr(&nachricht, 100);

	unsigned int Ziklus[2]; // Zykluszehler
	const rst_info * resetInfo = system_get_rst_info(); // Resetgrund einlesen
	ESPm.rtcUserMemoryRead(0, Ziklus, sizeof(Ziklus)); // Zyklus Nummer einlesen
	if ((resetInfo->reason) == 6)Ziklus[0] = 0; // Wenn Spannung OFF 
	Ziklus[0]++;
	ESPm.rtcUserMemoryWrite(0, Ziklus, sizeof(Ziklus)); // Zyklus Nummer speichern

	uint ADCValue = 0; // Batteriespannung messen
	ADCValue = ESPm.getVcc() - 164; // hier Korrektur Wert eintragen
	float ADCfloat = float(ADCValue);
	String Ubatt = "";
	Ubatt = String(ADCfloat / 1000, 2) + "V";

	WiFi.mode(WIFI_STA);
# if defined seriell
	Serial.println();
	Serial.print("Ziklus: ");
	Serial.println(Ziklus[0] - 1);
	Serial.print("U Batt: ");
	Serial.println(Ubatt);
	Serial.print("Connecting to ");
	Serial.println(ssid);
	// Serial.print(" Pass: ");
	// Serial.println (passwort);
	Serial.println(url);
	Serial.println(nachricht);
#endif

	WiFi.begin(ssid, passwort);

#if not defined seriell // Vorbereiten das LED
	pinMode(LEDpin, OUTPUT);
#endif

	while (WiFi.status() != WL_CONNECTED)
	{
		timout++;
		if (timout > 60) // Wenn Anmeldung nicht möglich
		{
			WiFi.forceSleepBegin(); // Sender OFF um Strom zu sparen
#if defined seriell
			Serial.print("Netzwerk nicht gefunden");
#else
			pinMode(LEDpin, OUTPUT);
			digitalWrite(LEDpin, 0); // LED ON für 5 Sekunden
			delay(5000);
			digitalWrite(LEDpin, 1);

#endif
			WiFi.forceSleepWake(); // Sender ON, sonst beim nächsten aufwachen geht WiFi nicht
			ESPm.deepSleep(0, WAKE_RFCAL);
			delay(100);
		} // End timeout

		WiFi.forceSleepBegin(); // Wlan aus
		pinMode(LEDpin, OUTPUT);
		digitalWrite(LEDpin, 0);
		delay(5000);
		digitalWrite(LEDpin, 1);
		WiFi.forceSleepWake(); // Wlan ON für neue Zyklus
		ESPm.deepSleep(0, WAKE_RFCAL); //gehe schlafen

	}

	ESPm.deepSleep(0, WAKE_RFCAL);
	delay(100);
}

void loop()
{
	yield();
}

