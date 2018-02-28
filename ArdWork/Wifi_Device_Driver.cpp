// 
// 
// 

#include "Wifi_Device_Driver.h"

//#define DEBUG

REGISTERIMPL(Wifi_Device_Driver);

Led_Device_Driver *Wifi_Device_Driver::statusLED;
bool Wifi_Device_Driver::__isOnline;

void Wifi_Device_Driver::callback_reply(void* arg, void *pdata) {
	struct ping_resp *pingresp = (struct ping_resp *)pdata;
	if (pingresp->resp_time > 0) {
		__isOnline = true;
	}
	else {
		Serial.println(F("Ping Timeout"));
	}
}

Wifi_Device_Driver::Wifi_Device_Driver(Module_Driver* module, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = WIFI_DEVICE_DRIVER_TYPE;
	statusLED = nullptr;
	conn_delta = 0;
	conn_delay = 500;
	__connection_try = 0;
	__dnsServer = nullptr;
	WiFi.mode(WIFI_OFF);
	__isOnline = false;
	__isNotifyOnline = false;
}

void Wifi_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::Build_Descriptor");
#endif // DEBUG
	__descriptor->name = F("Wifi");
	__descriptor->descr = F("connect to local AP");
	__descriptor->published = true;

	Input_CtrlElem *ctrlElem_SSID = new Input_CtrlElem(WIFI_DEVICE_DRIVER_SET_SSID, &__ssid, F("SSID"), F("change SSID"));

	Password_CtrlElem *ctrlElem_pass = new Password_CtrlElem(WIFI_DEVICE_DRIVER_SET_PASSWORD, &__password, F("Passwort"), F("change password"));

	Select_CtrlElem *ctrlElem_mode = new Select_CtrlElem(WIFI_DEVICE_DRIVER_SET_MODE, &__isAP, F("Wifi Mode"), F("AP or STA"));
	ctrlElem_mode->AddMember(F("STA"));
	ctrlElem_mode->AddMember(F("AP"));

	Group_CtrlElem *ctrlElem_reconnect = new Group_CtrlElem(WIFI_DEVICE_DRIVER_RECONNECT, F("Re-Start"), F("restart controller"));
	ctrlElem_reconnect->AddMember(F("Reconnect"));

	__descriptor->Add_Descriptor_Element(ctrlElem_SSID);
	__descriptor->Add_Descriptor_Element(ctrlElem_pass);
	__descriptor->Add_Descriptor_Element(ctrlElem_mode);
	__descriptor->Add_Descriptor_Element(ctrlElem_reconnect);
#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::Build_Descriptor");
#endif // DEBUG
}


void Wifi_Device_Driver::OnInit()
{
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::DoAfterInit");
#endif // DEBUG
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
#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::DoAfterInit");
#endif // DEBUG
}

void Wifi_Device_Driver::SetSSID(String _ssid)
{
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::SetSSID");
#endif // DEBUG
	__ssid = _ssid;
#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::SetSSID");
#endif // DEBUG
}

void Wifi_Device_Driver::SetPassword(String _password)
{
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::SetPassword");
#endif // DEBUG
	__password = _password;
#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::SetPassword");
#endif // DEBUG
}

void  Wifi_Device_Driver::SetMode(uint8_t _mode) {
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::SetMode");
#endif // DEBUG
	__isAP = _mode;
	if (!__WiFi_isConnected && !__AP_isConnected) {
		__run_isAp = _mode;
	}
#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::SetMode");
#endif // DEBUG
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
#ifdef DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_SET_SSID");
#endif // DEBUG
		String ssid = message.GetStringParamByIndex(0);
		SetSSID(ssid);
	}
	break;
	case WIFI_DEVICE_DRIVER_SET_PASSWORD:
	{
#ifdef DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_SET_PASSWORD");
#endif // DEBUG
		String password = message.GetStringParamByIndex(0);
		SetPassword(password);
	}
	break;
	case WIFI_DEVICE_DRIVER_SET_MODE:
	{
#ifdef DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_SET_MODE");
#endif // DEBUG
		int mode = message.GetIntParamByIndex(0);
		SetMode(mode);
	}
	break;
	case WIFI_DEVICE_DRIVER_RECONNECT:
	{
#ifdef DEBUG
		Serial.println("Wifi_Device_Driver::DoDeviceMessage - WIFI_DEVICE_DRIVER_RECONNECT");
#endif // DEBUG
		ResetConnection();
	}
	break;
	}
}

void Wifi_Device_Driver::ProvideAP() {
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::ProvideAP");
#endif // DEBUG
	IPAddress apIP(192, 168, 1, 1);
	const byte DNS_PORT = 53;

	if (__dnsServer != nullptr)
		__dnsServer = nullptr;
	__dnsServer = new DNSServer();
	noInterrupts();
	WiFi.persistent(false); // Do not write new connections to FLASH

	WiFi.mode(WIFI_AP);
	delay(1000);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	const char* ssid = "EspSetup";
	Serial.println(WiFi.softAP(ssid) ? "AccessPoint Ready" : "AccessPoint Failed!");;
	Serial.print("AccessPoint SSID: ["); Serial.print(ssid); Serial.print("]");
	IPAddress myIP = WiFi.softAPIP();
	Serial.print(" IP-address: ");
	Serial.print("[");
	Serial.print(myIP);
	Serial.println("]");
	__AP_isConnected = true;

	// if DNSServer is started with "*" for domain name, it will reply with
	// provided IP to all DNS request
	__dnsServer->start(DNS_PORT, "*", apIP);
	yield();
	StartMSDNServices();
	yield();
	DoNotifyConnected();
	yield();
	interrupts();
#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::ProvideAP");
#endif // DEBUG
}


void Wifi_Device_Driver::StartMSDNServices() {
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::StartMSDNServices");
#endif // DEBUG
	Serial.println(String(__parentModule->GetDescriptor()->name).c_str());
	MDNS.begin(String(__parentModule->GetDescriptor()->name).c_str());
	MDNS.addService(F("http"), F("tcp"), 80);
	MDNS.addService(F("ws"), F("tcp"), 81);
	SetupOTA();
	__isMSDN = true;

#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::StartMSDNServices");
#endif // DEBUG
}

void Wifi_Device_Driver::ConnectToWifi() {
#ifdef DEBUG
	Serial.println("Start Wifi_Device_Driver::ConnectToWifi");
#endif // DEBUG
	//__ssid = "FRITZ!Box 7490";
	//__password = "59049598120613392417";
	const char* ssid = &__ssid[0];
	const char* password = &__password[0];
	Serial.print(F("Try to Connect to ["));
	Serial.print(ssid);
	Serial.print(F("] with password ["));
	Serial.print(password);
	Serial.println(F("]"));
	noInterrupts();
	WiFi.persistent(false); // Do not write new connections to FLASH
	WiFi.mode(WIFI_STA);
	//delay(1000);
	WiFi.begin(ssid, password);
	yield();
	interrupts();
#ifdef DEBUG
	Serial.println("Ende Wifi_Device_Driver::ConnectToWifi");
#endif // DEBUG
	delay(1000);
}

void Wifi_Device_Driver::DoUpdate(uint32_t deltaTime) {
	if ((WiFi.status() != WL_CONNECTED) && !__AP_isConnected) {
		if (__WiFi_isConnected == true) {
			DoNotifyConnectionLost();
			__isOnline = false;
			__isNotifyOnline = false;
			Serial.println(F("Connection lost..."));
			Serial.println(F("Try to reconnect..."));
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

			if ((__connection_try > 20) && (!__WiFi_isConnected)) {
				__run_isAp = true;
				Serial.println(F("Fallback to AccessPoint: "));
			}
		}
		else {
			ProvideAP();
		}
	}
	else {
		if (!__WiFi_isConnected && !__run_isAp) {
			Serial.print(F("Connected to: "));
			Serial.print(__ssid);
			Serial.print(F(" with IP address: "));
			Serial.println(WiFi.localIP());
			__WiFi_isConnected = true;
			StartMSDNServices();
		}

		ArduinoOTA.handle();
		if ((__isOnline) && (!__isNotifyOnline)) {
			__isNotifyOnline = true;
			DoNotifyOnline();
		}

		if (__run_isAp) {
			__dnsServer->processNextRequest();
		}
	}
}


void Wifi_Device_Driver::TimerTick() {
	if (__WiFi_isConnected && !__isOnline) {
		Ping();
	}
}

void Wifi_Device_Driver::Ping()
{
	IPAddress googleServer(172, 217, 0, 0);
	pingopt.ip = googleServer;
	pingopt.count = 1;
	pingopt.coarse_time = 500;
	pingopt.recv_function = &callback_reply;
	// pingopt.sent_function = NULL;
	ping_start(&pingopt);
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
			type = F("sketch");
		else // U_SPIFFS
			type = F("filesystem");

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println(F("\nEnd"));
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
	Serial.println(F("OTA is Ready"));

}