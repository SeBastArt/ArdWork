#include "WebSocket_Wifi_Device_Driver.h"
#include <ArduinoJson.h>
#include <FS.h>

//#define DEBUG

ESP8266WebServer *WebSocket_Wifi_Device_Driver::server;
WebSocketsServer *WebSocket_Wifi_Device_Driver::webSocket;
event_msg WebSocket_Wifi_Device_Driver::__event_msg;

REGISTERIMPL(WebSocket_Wifi_Device_Driver);

String WebSocket_Wifi_Device_Driver::Json_GetvalueFromKey(String _text, String _key) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::Json_GetvalueFromKey"));
#endif // DEBUG
	String value = "";
	std::unique_ptr<char[]> buf(new char[_text.length() + 1]);
	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.

	_text.toCharArray(buf.get(), _text.length() + 1);

	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(buf.get());

	if (!root.success()) {
		Serial.println(F("Failed to parse json file"));
	}
	value = root[_key].as<String>();
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::Json_GetvalueFromKey"));
#endif // DEBUG
	return value;
}


void WebSocket_Wifi_Device_Driver::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, int length) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::webSocketEvent"));
#endif // DEBUG
	switch (type) {
	case WStype_DISCONNECTED:
		Serial.printf(String(F("[%u] Disconnected!")).c_str(), num);
		break;
	case WStype_CONNECTED: {
		IPAddress ip = webSocket->remoteIP(num);
		Serial.printf(String(F("[%u] Connected from %d.%d.%d.%d url: %s")).c_str(), num, ip[0], ip[1], ip[2], ip[3], payload);

		// send message to client
		webSocket->sendTXT(num, "Connected");
	}
						   break;
	case WStype_TEXT:
#ifdef DEBUG
		Serial.println(F("Start WebSocket_Wifi_Device_Driver::webSocketEvent WStype_TEXT"));
#endif // DEBUG
		Serial.printf(String(F("[%u] get Text: %s")).c_str(), num, payload);
		Serial.println();
		if (payload[0] == '{') {
			// we get RGB data

			// decode rgb data
			//uint32_t rgb = (uint32_t)strtol((const char *)&payload[1], NULL, 16);
			String json_string = String((const char *)&payload[0]);

			int deviceId = atoi(Json_GetvalueFromKey(json_string, F("deviceId")).c_str());
			int cmdId = atoi(Json_GetvalueFromKey(json_string, F("cmdId")).c_str());
			String value = Json_GetvalueFromKey(json_string, F("value")).c_str();
			if (__event_msg._filled == false) {
				__event_msg._deviceId = deviceId;
				__event_msg._cmdId = cmdId;
				__event_msg._value = value;
				__event_msg._filled = true;
			}
			delay(10);
		}
#ifdef DEBUG
		Serial.println(F("Ende WebSocket_Wifi_Device_Driver::webSocketEvent WStype_TEXT"));
#endif // DEBUG
		break;
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::webSocketEvent"));
#endif // DEBUG
}

WebSocket_Wifi_Device_Driver::WebSocket_Wifi_Device_Driver(Module_Driver * module, uint8_t priority) :
	Device_Driver(module, priority)
{
#ifdef DEBUG
	Serial.print("Start WebSocket_Wifi_Device_Driver with ID: "));
	Serial.println(this->DriverId);
#endif // DEBUG
	__DriverType = WEBSOCKET_WIFI_DEVICE_DRIVER_TYPE;
	server = nullptr;
	webSocket = nullptr;
	__event_msg._filled = false;
	SetTimerDelay(1000);
	webSocket = new WebSocketsServer(81);
#ifdef DEBUG
	Serial.println(F("Ende Start WebSocket_Wifi_Device_Driver"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::TimerTick() {
	if (!__isConnected)
		return;

	if (__descriptor_list->count <= 0)
		return;

	for (uint8_t I = 0; I < __descriptor_list->count; I++)
	{

		if (!__descriptor_list->GetElemByIndex(I)->published)
			continue;

		for (uint8_t J = 0; J < __descriptor_list->GetElemByIndex(I)->ctrl_count; J++)
		{
			if ((__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->type == value) ||
				(__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->type == zeit)) {

				if (__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->editable)
					continue;

				String json_str;
				StaticJsonBuffer<200> jsonBuffer;
				JsonObject& root = jsonBuffer.createObject();
				root[F("_deviceId")] = String(__descriptor_list->GetElemByIndex(I)->id);
				root[F("_cmdId")] = String(__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->id);
				root[F("_value")] = __descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->ToString();
				root[F("_unit")] = String(__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->unit);
				root.printTo(json_str);
				webSocket->sendTXT(0, json_str.c_str());
			}
		}
	}
}

void WebSocket_Wifi_Device_Driver::DoUpdate(uint32_t deltaTime) {
#ifdef DEBUG
	//Serial.println(F("Start WebSocket_Wifi_Device_Driver::UpdateComm"));
#endif // DEBUG
	if (__isConnected) {
		webSocket->loop();
		server->handleClient();

		if (__event_msg._filled) {
			ExternMessage* message = new ExternMessage(__event_msg._deviceId, __event_msg._cmdId, __event_msg._value);
#ifdef DEBUG
			Serial.println(F("WebSocket_Wifi_Device_Driver::UpdateComm SendAsyncTaskMessage"));
#endif // DEBUG
			if (!__parentModule->SendAsyncTaskMessage(message))
			{
				Serial.println(F(">> message buffer overflow <<"));
			}
			__event_msg._filled = false;
		}
	}
#ifdef DEBUG
	//Serial.println(F("Ende WebSocket_Wifi_Device_Driver::UpdateComm"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::OnNotifyConnectionLost()
{
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::OnNotifyConnectionLost"));
#endif // DEBUG
	if (server != nullptr) {
		server->~ESP8266WebServer();
		server = nullptr;
	}

	if (webSocket != nullptr) {
		webSocket->~WebSocketsServer();
		webSocket = nullptr;
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::OnNotifyConnectionLost"));
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::OnNotifyOnline() {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::OnNotifyOnline"));
#endif // DEBUG
	InitializeServices();
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::OnNotifyOnline"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	default:
		break;
	}
}

void WebSocket_Wifi_Device_Driver::OnNotifyConnected()
{
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::OnNotifyConnected"));
#endif // DEBUG
	InitializeServices();
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::OnNotifyConnected"));
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::InitializeServices()
{
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices"));
#endif // DEBUG
	if (server != nullptr) {
		server->~ESP8266WebServer();
		server = nullptr;
	}
	server = new ESP8266WebServer(80);

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - Server started"));
#endif // DEBUG
	server->on(F("/dist/css/kube.min.css"), [&]() {
		File file = SPIFFS.open(String(F("/dist/css/kube.min.css")).c_str(), "r");
		server->streamFile(file, F("text/css"));
		file.close();
	});
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - CSSs Loaded"));
#endif // DEBUG

	server->on(F("/dist/js/jscolor.min.js"), [&]() {
		File file = SPIFFS.open(String(F("/dist/js/jscolor.min.js")).c_str(), "r");
		server->streamFile(file, F("application/javascript"));
		file.close();
	});

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - JavaScripts Loaded"));
#endif // DEBUG

	server->on("/", [&]() {
#ifdef DEBUG
		Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices server->on"));
		Serial.printf("before all heap size: %u\n", ESP.getFreeHeap());
#endif // DEBUG
		noInterrupts();
		WiFiClient client = server->client();
		client.println(F("<!DOCTYPE html>")); 
		client.println(F("<html>"));
		SendHeader(&client);
		client.println(F("<body>"));
		if (__descriptor_list->count > 0) {
			client.println(F("<div class=\"container\">"));
			client.print(F("<h2 class=\"head-line\">")); client.print(String(__descriptor_list->projectname)); client.println(F("</h2>"));
			GenerateNav(&client, __descriptor_list);
			bool first = true;
			for (uint8_t I = 0; I < __descriptor_list->count; I++) {
				if (__descriptor_list->GetElemByIndex(I)->published) {
					client.print("<div id=\"tab" + String(__descriptor_list->GetElemByIndex(I)->name) + "\" class=\"tabcontent\"");
					if (first) {
						client.print(" style=\"display:block\"");
						first = false;
					}
					client.println(F(">"));
					GenerateTab(&client, __descriptor_list->GetElemByIndex(I));
					client.println(F("</div>"));
					yield();
				}
			}
			client.println(F("</div>"));
		}
		client.println(F("</body>"));
		client.println(F("</html>"));
		client.stop();
		interrupts();
#ifdef DEBUG
		Serial.printf("after all heap size: %u\n", ESP.getFreeHeap());
		Serial.println(F("Ende WebSocket_Wifi_Device_Driver::InitializeServices server->on"));
#endif // DEBUG
	});

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - dynamic content Loaded"));
#endif // DEBUG

	server->begin();


#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - server started"));
#endif // DEBUG

	if (webSocket != nullptr) {
		webSocket->~WebSocketsServer();
		webSocket = nullptr;
	}
	webSocket = new WebSocketsServer(81);
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - websocket created"));
#endif // DEBUG

	/*if (MDNS.begin(String(F("esp8266")).c_str())) {
		Serial.println(F("MDNS responder started"));
	}*/

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - MDNS started"));
#endif // DEBUG

	webSocket->onEvent(webSocketEvent);
	webSocket->begin();

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - websocket started"));
#endif // DEBUG

#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::InitComm"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateNav(WiFiClient *client, Descriptor_List *_descriptor_list) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateNav"));
#endif // DEBUG
	client->println(F("		<div class=\"tab\">"));
	bool first = true;
	for (uint8_t I = 0; I < __descriptor_list->count; I++)
		if (__descriptor_list->GetElemByIndex(I)->published) {
			client->print(F("			<button class = \"tablinks"));
			if (first) {
				client->print(F(" active"));
				first = false;
			}
			client->print(F("\" "));
			client->print(String(F("onclick=\"openTab(event, 'tab")) + String(__descriptor_list->GetElemByIndex(I)->name) + "')\">");
			client->print(String(__descriptor_list->GetElemByIndex(I)->name));
			client->println(F("</button>"));
		}
	client->println(F("		</div>"));

#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateNav"));
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::Build_Descriptor"));
#endif // DEBUG
	__descriptor->name = F("WebSocekt Driver");
	__descriptor->descr = F("Broadcasts the website of all controls");
	__descriptor->published = false;
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::Build_Descriptor"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateTab(WiFiClient *client, Descriptor *_descriptor) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateTab"));
#endif // DEBUG
	client->println(F("	<div class=\"simple-text\">"));
	client->println(F("		<p>"));
	client->print(F("		<strong>"));
	client->print(String(_descriptor->descr) + "");
	client->println(F("</strong>"));
	client->println(F("		</p>"));
	client->println(F("			</div>"));
	client->println(F("	<div class=\"head-line small\">Control Panel</div>"));
	client->println(F("	<br>"));
	for (uint8_t I = 0; I < _descriptor->ctrl_count; I++) {
		GenerateForm(client, _descriptor->id, _descriptor->GetCtrlElemByIndex(I));
	}
	client->println(F("	<br>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateTab"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateForm(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateForm"));
#endif // DEBUG
	client->println(F("		<div class = \"form-item\">"));
	switch (_ctrl_elem->type)
	{
	case input:
	{
		client->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateInput(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println(F("			</div>"));
		break;
	}
	case pass:
	{
		client->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateInput(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println(F("			</div>"));
		break;
	}
	case select:
	{
		client->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateSelect(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println(F("			</div>"));
		break;
	}

	case color:
	{
		client->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateColor(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println(F("			</div>"));
		break;
	}
	case group:
	{
		GenerateButtonGroup(client, _deviceId, _ctrl_elem);
		break;
	}
	case value:
	{
		if (_ctrl_elem->editable)
			client->println(F("			<div class = \"append w45 w100-sm\">"));
		else
			client->println(F("			<div class = \"append w35 w100-sm\">"));
		GenerateInput(client, _deviceId, _ctrl_elem);
		if (_ctrl_elem->editable)
			GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println(F("			</div>"));
		break;
	}
	case zeit:
	{
		client->println(F("			<div class = \"append w35 w100-sm\">"));
		GenerateInput(client, _deviceId, _ctrl_elem);
		client->println(F("			</div>"));
		break;
	}
	default:
		break;
	}
	GenerateDecending(client, _ctrl_elem);
	client->println(F("		</div>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateForm"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateDecending(WiFiClient *client, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateDecending"));
#endif // DEBUG
	client->println(F("			<div class=\"desc\">"));
	switch (_ctrl_elem->type)
	{
	case input:
	{
		client->print(F("				Set "));
		break;
	}

	case pass:
	{
		client->print(F("				Set "));
		break;
	}
	case select:
	{
		client->print(F("				Select "));
		break;
	}
	case color:
	{
		client->print(F("				Pick a "));
		break;
	}
	case group:
	{
		break;
	}
	}
	client->println(String(_ctrl_elem->name));
	if (_ctrl_elem->type == value)
		client->print(" [" + String(_ctrl_elem->unit) + "]");
	client->println(F("			</div>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateDecending"));
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::GenerateColor(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateColor"));
#endif // DEBUG
	client->print(F("			<input "));
	client->print(F("value=\""));
	client->print(reinterpret_cast<Color_CtrlElem*>(_ctrl_elem)->ToString());
	client->print(F("\" class=\"small "));
	client->print(F("jscolor{ mode:\'HSV\',width:225, height:130, position:\'top\', borderColor :\'#e3e3e3\', insetColor : \'#666\', backgroundColor : \'#666\'}\" "));
	client->println(String(F("id=\"")) + String(_deviceId) + "_" + String(_ctrl_elem->id) + String(F("\">")));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateColor"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateSelect(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateSelect"));
#endif // DEBUG
	client->print(F("				<select class=\"small\" "));
	client->println(String(F("id=\"")) + String(_deviceId) + "_" + String(_ctrl_elem->id) + String(F("\">")));
	GenerateMultiOption(client, _ctrl_elem);
	client->println(F("				</select>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateSelect"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateMultiOption(WiFiClient *client, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateMultiOption"));
#endif // DEBUG
	for (uint8_t I = 0; I < ((Select_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		client->print(F("					<option"));
		int t = ((Select_CtrlElem*)_ctrl_elem)->ToString().toInt();
		if (((Select_CtrlElem*)_ctrl_elem)->ToString().toInt() == I)
			client->print(F(" selected "));
		client->print(F("	value=\""));
		client->print(String(I));
		client->print(F("\">"));
		client->print(((Select_CtrlElem*)_ctrl_elem)->GetMember(I));
		client->print(_ctrl_elem->unit);
		client->println(F("</option>"));
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateMultiOption"));
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::GenerateInput(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateInput"));
#endif // DEBUG
	if (_ctrl_elem->type == pass)
		client->print(F("					<input type=\"password\" class=\"small\" id=\""));
	else
		client->print(F("					<input type=\"text\" class=\"small\" id=\""));
	client->print(String(_deviceId) + "_" + String(_ctrl_elem->id));
	client->print(F("\"  value=\""));
	client->print(_ctrl_elem->ToString());
	if (!_ctrl_elem->editable) {
		client->println(F("\" disabled>"));
	}
	else {
		client->println(F("\">"));
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateInput"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateButtonGroup(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateButtonGroup"));
#endif // DEBUG
	for (uint8_t I = 0; I < ((Group_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		client->print(F("<div class=\""));
		if (((Group_CtrlElem*)_ctrl_elem)->MembersCount() > 1) {
			if (I > 0) {
				client->print(F(" append "));
			}
			if (I < (((Group_CtrlElem*)_ctrl_elem)->MembersCount() - 1)) {
				client->print(F(" prepend "));
			}
		}
		client->print(F("\">"));
		client->print(F("				<button class=\"button small\" "));
		client->print(F("onclick=\"SendMessage("));
		client->print(String(_deviceId));
		client->print(F(", "));
		client->print(String(_ctrl_elem->id));
		client->print(F(", "));
		client->print(String(I) + F("); \">"));
		client->print(((Group_CtrlElem*)_ctrl_elem)->GetMember(I));
		client->println(F("</button>"));
	}
	for (uint8_t I = 0; I < ((Group_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		client->print(F("</div>"));
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateButtonGroup"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateSetButton(WiFiClient *client, int _deviceId, int _cmdId) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateSetButton"));
#endif // DEBUG
	client->print(F("				<button class=\"button small\" onclick=\"SendMessage("));
	client->print(String(_deviceId));
	client->print(F(", "));
	client->print(String(_cmdId));
	client->println(F(");\">Set</button>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateSetButton"));
#endif // DEBUG
}

