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
	Serial.println("Start WebSocket_Wifi_Device_Driver::Json_GetvalueFromKey");
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
		Serial.println("Failed to parse json file");
	}
	value = root[_key].as<String>();
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::Json_GetvalueFromKey");
#endif // DEBUG
	return value;
}


void WebSocket_Wifi_Device_Driver::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::webSocketEvent");
#endif // DEBUG
	switch (type) {
	case WStype_DISCONNECTED:
		Serial.printf("[%u] Disconnected!", num);
		break;
	case WStype_CONNECTED: {
		IPAddress ip = webSocket->remoteIP(num);
		Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s", num, ip[0], ip[1], ip[2], ip[3], payload);

		// send message to client
		webSocket->sendTXT(num, "Connected");
	}
						   break;
	case WStype_TEXT:
#ifdef DEBUG
		Serial.println("Start WebSocket_Wifi_Device_Driver::webSocketEvent WStype_TEXT");
#endif // DEBUG
		Serial.printf("[%u] get Text: %s", num, payload);
		Serial.println();
		if (payload[0] == '{') {
			// we get RGB data

			// decode rgb data
			//uint32_t rgb = (uint32_t)strtol((const char *)&payload[1], NULL, 16);
			String json_string = String((const char *)&payload[0]);

			int deviceId = atoi(Json_GetvalueFromKey(json_string, "deviceId").c_str());
			int cmdId = atoi(Json_GetvalueFromKey(json_string, "cmdId").c_str());
			String value = Json_GetvalueFromKey(json_string, "value");
			if (__event_msg._filled == false) {
				__event_msg._deviceId = deviceId;
				__event_msg._cmdId = cmdId;
				__event_msg._value = value;
				__event_msg._filled = true;
			}
			delay(10);
		}
#ifdef DEBUG
		Serial.println("Ende WebSocket_Wifi_Device_Driver::webSocketEvent WStype_TEXT");
#endif // DEBUG
		break;
	}
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::webSocketEvent");
#endif // DEBUG
}

WebSocket_Wifi_Device_Driver::WebSocket_Wifi_Device_Driver(Module_Driver * module, uint8_t priority) :
	Device_Driver(module, priority)
{
#ifdef DEBUG
	Serial.print("Start WebSocket_Wifi_Device_Driver with ID: ");
	Serial.println(this->DriverId);
#endif // DEBUG
	__DriverType = WEBSOCKET_WIFI_DEVICE_DRIVER_TYPE;
	server = nullptr;
	webSocket = nullptr;
	__event_msg._filled = false;
	SetTimerDelay(1000);
#ifdef DEBUG
	Serial.println("Ende Start WebSocket_Wifi_Device_Driver");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::TimerTick() {
	if (__isConnected)
		if (__descriptor_list->count > 0)
			for (uint8_t I = 0; I < __descriptor_list->count; I++)
				if (__descriptor_list->GetElemByIndex(I)->published)
					for (uint8_t J = 0; J < __descriptor_list->GetElemByIndex(I)->ctrl_count; J++)
						if ((__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->type == value) ||
							(__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->type == zeit))
						{
							String json_str;
							StaticJsonBuffer<200> jsonBuffer;
							JsonObject& root = jsonBuffer.createObject();
							root["_deviceId"] = String(__descriptor_list->GetElemByIndex(I)->id);
							root["_cmdId"] = String(__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->id);
							root["_value"] = __descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->ToString();
							root["_unit"] = String(__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->unit);
							root.printTo(json_str);
							webSocket->sendTXT(0, json_str.c_str());
						}
}

void WebSocket_Wifi_Device_Driver::DoUpdate(uint32_t deltaTime) {
#ifdef DEBUG
	//Serial.println("Start WebSocket_Wifi_Device_Driver::UpdateComm");
#endif // DEBUG
	if (__isConnected) {
		webSocket->loop();
		server->handleClient();

		if (__event_msg._filled) {
			ExternMessage* message = new ExternMessage(__event_msg._deviceId, __event_msg._cmdId, __event_msg._value);
#ifdef DEBUG
			Serial.println("WebSocket_Wifi_Device_Driver::UpdateComm SendAsyncTaskMessage");
#endif // DEBUG
			if (!__parentModule->SendAsyncTaskMessage(message))
			{
				Serial.println(">> message buffer overflow <<");
			}
			__event_msg._filled = false;
		}
	}
#ifdef DEBUG
//Serial.println("Ende WebSocket_Wifi_Device_Driver::UpdateComm");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::OnNotifyConnectionLost()
{
	if (server != nullptr) {
		server->~ESP8266WebServer();
		server = nullptr;
}

	if (webSocket != nullptr) {
		webSocket->~WebSocketsServer();
		webSocket = nullptr;
	}
}

void WebSocket_Wifi_Device_Driver::OnNotifyOnline()
{
	InitializeServices();
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
	InitializeServices();
}


void WebSocket_Wifi_Device_Driver::InitializeServices()
{
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::InitializeServices");
#endif // DEBUG
	if (server != nullptr) {
		server->~ESP8266WebServer();
		server = nullptr;
	}
	server = new ESP8266WebServer(80);

	// handle index
	//server->serveStatic("/dist/js/jscolor.min.js", SPIFFS, "/dist/js/jscolor.min.js");
	//server->serveStatic("/dist/css/kube.min.css", SPIFFS, "/dist/css/kube.min.css");

	server->on("/dist/css/kube.min.css", [&]() {
		File file = SPIFFS.open("/dist/css/kube.min.css", "r");
		server->streamFile(file, "text/css");
		file.close();
	});


	server->on("/dist/js/jscolor.min.js", [&]() {
		File file = SPIFFS.open("/dist/js/jscolor.min.js", "r");
		server->streamFile(file, "application/javascript");
		file.close();
	});

	server->on("/", [&]() {
#ifdef DEBUG
		Serial.println("Start WebSocket_Wifi_Device_Driver::InitializeServices server->on");
		Serial.printf("before all heap size: %u\n", ESP.getFreeHeap());
#endif // DEBUG
		WiFiClient client = server->client();
		//server->send(200, "text/html", "<html><head> <script> var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']); connection.onopen = function() { connection.send('Connect ' + new Date()); }; connection.onerror = function(error) { console.log('WebSocket Error ', error); }; connection.onmessage = function(e) { if (e.data.indexOf('subProtocol') == -1) document.getElementById('response').innerHTML = e.data + '<br/>'; }; function sendMessage(msg) { connection.send(msg); } function sendRGB() { var r = parseInt(document.getElementById('r').value).toString(16); var g = parseInt(document.getElementById('g').value).toString(16); var b = parseInt(document.getElementById('b').value).toString(16); if (r.length < 2) { r = '0' + r; } if (g.length < 2) { g = '0' + g; } if (b.length < 2) { b = '0' + b; } var rgb = '#' + r + g + b; console.log('RGB: ' + rgb); connection.send(rgb); } </script></head><body>LED Control: <br/> <br/>R: <input id='r' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>G: <input id='g' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>B: <input id='b' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/> <br/> <br/> <div id='response'> My Name is jWebSocketServer </div></body></html>");
		client.println("<!DOCTYPE html>");
		client.println("<html>");
		SendHeader(&client);
		client.println("<body>");
		if (__descriptor_list->count > 0) {
			client.println("<div class=\"container\">");
			client.println("<h2 class=\"head-line\">Bilderrahmen Steuerung</h2>");
			GenerateNav(&client, __descriptor_list);
			bool first = true;
			for (uint8_t I = 0; I < __descriptor_list->count; I++) {
				if (__descriptor_list->GetElemByIndex(I)->published) {
					client.print("<div id=\"tab" + String(__descriptor_list->GetElemByIndex(I)->name) + "\" class=\"tabcontent\"");
					if (first) {
						client.print(" style=\"display:block\"");
						first = false;
					}
					client.println(">");
					GenerateTab(&client, __descriptor_list->GetElemByIndex(I));
					client.println("</div>");
					yield();
				}
			}
			client.println("</div>");
		}
		client.println("</body>");
		client.println("</html>");
		client.stop();
#ifdef DEBUG
		Serial.printf("after all heap size: %u\n", ESP.getFreeHeap());
		Serial.println("Ende WebSocket_Wifi_Device_Driver::InitializeServices server->on");
#endif // DEBUG
	});


	server->begin();


	if (webSocket != nullptr) {
		webSocket->~WebSocketsServer();
		webSocket = nullptr;
	}
	webSocket = new WebSocketsServer(81);

	if (MDNS.begin("esp8266")) {
		Serial.println("MDNS responder started");
	}

	webSocket->onEvent(webSocketEvent);
	webSocket->begin();

#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::InitComm");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateNav(WiFiClient *client, Descriptor_List *_descriptor_list) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateNav");
#endif // DEBUG
	client->println("		<div class=\"tab\">");
	bool first = true;
	for (uint8_t I = 0; I < __descriptor_list->count; I++)
		if (__descriptor_list->GetElemByIndex(I)->published) {
			client->println("			<button class = \"tablinks");
			if (first) {
				client->print(" active");
				first = false;
		}
			client->print("\" ");
			client->print("onclick=\"openTab(event, 'tab" + String(__descriptor_list->GetElemByIndex(I)->name) + "')\">");
			client->print(String(__descriptor_list->GetElemByIndex(I)->name));
			client->println("</button>");
}
	client->println("		</div>");

#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateNav");
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::OnBuild_Descriptor() {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::Build_Descriptor");
#endif // DEBUG
	__descriptor->name = F("WebSocekt Driver");
	__descriptor->descr = F("Broadcasts the website of all controls");
	__descriptor->published = false;
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::Build_Descriptor");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateTab(WiFiClient *client, Descriptor *_descriptor) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateTab");
#endif // DEBUG
	client->println("	<div class=\"simple-text\">");
	client->println("		<p>");
	client->print("		<strong>");
	client->print(String(_descriptor->descr) + "");
	client->print("</strong>");
	client->println("		</p>");
	client->println(F("		<div class=\"head-line small\">Features</div>"));
	client->println("		<p>");
	client->println("			<ul>");
	for (uint8_t J = 0; J < _descriptor->ctrl_count; J++)
		client->println("				<li>" + String(_descriptor->GetCtrlElemByIndex(J)->description) + "</li>");
	client->println("			</ul>");
	client->println("		</p>");
	client->println("	</div>");
	client->println(F("	<div class=\"head-line small\">Steuerung</div>"));
	client->println("	<br>");
	for (uint8_t I = 0; I < _descriptor->ctrl_count; I++) {
		GenerateForm(client, _descriptor->id, _descriptor->GetCtrlElemByIndex(I));
	}
	client->println("	<br>");
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateTab");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateForm(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateForm");
#endif // DEBUG
	client->println("		<div class = \"form-item\">");
	switch (_ctrl_elem->type)
	{
	case input:
	{
		client->println("			<div class = \"append w45 w100-sm\">");
		GenerateInput(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println("			</div>");
		break;
	}
	case pass:
	{
		client->println("			<div class = \"append w45 w100-sm\">");
		GenerateInput(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println("			</div>");
		break;
	}
	case select:
	{
		client->println("			<div class = \"append w45 w100-sm\">");
		GenerateSelect(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println("			</div>");
		break;
	}

	case color:
	{
		client->println("			<div class = \"append w45 w100-sm\">");
		GenerateColor(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println("			</div>");
		break;
	}
	case group:
	{
		GenerateButtonGroup(client, _deviceId, _ctrl_elem);
		break;
	}
	case value:
	{
		if ((bool)((Value_CtrlElem*)_ctrl_elem)->IsEditable())
			client->println("			<div class = \"append w45 w100-sm\">");
		else
			client->println("			<div class = \"append w35 w100-sm\">");
		GenerateInput(client, _deviceId, _ctrl_elem);
		if ((bool)((Value_CtrlElem*)_ctrl_elem)->IsEditable())
			GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		client->println("			</div>");
		break;
	}
	case zeit:
	{
		client->println("			<div class = \"append w35 w100-sm\">");
		GenerateInput(client, _deviceId, _ctrl_elem);
		client->println("			</div>");
		break;
	}
	default:
		break;
}
	GenerateDecending(client, _ctrl_elem);
	client->println("		</div>");
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateForm");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateDecending(WiFiClient *client, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateDecending");
#endif // DEBUG
	client->println("			<div class=\"desc\">");
	switch (_ctrl_elem->type)
	{
	case input:
	{
		client->print("				Set ");
		break;
	}

	case pass:
	{
		client->print("				Set ");
		break;
	}
	case select:
	{
		client->print("				Select ");
		break;
	}
	case color:
	{
		client->print("				Pick a ");
		break;
	}
	case group:
	{
		break;
	}
}
	client->println(String(_ctrl_elem->name));
	client->println("			</div>");
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateDecending");
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::GenerateColor(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateColor");
#endif // DEBUG
	client->print("			<input ");
	client->print("value=\"");
	client->print(reinterpret_cast<Color_CtrlElem*>(_ctrl_elem)->ToString());
	client->print("\" class=\"small ");
	client->print("jscolor{ mode:\'HSV\',width:225, height:130, position:\'top\', borderColor :\'#e3e3e3\', insetColor : \'#666\', backgroundColor : \'#666\'}\" ");
	client->println("id=\"" + String(_deviceId) + "_" + String(_ctrl_elem->id) + "\">");
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateColor");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateSelect(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateSelect");
#endif // DEBUG
	client->print("				<select class=\"small\" ");
	client->println("id=\"" + String(_deviceId) + "_" + String(_ctrl_elem->id) + "\">");
	GenerateMultiOption(client, _ctrl_elem);
	client->println("				</select>");
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateSelect");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateMultiOption(WiFiClient *client, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateMultiOption");
#endif // DEBUG
	for (uint8_t I = 0; I < ((Select_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		client->print("					<option");
		int t = ((Select_CtrlElem*)_ctrl_elem)->ToString().toInt();
		if (((Select_CtrlElem*)_ctrl_elem)->ToString().toInt() == I)
			client->print(" selected ");
		client->print("	value=\"");
		client->print(String(I));
		client->print("\">");
		client->print(((Select_CtrlElem*)_ctrl_elem)->GetMember(I));
		client->print(_ctrl_elem->unit);
		client->println("</option>");
	}
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateMultiOption");
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::GenerateInput(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateInput");
#endif // DEBUG
	if (_ctrl_elem->type == pass)
		client->print("					<input type=\"password\" class=\"small\" id=\"");
	else
		client->print("					<input type=\"text\" class=\"small\" id=\"");
	client->print(String(_deviceId) + "_" + String(_ctrl_elem->id));
	if (_ctrl_elem->type == pass)
		client->print("\"  value=\"");
	else
		client->print("\"  placeholder=\"");
	client->print(_ctrl_elem->ToString());
	if (((_ctrl_elem->type == value) || (_ctrl_elem->type == zeit)) && (!((Value_CtrlElem*)_ctrl_elem)->IsEditable())) {
		client->println("\" disabled>");
	}
	else {
		client->println("\">");
	}
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateInput");
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateButtonGroup(WiFiClient *client, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateButtonGroup");
#endif // DEBUG
	for (uint8_t I = 0; I < ((Group_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		client->print("<div class=\"");
		if (((Group_CtrlElem*)_ctrl_elem)->MembersCount() > 1) {
			if (I > 0) {
				client->print(" append ");
			}
			if (I < (((Group_CtrlElem*)_ctrl_elem)->MembersCount() - 1)) {
				client->print(" prepend ");
			}
			}
		client->print("\">");
		client->print("				<button class=\"button small\" ");
		client->print("onclick=\"SendMessage(");
		client->print(String(_deviceId));
		client->print(", ");
		client->print(String(_ctrl_elem->id));
		client->print(", ");
		client->print(String(I) + "); \">");
		client->print(((Group_CtrlElem*)_ctrl_elem)->GetMember(I));
		client->println("</button>");
		}
	for (uint8_t I = 0; I < ((Group_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		client->print("</div>");
	}
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateButtonGroup");
#endif // DEBUG
	}

void WebSocket_Wifi_Device_Driver::GenerateSetButton(WiFiClient *client, int _deviceId, int _cmdId) {
#ifdef DEBUG
	Serial.println("Start WebSocket_Wifi_Device_Driver::GenerateSetButton");
#endif // DEBUG
	client->print("				<button class=\"button small\" onclick=\"SendMessage(");
	client->print(String(_deviceId));
	client->print(", ");
	client->print(String(_cmdId));
	client->println(");\">Set</button>");
#ifdef DEBUG
	Serial.println("Ende WebSocket_Wifi_Device_Driver::GenerateSetButton");
#endif // DEBUG
}

