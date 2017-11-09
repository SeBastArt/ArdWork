// 
// 
// 

#include "Wifi_Device_Driver.h"

Led_Device_Driver *Wifi_Device_Driver::statusLED;

Wifi_Device_Driver::Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = WIFI_DEVICE_DRIVER_TYPE;
	statusLED = _statusLED;
	__isConnected = false;
	__isAP = false;
	__ssid = "";
	__password = "";
	hostname = F("ESP");
	conn_delta = 0;
	conn_delay = 500;
	__dnsServer = nullptr;
}

void Wifi_Device_Driver::Build_Descriptor() {
	__descriptor->name = F("Wifi");
	__descriptor->descr = F("Wifi stellt die Verbindung zum heimischen Netzwerk her oder stellt selbst einen AcessPoint bereit");
	__descriptor->published = true;

	Ctrl_Elem *ctrl_elem_SSID = new Ctrl_Elem(WIFI_DEVICE_DRIVER_SET_SSID, F("SSID"), text, F("SSID aendern"));

	Atomic<String> *atomic_SSID = new Atomic<String>(0, F("SSID"));
	ctrl_elem_SSID->AddAtomic(atomic_SSID);
	ctrl_elem_SSID->published = true;

	Ctrl_Elem *ctrl_elem_pass = new Ctrl_Elem(WIFI_DEVICE_DRIVER_SET_PASSWORD, F("Passwort"), pass, F("Passwort aendern"));

	Atomic<String> *atomic_pass = new Atomic<String>(0, F("Passwort"));
	ctrl_elem_pass->AddAtomic(atomic_pass);
	ctrl_elem_pass->published = true;

	Ctrl_Elem *ctrl_elem_mode = new Ctrl_Elem(WIFI_DEVICE_DRIVER_SET_MODE, F("Wifi Mode"), select, F("Aendern des Wifi-Modes zwischen AccessPoint und client"));
	Atomic<String> *atomic_ap = new Atomic<String>(0, F("STA"));
	Atomic<String> *atomic_sta = new Atomic<String>(1, F("AP"));
	ctrl_elem_mode->AddAtomic(atomic_ap);
	ctrl_elem_mode->AddAtomic(atomic_sta);
	ctrl_elem_mode->published = true;

	Ctrl_Elem *ctrl_elem_reconnect = new Ctrl_Elem(WIFI_DEVICE_DRIVER_RECONNECT, F("Reconnect"), button_group, F("here you can reestablish the wifi connection as AP or STA"));
	Atomic<String> *atomic_reconnect = new Atomic<String>(2, F("Reconnect"));
	ctrl_elem_reconnect->AddAtomic(atomic_reconnect);
	
	ctrl_elem_reconnect->published = true;

	__descriptor->Add_Descriptor_Element(ctrl_elem_SSID);
	__descriptor->Add_Descriptor_Element(ctrl_elem_pass);
	__descriptor->Add_Descriptor_Element(ctrl_elem_mode);
	__descriptor->Add_Descriptor_Element(ctrl_elem_reconnect);
}


void Wifi_Device_Driver::DoAfterInit()
{
	Serial.println(F("WiFi_Module_Driver initialized!"));
	__connection_try = 0;

	__isAP = false;
	__isConnected = false;
	__connection_try = 0;
	__fallbackAP = false;

	_filesystem.OpenFile(F("/config.json"));
	__ssid = _filesystem.Json_GetvalueFromKey(F("WifiSsid"));
	__password = _filesystem.Json_GetvalueFromKey(F("password"));
	__isAP = _filesystem.Json_GetvalueFromKey(F("isAP")).equals(F("1"));
	_filesystem.CloseFile();

	// Add service to MDNS
	MDNS.begin(String(parentModule->GetDescriptor()->name).c_str());
	MDNS.addService(F("http"), F("tcp"), 80);
	MDNS.addService(F("ws"), F("tcp"), 81);

	if (__isAP) {
		ProvideAP();
		InitComm();
	}
	else {
		ConnectToWifi();
	}

}

void Wifi_Device_Driver::ProvideAP() {
	const char* ssid;
	IPAddress apIP(192, 168, 1, 1);
	const byte DNS_PORT = 53;


	if (!__ssid.equals(""))
		ssid = &__ssid[0];
	else
		ssid = "EspSetup";

	if (__dnsServer != nullptr)
		__dnsServer = nullptr;
	__dnsServer = new DNSServer();
	WiFi.disconnect();
	WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(ssid);

	// if DNSServer is started with "*" for domain name, it will reply with
	// provided IP to all DNS request
	__dnsServer->start(DNS_PORT, "*", apIP);

	Serial.print("AccessPoint SSID: [");
	Serial.print(ssid);
	Serial.print("]");
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("]; IP-address: ");
	Serial.println(myIP);
	delay(1000);
	__isConnected = true;
}

void Wifi_Device_Driver::ConnectToWifi() {
	__connection_try = 0;
	WiFi.disconnect();
	WiFi.mode(WIFI_STA);
	const char* ssid = &__ssid[0];
	const char* password = &__password[0];
	WiFi.begin(ssid, password);
	Serial.print("Try to Connect to [");
	Serial.print(ssid);
	Serial.print("] with password [");
	Serial.print(password);
	Serial.println("]");
}

void Wifi_Device_Driver::SaveConnectionParameters() {
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["WifiSsid"] = __ssid;
	root["password"] = __password;
	root["isAP"] = String(__isAP);
	String output;
	root.printTo(output);
	Serial.println(output);
	_filesystem.SaveToFile("/config.json", output);
}

void Wifi_Device_Driver::SetSSID(String _ssid)
{
	Serial.println("SetSSID");
	__ssid = _ssid;
	SaveConnectionParameters();
}

void Wifi_Device_Driver::SetPassword(String _password)
{
	__password = _password;
	SaveConnectionParameters();
}

void  Wifi_Device_Driver::SetMode(int _mode) {
	__isAP = _mode;
	SaveConnectionParameters();
}

void Wifi_Device_Driver::DoBeforeShutdown()
{
}

void Wifi_Device_Driver::DoBeforeSuspend()
{
}

void Wifi_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{
	int messageID = message.GetID();
	switch (messageID)
	{
	case WIFI_DEVICE_DRIVER_SET_SSID:
	{
		String ssid = message.GetStringParamByIndex(1);
		SetSSID(ssid);
	}
	break;
	case WIFI_DEVICE_DRIVER_SET_PASSWORD:
	{
		String password = message.GetStringParamByIndex(1);
		SetPassword(password);
	}
	break;
	case WIFI_DEVICE_DRIVER_SET_MODE:
	{
		int mode = message.GetIntParamByIndex(1);
		SetMode(mode);
	}
	break;
	case WIFI_DEVICE_DRIVER_RECONNECT:
	{
		ESP.reset();
	}
	break;
	}
}

void Wifi_Device_Driver::DoUpdate(uint32_t deltaTime) {
	if ((WiFi.status() != WL_CONNECTED) && !__isAP) {
		if (__isConnected == true) {
			Serial.println("Connection lost...");
			Serial.println("Try to reconnect...");
			__isConnected = false;
			__fallbackAP = false;
			ConnectToWifi();
		}
		conn_delta += deltaTime;
		if (conn_delta > conn_delay) {
			//if (statusLED != nullptr)
				//statusLED->Exec_Set_Led_Blink(200);
			conn_delta = 0;
			Serial.print(".");
			__connection_try++;
		}

		if ((__connection_try > 30) && (!__isConnected)) {
			__isAP = true;
			__fallbackAP = true;
			Serial.println("Fallback to AccessPoint: ");
			ProvideAP();
			InitComm();
			//if (statusLED != nullptr)
				//statusLED->Exec_Set_Led_Off();
		}
	}
	else {
		if (!__isConnected && !__isAP) {
			Serial.print("Connected to: ");
			Serial.print(__ssid);
			Serial.print(" with IP address: ");
			Serial.println(WiFi.localIP());
			localhost = WiFi.localIP();
			__isConnected = true;
			__isAP = false;
			__fallbackAP = false;
			SetupOTA();
			InitComm();
			//if (statusLED != nullptr)
				//statusLED->Exec_Set_Led_Off();
		}
		UpdateComm(deltaTime);
	}
	if (__isAP && __isConnected) {
		__dnsServer->processNextRequest();
	}
}

void Wifi_Device_Driver::Exec_Set_SSID(String _ssid) {
	Int_Thread_Msg *message = new Int_Thread_Msg(WIFI_DEVICE_DRIVER_SET_SSID);
	message->AddParam(_ssid);
	PostMessage(&message);
}

void Wifi_Device_Driver::Exec_Set_Password(String _password) {
	Int_Thread_Msg *message = new Int_Thread_Msg(WIFI_DEVICE_DRIVER_SET_PASSWORD);
	message->AddParam(_password);
	PostMessage(&message);
}

void Wifi_Device_Driver::Exec_Reconnect() {
	Int_Thread_Msg *message = new Int_Thread_Msg(WIFI_DEVICE_DRIVER_RECONNECT);
	PostMessage(&message);
}

void Wifi_Device_Driver::SetupOTA()
{
	ArduinoOTA.setHostname(&hostname[0]);

	ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });

	/* setup the OTA server */
	ArduinoOTA.begin();
	Serial.println("OTA is Ready");
}