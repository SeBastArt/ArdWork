// 
// 
// 

#include "Wifi_Device_Driver.h"

//#define DEBUG

Led_Device_Driver *Wifi_Device_Driver::statusLED;

Wifi_Device_Driver::Wifi_Device_Driver(Module_Driver* module, Led_Device_Driver *_statusLED, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = WIFI_DEVICE_DRIVER_TYPE;
	statusLED = _statusLED;
	conn_delta = 0;
	conn_delay = 500;
	__connection_try = 0;
	__dnsServer = nullptr;
}

void Wifi_Device_Driver::OnBuild_Descriptor() {
#ifdef  DEBUG
	Serial.println("Start Wifi_Device_Driver::Build_Descriptor");
#endif //  DEBUG
	__descriptor->name = F("Wifi");
	__descriptor->descr = F("Wifi stellt die Verbindung zum heimischen Netzwerk her oder stellt selbst einen AcessPoint bereit");
	__descriptor->published = true;


	Input_CtrlElem *ctrlElem_SSID = new Input_CtrlElem(WIFI_DEVICE_DRIVER_SET_SSID, &__ssid, F("SSID"), F("SSID aendern"));

	Password_CtrlElem *ctrlElem_pass = new Password_CtrlElem(WIFI_DEVICE_DRIVER_SET_PASSWORD, &__password, F("Passwort"), F("Passwort aendern"));

	Select_CtrlElem *ctrlElem_mode = new Select_CtrlElem(WIFI_DEVICE_DRIVER_SET_MODE, &__isAP, F("Wifi Mode"), F("Aendern des Wifi-Modes zwischen AccessPoint und client"));
	ctrlElem_mode->AddMember("STA");
	ctrlElem_mode->AddMember("AP");

	Group_CtrlElem *ctrlElem_reconnect = new Group_CtrlElem(WIFI_DEVICE_DRIVER_RECONNECT, F("Re-Start"), F("here you can reestablish the wifi connection as AP or STA"));
	ctrlElem_reconnect->AddMember("Reconnect");

	__descriptor->Add_Descriptor_Element(ctrlElem_SSID);
	__descriptor->Add_Descriptor_Element(ctrlElem_pass);
	__descriptor->Add_Descriptor_Element(ctrlElem_mode);
	__descriptor->Add_Descriptor_Element(ctrlElem_reconnect);
#ifdef  DEBUG
	Serial.println("Ende Wifi_Device_Driver::Build_Descriptor");
#endif //  DEBUG
}


void Wifi_Device_Driver::OnInit()
{
#ifdef  DEBUG
	Serial.println("Start Wifi_Device_Driver::DoAfterInit");
#endif //  DEBUG
	Device_Driver::OnInit();
	__connection_try = 0;
	__WiFi_isConnected = false;
	__AP_isConnected = false;
	__isAP = false;
	__run_isAp = false;
	__isMSDN = false;
	__ssid = "";
	__password = "";
	hostname = "ESP";
#ifdef  DEBUG
	Serial.println("Ende Wifi_Device_Driver::DoAfterInit");
#endif //  DEBUG
}

void Wifi_Device_Driver::SetSSID(String _ssid)
{
#ifdef  DEBUG
	Serial.println("Start Wifi_Device_Driver::SetSSID");
#endif //  DEBUG
	__ssid = _ssid;
#ifdef  DEBUG
	Serial.println("Ende Wifi_Device_Driver::SetSSID");
#endif //  DEBUG
}

void Wifi_Device_Driver::SetPassword(String _password)
{
#ifdef  DEBUG
	Serial.println("Start Wifi_Device_Driver::SetPassword");
#endif //  DEBUG
	__password = _password;
#ifdef  DEBUG
	Serial.println("Ende Wifi_Device_Driver::SetPassword");
#endif //  DEBUG
}

void  Wifi_Device_Driver::SetMode(uint8_t _mode) {
#ifdef  DEBUG
	Serial.println("Start Wifi_Device_Driver::SetMode");
#endif //  DEBUG
	__isAP = _mode;
	if (!__WiFi_isConnected && !__AP_isConnected) {
		__run_isAp = _mode;
	}
#ifdef  DEBUG
	Serial.println("Ende Wifi_Device_Driver::SetMode");
#endif //  DEBUG
}

void  Wifi_Device_Driver::ResetConnection() {
	ESP.restart();
}


void Wifi_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	case WIFI_DEVICE_DRIVER_SET_SSID:
	{
#ifdef  DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_SET_SSID");
#endif //  DEBUG
		String ssid = message.GetStringParamByIndex(0);
		SetSSID(ssid);
	}
	break;
	case WIFI_DEVICE_DRIVER_SET_PASSWORD:
	{
#ifdef  DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_SET_PASSWORD");
#endif //  DEBUG
		String password = message.GetStringParamByIndex(0);
		SetPassword(password);
	}
	break;
	case WIFI_DEVICE_DRIVER_SET_MODE:
	{
#ifdef  DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_SET_MODE");
#endif //  DEBUG
		int mode = message.GetIntParamByIndex(0);
		SetMode(mode);
	}
	break;
	case WIFI_DEVICE_DRIVER_RECONNECT:
	{
#ifdef  DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_RECONNECT");
#endif //  DEBUG
		ResetConnection();
	}
	break;
	}
}

void Wifi_Device_Driver::ProvideAP() {
#ifdef  DEBUG
	Serial.println("Start Wifi_Device_Driver::ProvideAP");
#endif //  DEBUG
	IPAddress apIP(192, 168, 1, 1);
	const byte DNS_PORT = 53;

	if (__dnsServer != nullptr)
		__dnsServer = nullptr;
	__dnsServer = new DNSServer();

	WiFi.persistent(false); // Do not write new connections to FLASH
	WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

	const char* ssid = "EspSetup";
	WiFi.softAP(ssid);

	// if DNSServer is started with "*" for domain name, it will reply with
	// provided IP to all DNS request
	__dnsServer->start(DNS_PORT, "*", apIP);

	Serial.print("AccessPoint SSID: ["); Serial.print(ssid); Serial.print("]");
	IPAddress myIP = WiFi.softAPIP();
	Serial.print(" IP-address: ");
	Serial.print("[");
	Serial.print(myIP);
	Serial.println("]");
	__AP_isConnected = true;
	StartMSDNServices();
#ifdef  DEBUG
	Serial.println("Ende Wifi_Device_Driver::ProvideAP");
#endif //  DEBUG
}


void Wifi_Device_Driver::StartMSDNServices() {
	MDNS.begin(String(__parentModule->GetDescriptor()->name).c_str());
	MDNS.addService(F("http"), F("tcp"), 80);
	MDNS.addService(F("ws"), F("tcp"), 81);

	SetupOTA();
	InitComm();

	__isMSDN = true;
}

void Wifi_Device_Driver::ConnectToWifi() {
#ifdef  DEBUG
	Serial.println("Start Wifi_Device_Driver::ConnectToWifi");
#endif //  DEBUG
	const char* ssid = &__ssid[0];
	const char* password = &__password[0];
	Serial.print("Try to Connect to [");
	Serial.print(ssid);
	Serial.print("] with password [");
	Serial.print(password);
	Serial.println("]");

	WiFi.persistent(false); // Do not write new connections to FLASH
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
#ifdef  DEBUG
	Serial.println("Ende Wifi_Device_Driver::ConnectToWifi");
#endif //  DEBUG
	delay(1000);
}

void Wifi_Device_Driver::DoUpdate(uint32_t deltaTime) {
#ifdef  DEBUG
	//Serial.println("Start Wifi_Device_Driver::DoUpdate");
#endif //  DEBUG
	if ((WiFi.status() != WL_CONNECTED) && !__AP_isConnected) {
		if (__WiFi_isConnected == true) {
			Serial.println("Connection lost...");
			Serial.println("Try to reconnect...");
			__WiFi_isConnected = false;
			__connection_try = 0;
		}
		if (!__run_isAp) {
			if (!__WiFi_isConnected && (__connection_try == 0)) {
				ConnectToWifi();
			}

			conn_delta += deltaTime;
			if (conn_delta > conn_delay) {
				conn_delta = 0;
				Serial.print(".");
				__connection_try++;
			}

			if ((__connection_try > 30) && (!__WiFi_isConnected)) {
				__run_isAp = true;
				Serial.println("Fallback to AccessPoint: ");
			}
		}
		else {
			ProvideAP();
		}
	}
	else {
		if (!__WiFi_isConnected && !__run_isAp) {
			Serial.print("Connected to: ");
			Serial.print(__ssid);
			Serial.print(" with IP address: ");
			Serial.println(WiFi.localIP());
			__WiFi_isConnected = true;

			StartMSDNServices();
		}

		UpdateComm(deltaTime);
		ArduinoOTA.handle();
		if (__run_isAp) {
			__dnsServer->processNextRequest();
		}
	}
#ifdef  DEBUG
	//Serial.println("Ende Wifi_Device_Driver::DoUpdate");
#endif //  DEBUG
}

void Wifi_Device_Driver::Exec_Set_SSID(String _ssid) {
	Int_Task_Msg *message = new Int_Task_Msg(WIFI_DEVICE_DRIVER_SET_SSID);
	message->AddParam(_ssid);
	PostMessage(&message);
}

void Wifi_Device_Driver::Exec_Set_Password(String _password) {
	Int_Task_Msg *message = new Int_Task_Msg(WIFI_DEVICE_DRIVER_SET_PASSWORD);
	message->AddParam(_password);
	PostMessage(&message);
}

void Wifi_Device_Driver::Exec_Reconnect() {
	Int_Task_Msg *message = new Int_Task_Msg(WIFI_DEVICE_DRIVER_RECONNECT);
	PostMessage(&message);
}

void Wifi_Device_Driver::SetupOTA()
{
	ArduinoOTA.setHostname(&hostname[0]);
	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		else // U_SPIFFS
			type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();
	Serial.println("OTA is Ready");

}