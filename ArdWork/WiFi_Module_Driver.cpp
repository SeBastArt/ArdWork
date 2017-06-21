// 
// 
// 

#include "WiFi_Module_Driver.h"
#include "Led_Device_Driver.h"


WiFi_Module_Driver::WiFi_Module_Driver(String _hostname, String _ssid, String _password, uint8_t priority) : Module_Driver(priority), 
ssid(_ssid),
password(_password),
hostname(_hostname)
{
	wifi_status_Led = 0;
	isConnected = false;
	conn_delta = 0;
	conn_delay = 500;
};


void WiFi_Module_Driver::DoUpdate(uint32_t deltaTime)
{

	if (WiFi.status() != WL_CONNECTED) {
		if (isConnected == true) {
			Serial.println("Connection lost...");
			Serial.println("Try to reconnect...");
			isConnected = false;
		}
		conn_delta += deltaTime;
		if (conn_delta > conn_delay) {
			wifi_status_Led->Exec_Set_IO_Pin_Blink(200);
			conn_delta = 0;
			Serial.print(".");
		}
	}
	else {
		if (!isConnected) {
			Serial.println("WiFi connected");
			Serial.print("IP address: ");
			Serial.println(WiFi.localIP());
			localhost = WiFi.localIP();
			isConnected = true;
			wifi_status_Led->Exec_Set_IO_Pin_Low();
		}
		CheckComm(deltaTime);
		UpdateComm(deltaTime);
		UpdateHardware(deltaTime);
	}
	ArduinoOTA.handle();
}


void WiFi_Module_Driver::DoBeforeSuspend()
{
}

void WiFi_Module_Driver::DoBeforeShutdown()
{
}

void WiFi_Module_Driver::DoAfterInit()
{
	const char* _ssid = &ssid[0];
	const char* _password = &password[0];

	Serial.println("WiFi_Module_Driver initialized!");
	WiFi.begin(_ssid, _password);
	Serial.print("Try to Connect to [");
	Serial.print(ssid);
	Serial.print("] with Passwort [");
	Serial.print(password);
	Serial.println("]");
	
	SetupOTA();
}


void WiFi_Module_Driver::DoModuleMessage(Int_Thread_Msg message)
{
}

void WiFi_Module_Driver::SetupOTA()
{
	ArduinoOTA.setHostname(&hostname[0]);
	ArduinoOTA.onStart([=]() { // switch off wifi_status_Led
		analogWrite(wifi_status_Led->GetPinNumber(), 0);
	});

	ArduinoOTA.onEnd([=]() { // do a fancy thing with wifi_status_Led
		for (int i = 0; i<30; i++)
		{
			analogWrite(wifi_status_Led->GetPinNumber(), (i * 100) % 1001);
			delay(50);
		}
	});

	ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });

	/* setup the OTA server */
	ArduinoOTA.begin();
	Serial.println("OTA is Ready");
}

void WiFi_Module_Driver::SetWiFiStatusLed(Led_Device_Driver *led) {
	AddDevice(led);
	wifi_status_Led = led;
}

void WiFi_Module_Driver::SetHostName(String _hostname)
{
	hostname = _hostname;
}

