#include "WebSocket_Wifi_Device_Driver.h"
#include <ArduinoJson.h>
#include <FS.h>

ESP8266WebServer *WebSocket_Wifi_Device_Driver::server;
WebSocketsServer *WebSocket_Wifi_Device_Driver::webSocket;
event_msg WebSocket_Wifi_Device_Driver::__event_msg;



String WebSocket_Wifi_Device_Driver::Json_GetvalueFromKey(String _text, String _key) {
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
	return value;
}

void WebSocket_Wifi_Device_Driver::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
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
		Serial.printf("[%u] get Text: %s", num, payload);

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

		break;
	}

}

WebSocket_Wifi_Device_Driver::WebSocket_Wifi_Device_Driver(Module_Driver * module, String _ssid, String _password, Led_Device_Driver * _statusLED, uint8_t priority) :
	Wifi_Device_Driver(module, _ssid, _password, _statusLED, priority)
{
	__DriverType = WEBSOCKET_WIFI_DEVICE_DRIVER_TYPE;
	server = nullptr;
	webSocket = nullptr;

	__event_msg._filled = false;
	accuracy_delay = 2000;
	accuracy_delta = 0;
}




void WebSocket_Wifi_Device_Driver::UpdateControls() {

}


void WebSocket_Wifi_Device_Driver::UpdateComm(uint32_t deltaTime) {
	accuracy_delta += deltaTime;
	if (accuracy_delta > accuracy_delay) {
		accuracy_delta = 0;
		if (__descriptor_list->count > 0)
			for (uint8_t I = 0; I < __descriptor_list->count; I++)
				if (__descriptor_list->GetElemByIndex(I)->published)
					for (uint8_t J = 0; J < __descriptor_list->GetElemByIndex(I)->ctrl_count; J++)
						if ((__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->type == value) || (__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->type == edtvalue)) {
							String json_str;
							StaticJsonBuffer<200> jsonBuffer;
							JsonObject& root = jsonBuffer.createObject();
							root["_deviceId"] = String(__descriptor_list->GetElemByIndex(I)->id);
							root["_cmdId"] = String(__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->id);
							root["_value"] = ((Atomic<String>*)__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->GetAtomicByIndex(0))->ValueToString();
							root["_unit"] = String(((Atomic<String>*)__descriptor_list->GetElemByIndex(I)->GetCtrlElemByIndex(J)->GetAtomicByIndex(0))->unit);
							root.printTo(json_str);
							webSocket->sendTXT(0, json_str.c_str());
						}
	}

	webSocket->loop();
	server->handleClient();

	if (__event_msg._filled) {
		CommunicationMessage* message = new CommunicationMessage(__event_msg._deviceId, __event_msg._cmdId, __event_msg._value);
		if (!parentModule->SendAsyncThreadMessage(message))
		{
			Serial.println(">> message buffer overflow <<");
		}
		__event_msg._filled = false;
	}


}

void WebSocket_Wifi_Device_Driver::InitComm() {
	if (server != nullptr)
		server->~ESP8266WebServer();
	server = nullptr,
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
		Serial.printf("before all heap size: %u\n", ESP.getFreeHeap());
		WiFiClient client = server->client();
		//server->send(200, "text/html", "<html><head> <script> var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']); connection.onopen = function() { connection.send('Connect ' + new Date()); }; connection.onerror = function(error) { console.log('WebSocket Error ', error); }; connection.onmessage = function(e) { if (e.data.indexOf('subProtocol') == -1) document.getElementById('response').innerHTML = e.data + '<br/>'; }; function sendMessage(msg) { connection.send(msg); } function sendRGB() { var r = parseInt(document.getElementById('r').value).toString(16); var g = parseInt(document.getElementById('g').value).toString(16); var b = parseInt(document.getElementById('b').value).toString(16); if (r.length < 2) { r = '0' + r; } if (g.length < 2) { g = '0' + g; } if (b.length < 2) { b = '0' + b; } var rgb = '#' + r + g + b; console.log('RGB: ' + rgb); connection.send(rgb); } </script></head><body>LED Control: <br/> <br/>R: <input id='r' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>G: <input id='g' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>B: <input id='b' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/> <br/> <br/> <div id='response'> My Name is jWebSocketServer </div></body></html>");
		client.println("<!DOCTYPE html>");
		client.println("<html>");
		SendHeader(&client);
		client.println("<body>");

		if (__isAP) {
			String thisName = __descriptor->name;
			for (uint8_t i = 0; i < __descriptor_list->count; i++)
				if (!thisName.equals(__descriptor_list->GetElemByIndex(i)->name)) {
					if (__fallbackAP)
						__descriptor_list->GetElemByIndex(i)->published = false;
				}
				else
					__descriptor_list->GetElemByIndex(i)->published = true;
		}

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
		Serial.printf("after all heap size: %u\n", ESP.getFreeHeap());
	});

	
	server->begin();


	if (webSocket != nullptr)
		webSocket->~WebSocketsServer();
	webSocket = nullptr,
		webSocket = new WebSocketsServer(81);

	if (MDNS.begin("esp8266")) {
		Serial.println("MDNS responder started");
	}

	webSocket->onEvent(webSocketEvent);
	webSocket->begin();


}

void WebSocket_Wifi_Device_Driver::GenerateAndSendHtml(WiFiClient *_client) {
	_client->println("<!DOCTYPE html>");
	_client->println("<html>");
	SendHeader(_client);
	_client->println("<body>");

	if (__descriptor_list->count > 0) {
		_client->println("<div class=\"container\">");
		_client->println("<h2 class=\"head-line\">Bilderrahmen Steuerung</h2>");
		GenerateNav(_client, __descriptor_list);
		bool first = true;
		for (uint8_t I = 0; I < __descriptor_list->count; I++) {
			if (__descriptor_list->GetElemByIndex(I)->published) {
				_client->print("<div id=\"tab" + String(__descriptor_list->GetElemByIndex(I)->name) + "\" class=\"tabcontent\"");
				if (first) {
					_client->print(" style=\"display:block\"");
					first = false;
				}
				_client->println(">");
				GenerateTab(_client, __descriptor_list->GetElemByIndex(I));
				_client->println("</div>");
				yield();
			}
		}
		_client->println("</div>");
	}
	_client->println("</body>");
	_client->println("</html>");
	_client->stop();
}

void WebSocket_Wifi_Device_Driver::GenerateNav(WiFiClient *client, Descriptor_List *_descriptor_list) {
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
}

void WebSocket_Wifi_Device_Driver::GenerateTab(WiFiClient *client, Descriptor *_descriptor) {
	client->println("	<div class=\"simple-text\">");
	client->println("		<p>");
	client->print("		<strong>");
	client->print(String(_descriptor->descr) + "");
	client->print("</strong>");
	client->println("		</p>");
	client->println("		<div class=\"head-line small\">Features</div>");
	client->println("		<p>");
	client->println("			<ul>");
	for (uint8_t J = 0; J < _descriptor->ctrl_count; J++)
		client->println("				<li>" + String(_descriptor->GetCtrlElemByIndex(J)->descr) + "</li>");
	client->println("			</ul>");
	client->println("		</p>");
	client->println("	</div>");
	client->println("	<div class=\"head-line small\">Steuerung</div>");
	client->println("	<br>");
	for (uint8_t I = 0; I < _descriptor->ctrl_count; I++) {
		GenerateForm(client, _descriptor->id, _descriptor->GetCtrlElemByIndex(I));
	}
	client->println("	<br>");
}

void WebSocket_Wifi_Device_Driver::GenerateForm(WiFiClient *client, int _deviceId, Ctrl_Elem* _ctrl_elem) {

	client->println("		<div class = \"form-item\">");
	if (_ctrl_elem->type == value)
		client->println("			<div class = \"append w35 w100-sm\">");
	else
		client->println("			<div class = \"append w45 w100-sm\">");
	switch (_ctrl_elem->type)
	{
	case text:
	{
		GenerateInput(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		break;
	}
	case pass:
	{
		GenerateInput(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		break;
	}
	case select:
	{
		GenerateSelect(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		break;
	}
	case color:
	{
		GenerateColor(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
		break;
	}
	case button:
	{
		GenerateButton(client, _deviceId, _ctrl_elem);
		break;
	}
	case value:
	{
		GenerateInput(client, _deviceId, _ctrl_elem);
		break;
	}
	case edtvalue:
	{
		GenerateInput(client, _deviceId, _ctrl_elem);
		GenerateSetButton(client, _deviceId, _ctrl_elem->id);
	}
	default:
		break;
	}

	client->println("			</div>");
	GenerateDecending(client, _ctrl_elem);
	client->println("		</div>");
}

void WebSocket_Wifi_Device_Driver::GenerateDecending(WiFiClient *client, Ctrl_Elem* _ctrl_elem) {
	client->println("			<div class=\"desc\">");
	switch (_ctrl_elem->type)
	{
	case text:
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
		client->print("				Pick a ");
		break;
	}
	case color:
	{
		client->print("				Pick a ");
		break;
	}
	case button:
	{
		client->print("				" + String(_ctrl_elem->descr));
		break;
	}
	}
	client->println(String(_ctrl_elem->name));
	client->println("			</div>");
}


void WebSocket_Wifi_Device_Driver::GenerateColor(WiFiClient *client, int _deviceId, Ctrl_Elem* _ctrl_elem) {

	client->print("			<input ");
	client->print("value=\"");

	for (uint8_t I = 0; I < _ctrl_elem->atomic_count; I++) {
		char buf[12];
		client->print(String(itoa(((Atomic<int>*)_ctrl_elem->GetAtomicByIndex(I))->value, buf, 16)));
	}
	client->print("\" class=\"small ");
	client->print("jscolor{ mode:\'HSV\',width:225, height:130, position:\'top\', borderColor :\'#e3e3e3\', insetColor : \'#666\', backgroundColor : \'#666\'}\" ");
	client->println("id=\"" + String(_deviceId) + "_" + String(_ctrl_elem->id) + "\">");
}


void WebSocket_Wifi_Device_Driver::GenerateSelect(WiFiClient *client, int _deviceId, Ctrl_Elem* _ctrl_elem) {
	client->print("				<select class=\"small\" ");
	client->println("id=\"" + String(_deviceId) + "_" + String(_ctrl_elem->id) + "\">");
	GenerateOption(client, _ctrl_elem);
	client->println("				</select>");
}

void WebSocket_Wifi_Device_Driver::GenerateOption(WiFiClient *client, Ctrl_Elem* _ctrl_elem) {
	for (uint8_t I = 0; I < _ctrl_elem->atomic_count; I++) {
		client->print("					<option value=\"");
		client->print(String(((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->id));
		client->print("\">");
		client->print(((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->ValueToString());
		client->print(String(((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->unit));
		client->println("</option>");
	}
}


void WebSocket_Wifi_Device_Driver::GenerateInput(WiFiClient *client, int _deviceId, Ctrl_Elem* _ctrl_elem) {
	for (uint8_t I = 0; I < _ctrl_elem->atomic_count; I++) {
		if (_ctrl_elem->type == pass)
			client->print("					<input type=\"password\" class=\"small\" id=\"");
		else
			client->print("					<input type=\"text\" class=\"small\" id=\"");
		client->print(String(_deviceId) + "_" + String(_ctrl_elem->id));
		client->print("\"  placeholder=\"");
		client->print(((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->ValueToString());
		client->print(" " + ((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->unit);
		if (_ctrl_elem->type == value) {
			client->println("\" disabled>");
		}
		else {
			client->println("\">");
		}
	}
}

void WebSocket_Wifi_Device_Driver::GenerateButton(WiFiClient *client, int _deviceId, Ctrl_Elem* _ctrl_elem) {

	client->print("				<button class=\"button small\" id=\"");
	client->print(String(_deviceId) + "_" + String(_ctrl_elem->id));
	client->print("\" ");
	client->print("value=\"0\" ");
	client->print("onclick=\"SendMessage(");
	client->print(String(_deviceId));
	client->print(", ");
	client->print(String(_ctrl_elem->id));
	client->print(");\">");
	client->print(_ctrl_elem->name);
	client->println("</button>");
}

void WebSocket_Wifi_Device_Driver::GenerateSetButton(WiFiClient *client, int _deviceId, int _cmdId) {
	client->print("				<button class=\"button small\" onclick=\"SendMessage(");
	client->print(String(_deviceId));
	client->print(", ");
	client->print(String(_cmdId));
	client->println(");\">Set</button>");
}
