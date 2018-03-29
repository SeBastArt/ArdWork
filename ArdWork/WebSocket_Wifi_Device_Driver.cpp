#include "WebSocket_Wifi_Device_Driver.h"
#include <ArduinoJson.h>
#include <FS.h>

//#define DEBUG

AsyncWebServer *WebSocket_Wifi_Device_Driver::server;
AsyncWebSocket *WebSocket_Wifi_Device_Driver::webSocket;
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


void WebSocket_Wifi_Device_Driver::webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::webSocketEvent"));
#endif // DEBUG
	switch (type) {
	case WS_EVT_DISCONNECT: {
		Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
		break;
	}
	case WS_EVT_CONNECT: {
		Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());

		// send message to client
		client->printf("Hello Client %u :)", client->id());
		client->ping();
		break;
	}
	case WS_EVT_PONG: {
#ifdef DEBUG
		Serial.println(F("Start WebSocket_Wifi_Device_Driver::webSocketEvent WS_EVT_PONG"));
#endif // DEBUG
		Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
						   break;
	}
	case WS_EVT_DATA: {
#ifdef DEBUG
		Serial.println(F("Start WebSocket_Wifi_Device_Driver::webSocketEvent WS_EVT_DATA"));
#endif // DEBUG
		AwsFrameInfo * info = (AwsFrameInfo*)arg;
		String json_string = "";
		if (info->final && info->index == 0 && info->len == len) {
			//the whole message is in a single frame and we got all of it's data
			Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

			if (info->opcode == WS_TEXT) {
				for (size_t i = 0; i < info->len; i++) {
					json_string += (char)data[i];
				}
			}
			else {
				char buff[3];
				for (size_t i = 0; i < info->len; i++) {
					sprintf(buff, "%02x ", (uint8_t)data[i]);
					json_string += buff;
				}
			}
			Serial.printf("%s\n", json_string.c_str());

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
		break;
	}
#ifdef DEBUG
	 Serial.println(F("Ende WebSocket_Wifi_Device_Driver::webSocketEvent WS_EVT_DATA"));
#endif // DEBUG
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::webSocketEvent"));
#endif // DEBUG
}

WebSocket_Wifi_Device_Driver::WebSocket_Wifi_Device_Driver(Module_Driver* module, uint8_t priority) : Device_Driver(module, priority) {
#ifdef DEBUG
	Serial.print(F("Start WebSocket_Wifi_Device_Driver with ID: "));
	Serial.println(this->DriverId);
#endif // DEBUG
	__DriverType = WEBSOCKET_WIFI_DEVICE_DRIVER_TYPE;
	server = nullptr;
	webSocket = nullptr;
	__event_msg._filled = false;
	SetTimerDelay(1000);
	//webSocket = new AsyncWebSocket("/ws");
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
				webSocket->text(0, json_str.c_str());
			}
		}
	}
}

void WebSocket_Wifi_Device_Driver::DoUpdate(uint32_t deltaTime) {
#ifdef DEBUG
	//Serial.println(F("Start WebSocket_Wifi_Device_Driver::UpdateComm"));
#endif // DEBUG
	if (__isConnected) {
		//server->handleClient();

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
		server->~AsyncWebServer();
		server = nullptr;
	}

	if (webSocket != nullptr) {
		webSocket->~AsyncWebSocket();
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
		server->~AsyncWebServer();
		server = nullptr;
	}
	server = new AsyncWebServer(80);

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - Server started"));
#endif // DEBUG

	server->on(String(F("/dist/css/kube.min.css")).c_str(), HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/dist/css/kube.min.css");
	});
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - CSSs Loaded"));
#endif // DEBUG


	server->on(String(F("/dist/js/jscolor.min.js")).c_str(), HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/dist/js/jscolor.min.js");
	});

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - JavaScripts Loaded"));
#endif // DEBUG

	server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
#ifdef DEBUG
		Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices server->on"));
		Serial.printf("before all heap size: %u\n", ESP.getFreeHeap());
#endif // DEBUG
		AsyncResponseStream *response = request->beginResponseStream("text/html");
#ifdef DEBUG
		Serial.println(F("AsyncResponseStream created"));
#endif // DEBUG
		response->println(F("<!DOCTYPE html>"));
		response->println(F("<html>"));
		SendHeader(response);
		response->println(F("<body>"));
		if (__descriptor_list->count > 0) {
			response->println(F("<div class=\"container\">"));
			response->print(F("<h2 class=\"head-line\">")); response->print(String(__descriptor_list->projectname)); response->println(F("</h2>"));
			GenerateNav(response, __descriptor_list);
			bool first = true;
			for (uint8_t I = 0; I < __descriptor_list->count; I++) {
				if (__descriptor_list->GetElemByIndex(I)->published) {
					response->print("<div id=\"tab" + String(__descriptor_list->GetElemByIndex(I)->name) + "\" class=\"tabcontent\"");
					if (first) {
						response->print(" style=\"display:block\"");
						first = false;
					}
					response->println(F(">"));
					GenerateTab(response, __descriptor_list->GetElemByIndex(I));
					response->println(F("</div>"));
					//yield();
				}
			}
			response->println(F("</div>"));
		}
		response->println(F("</body>"));
		response->println(F("</html>"));
		Serial.println(F("AsyncResponseStream send"));
		request->send(response);
		Serial.println(F("AsyncResponseStream send Done"));
#ifdef DEBUG
		Serial.printf("after all heap size: %u\n", ESP.getFreeHeap());
		Serial.println(F("Ende WebSocket_Wifi_Device_Driver::InitializeServices server->on"));
#endif // DEBUG
	});

#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - dynamic content Loaded"));
#endif // DEBUG

	if (webSocket != nullptr) {
		webSocket->~AsyncWebSocket();
		webSocket = nullptr;
	}
	webSocket = new AsyncWebSocket("/ws");
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - websocket created"));
#endif // DEBUG
	webSocket->onEvent(webSocketEvent);
	server->addHandler(webSocket);
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - websocket started"));
#endif // DEBUG
	server->begin();
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::InitializeServices - server started"));
#endif // DEBUG
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::InitComm"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateNav(AsyncResponseStream *response, Descriptor_List *_descriptor_list) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateNav"));
#endif // DEBUG
	response->println(F("		<div class=\"tab\">"));
	bool first = true;
	for (uint8_t I = 0; I < __descriptor_list->count; I++)
		if (__descriptor_list->GetElemByIndex(I)->published) {
			response->print(F("			<button class = \"tablinks"));
			if (first) {
				response->print(F(" active"));
				first = false;
			}
			response->print(F("\" "));
			response->print(String(F("onclick=\"openTab(event, 'tab")) + String(__descriptor_list->GetElemByIndex(I)->name) + "')\">");
			response->print(String(__descriptor_list->GetElemByIndex(I)->name));
			response->println(F("</button>"));
		}
	response->println(F("		</div>"));

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

void WebSocket_Wifi_Device_Driver::GenerateTab(AsyncResponseStream *response, Descriptor *_descriptor) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateTab"));
#endif // DEBUG
	response->println(F("	<div class=\"simple-text\">"));
	response->println(F("		<p>"));
	response->print(F("		<strong>"));
	response->print(String(_descriptor->descr) + "");
	response->println(F("</strong>"));
	response->println(F("		</p>"));
	response->println(F("			</div>"));
	response->println(F("	<div class=\"head-line small\">Control Panel</div>"));
	response->println(F("	<br>"));
	for (uint8_t I = 0; I < _descriptor->ctrl_count; I++) {
		GenerateForm(response, _descriptor->id, _descriptor->GetCtrlElemByIndex(I));
	}
	response->println(F("	<br>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateTab"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateForm(AsyncResponseStream *response, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateForm"));
#endif // DEBUG
	response->println(F("		<div class = \"form-item\">"));
	switch (_ctrl_elem->type)
	{
	case input:
	{
		response->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateInput(response, _deviceId, _ctrl_elem);
		GenerateSetButton(response, _deviceId, _ctrl_elem->id);
		response->println(F("			</div>"));
		break;
	}
	case pass:
	{
		response->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateInput(response, _deviceId, _ctrl_elem);
		GenerateSetButton(response, _deviceId, _ctrl_elem->id);
		response->println(F("			</div>"));
		break;
	}
	case select:
	{
		response->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateSelect(response, _deviceId, _ctrl_elem);
		GenerateSetButton(response, _deviceId, _ctrl_elem->id);
		response->println(F("			</div>"));
		break;
	}

	case color:
	{
		response->println(F("			<div class = \"append w45 w100-sm\">"));
		GenerateColor(response, _deviceId, _ctrl_elem);
		GenerateSetButton(response, _deviceId, _ctrl_elem->id);
		response->println(F("			</div>"));
		break;
	}
	case group:
	{
		GenerateButtonGroup(response, _deviceId, _ctrl_elem);
		break;
	}
	case value:
	{
		if (_ctrl_elem->editable)
			response->println(F("			<div class = \"append w45 w100-sm\">"));
		else
			response->println(F("			<div class = \"append w35 w100-sm\">"));
		GenerateInput(response, _deviceId, _ctrl_elem);
		if (_ctrl_elem->editable)
			GenerateSetButton(response, _deviceId, _ctrl_elem->id);
		response->println(F("			</div>"));
		break;
	}
	case zeit:
	{
		response->println(F("			<div class = \"append w35 w100-sm\">"));
		GenerateInput(response, _deviceId, _ctrl_elem);
		response->println(F("			</div>"));
		break;
	}
	default:
		break;
	}
	GenerateDecending(response, _ctrl_elem);
	response->println(F("		</div>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateForm"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateDecending(AsyncResponseStream *response, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateDecending"));
#endif // DEBUG
	response->println(F("			<div class=\"desc\">"));
	switch (_ctrl_elem->type)
	{
	case input:
	{
		response->print(F("				Set "));
		break;
	}

	case pass:
	{
		response->print(F("				Set "));
		break;
	}
	case select:
	{
		response->print(F("				Select "));
		break;
	}
	case color:
	{
		response->print(F("				Pick a "));
		break;
	}
	case group:
	{
		break;
	}
	}
	response->println(String(_ctrl_elem->name));
	if (_ctrl_elem->type == value)
		response->print(" [" + String(_ctrl_elem->unit) + "]");
	response->println(F("			</div>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateDecending"));
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::GenerateColor(AsyncResponseStream *response, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateColor"));
#endif // DEBUG
	response->print(F("			<input "));
	response->print(F("value=\""));
	response->print(reinterpret_cast<Color_CtrlElem*>(_ctrl_elem)->ToString());
	response->print(F("\" class=\"small "));
	response->print(F("jscolor{ mode:\'HSV\',width:225, height:130, position:\'top\', borderColor :\'#e3e3e3\', insetColor : \'#666\', backgroundColor : \'#666\'}\" "));
	response->println(String(F("id=\"")) + String(_deviceId) + "_" + String(_ctrl_elem->id) + String(F("\">")));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateColor"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateSelect(AsyncResponseStream *response, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateSelect"));
#endif // DEBUG
	response->print(F("				<select class=\"small\" "));
	response->println(String(F("id=\"")) + String(_deviceId) + "_" + String(_ctrl_elem->id) + String(F("\">")));
	GenerateMultiOption(response, _ctrl_elem);
	response->println(F("				</select>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateSelect"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateMultiOption(AsyncResponseStream *response, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateMultiOption"));
#endif // DEBUG
	for (uint8_t I = 0; I < ((Select_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		response->print(F("					<option"));
		int t = ((Select_CtrlElem*)_ctrl_elem)->ToString().toInt();
		if (((Select_CtrlElem*)_ctrl_elem)->ToString().toInt() == I)
			response->print(F(" selected "));
		response->print(F("	value=\""));
		response->print(String(I));
		response->print(F("\">"));
		response->print(((Select_CtrlElem*)_ctrl_elem)->GetMember(I));
		response->print(_ctrl_elem->unit);
		response->println(F("</option>"));
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateMultiOption"));
#endif // DEBUG
}


void WebSocket_Wifi_Device_Driver::GenerateInput(AsyncResponseStream *response, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateInput"));
#endif // DEBUG
	if (_ctrl_elem->type == pass)
		response->print(F("					<input type=\"password\" class=\"small\" id=\""));
	else
		response->print(F("					<input type=\"text\" class=\"small\" id=\""));
	response->print(String(_deviceId) + "_" + String(_ctrl_elem->id));
	response->print(F("\"  value=\""));
	response->print(_ctrl_elem->ToString());
	if (!_ctrl_elem->editable) {
		response->println(F("\" disabled>"));
	}
	else {
		response->println(F("\">"));
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateInput"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateButtonGroup(AsyncResponseStream *response, int _deviceId, CtrlElem* _ctrl_elem) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateButtonGroup"));
#endif // DEBUG
	for (uint8_t I = 0; I < ((Group_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		response->print(F("<div class=\""));
		if (((Group_CtrlElem*)_ctrl_elem)->MembersCount() > 1) {
			if (I > 0) {
				response->print(F(" append "));
			}
			if (I < (((Group_CtrlElem*)_ctrl_elem)->MembersCount() - 1)) {
				response->print(F(" prepend "));
			}
		}
		response->print(F("\">"));
		response->print(F("				<button class=\"button small\" "));
		response->print(F("onclick=\"SendMessage("));
		response->print(String(_deviceId));
		response->print(F(", "));
		response->print(String(_ctrl_elem->id));
		response->print(F(", "));
		response->print(String(I) + F("); \">"));
		response->print(((Group_CtrlElem*)_ctrl_elem)->GetMember(I));
		response->println(F("</button>"));
	}
	for (uint8_t I = 0; I < ((Group_CtrlElem*)_ctrl_elem)->MembersCount(); I++) {
		response->print(F("</div>"));
	}
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateButtonGroup"));
#endif // DEBUG
}

void WebSocket_Wifi_Device_Driver::GenerateSetButton(AsyncResponseStream *response, int _deviceId, int _cmdId) {
#ifdef DEBUG
	Serial.println(F("Start WebSocket_Wifi_Device_Driver::GenerateSetButton"));
#endif // DEBUG
	response->print(F("				<button class=\"button small\" onclick=\"SendMessage("));
	response->print(String(_deviceId));
	response->print(F(", "));
	response->print(String(_cmdId));
	response->println(F(");\">Set</button>"));
#ifdef DEBUG
	Serial.println(F("Ende WebSocket_Wifi_Device_Driver::GenerateSetButton"));
#endif // DEBUG
}

