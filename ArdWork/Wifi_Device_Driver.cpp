// 
// 
// 

#include "Wifi_Device_Driver.h"

Led_Device_Driver *Wifi_Device_Driver::statusLED;

Wifi_Device_Driver::Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED, uint8_t priority) :
	Device_Driver(module, priority),
	Ssid(_ssid),
	Password(_password)
{
	driver_name = "Wifi_Device_Driver";
	statusLED = _statusLED;
	isConnected = false;
	conn_delta = 0;
	conn_delay = 500;
}

void Wifi_Device_Driver::DoAfterInit()
{
	Serial.println("Wifi_Device_Driver::DoAfterInit()");
	const char* _ssid = &Ssid[0];
	const char* _password = &Password[0];

	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("WiFi_Module_Driver initialized!");
		WiFi.begin(_ssid, _password);
		Serial.print("Try to Connect to [");
		Serial.print(Ssid);
		Serial.print("] with Passwort [");
		Serial.print(Password);
		Serial.println("]");
		InitComm();
	}
	Serial.println("Wifi_Device_Driver::DoAfterInit() Done");
}

void Wifi_Device_Driver::DoBeforeShutdown()
{
}

void Wifi_Device_Driver::DoBeforeSuspend()
{
}

void Wifi_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{
}

void Wifi_Device_Driver::DoUpdate(uint32_t deltaTime)
{
	if (WiFi.status() != WL_CONNECTED) {
		if (isConnected == true) {
			Serial.println("Connection lost...");
			Serial.println("Try to reconnect...");
			isConnected = false;
		}
		conn_delta += deltaTime;
		if (conn_delta > conn_delay) {
			if (statusLED != NULL)
				statusLED->Exec_Set_Led_Blink(200);
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
			if (statusLED != NULL)
				statusLED->Exec_Set_Led_Off();
		}
		UpdateComm(deltaTime);
	}
}

void Wifi_Device_Driver::SetWiFiStatusLed(Led_Device_Driver *led) {
	statusLED = led;
}

void Wifi_Device_Driver::SetHostName(String _hostname)
{
	Serial.print("Set HostName To: ");
	Serial.println(_hostname);
	hostname = _hostname;
}


