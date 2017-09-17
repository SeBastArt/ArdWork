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
		Serial.println("Failed to parse config file");
	}
	value = root[_key].as<String>();
	return value;
}

void WebSocket_Wifi_Device_Driver::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
	switch (type) {
	case WStype_DISCONNECTED:
		Serial.printf("[%u] Disconnected!\n", num);
		break;
	case WStype_CONNECTED: {
		IPAddress ip = webSocket->remoteIP(num);
		Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

		// send message to client
		webSocket->sendTXT(num, "Connected");
	}
						   break;
	case WStype_TEXT:
		Serial.printf("[%u] get Text: %s\n", num, payload);

		if (payload[0] == '{') {
			// we get RGB data

			// decode rgb data
			//uint32_t rgb = (uint32_t)strtol((const char *)&payload[1], NULL, 16);
			String test =String((const char *)&payload[0]);

			int deviceId = atoi(Json_GetvalueFromKey(test, "deviceId").c_str());
			int cmdId = atoi(Json_GetvalueFromKey(test, "cmdId").c_str());
			String value = Json_GetvalueFromKey(test, "value");
			Serial.print("deviceId: ");
			Serial.println(deviceId);
			Serial.print("cmdId: ");
			Serial.println(cmdId);
			Serial.print("value: ");
			Serial.println(value);

			if (__event_msg._filled == false) {
				__event_msg._deviceId = deviceId;
				__event_msg._cmdId = cmdId;
				__event_msg._value = value;
				__event_msg._filled = true;
			}
			webSocket->sendTXT(num, (const char *)&payload[0]);
		
			delay(10);
			//analogWrite(LED_RED, ((rgb >> 16) & 0xFF));
			//analogWrite(LED_GREEN, ((rgb >> 8) & 0xFF));
			//analogWrite(LED_BLUE, ((rgb >> 0) & 0xFF));
		}

		break;
	}

}

WebSocket_Wifi_Device_Driver::WebSocket_Wifi_Device_Driver(Module_Driver * module, String _ssid, String _password, Led_Device_Driver * _statusLED, uint8_t priority) :
	Wifi_Device_Driver(module, _ssid, _password, _statusLED, priority)
{
	driver_name = "WebSocket_Wifi_Device_Driver";

	server = new ESP8266WebServer(80);
	webSocket = new WebSocketsServer(81);
	__event_msg._filled = false;
}


void WebSocket_Wifi_Device_Driver::Build_Descriptor() {
	__descriptor->name = "Wifi";
	__descriptor->descr = "Wifi stellt die Verbindung zum heimischen Netzwerk her oder stellt selbst einen AcessPoint bereit";
	__descriptor->published = true;

	Ctrl_Elem *ctrl_elem_mode = new Ctrl_Elem(WEBSOCKET_WIFI_DEVICE_DRIVER_FIRST_MESSAGE, "Wifi-Mode", select, "Mode aendern");
	
	Atomic<String> *atomic_mode_sta = new Atomic<String>(0, "STA", "");
	Atomic<String> *atomic_mode_ap = new Atomic<String>(1, "AP", "");
	Atomic<String> *atomic_mode_staap = new Atomic<String>(2, "STA/AP", "");

	ctrl_elem_mode->AddAtomic(atomic_mode_sta);
	ctrl_elem_mode->AddAtomic(atomic_mode_ap);
	ctrl_elem_mode->AddAtomic(atomic_mode_staap);
	ctrl_elem_mode->published = true;

	Ctrl_Elem *ctrl_elem_SSID = new Ctrl_Elem(WEBSOCKET_WIFI_DEVICE_DRIVER_FIRST_MESSAGE + 1, "SSID", text, "SSID aendern");
	
	Atomic<String> *atomic_SSID = new Atomic<String>(0, "SSID");
	ctrl_elem_SSID->AddAtomic(atomic_SSID);
	ctrl_elem_SSID->published = true;

	Ctrl_Elem *ctrl_elem_pass = new Ctrl_Elem(WEBSOCKET_WIFI_DEVICE_DRIVER_FIRST_MESSAGE + 2, "Passwort", pass, "Passwort aendern");
	
	Atomic<String> *atomic_pass = new Atomic<String>(0, "Passwort");
	ctrl_elem_pass->AddAtomic(atomic_pass);
	ctrl_elem_pass->published = true;

	Ctrl_Elem *ctrl_elem_reconnect = new Ctrl_Elem(WEBSOCKET_WIFI_DEVICE_DRIVER_FIRST_MESSAGE + 3, "Reconnect", button, "Reconnect to Wifi");
	ctrl_elem_reconnect->published = true;

	__descriptor->Add_Descriptor_Element(ctrl_elem_SSID);
	__descriptor->Add_Descriptor_Element(ctrl_elem_pass);
	__descriptor->Add_Descriptor_Element(ctrl_elem_mode);
	__descriptor->Add_Descriptor_Element(ctrl_elem_reconnect);
}


void WebSocket_Wifi_Device_Driver::UpdateControls() {

}


void WebSocket_Wifi_Device_Driver::UpdateComm(uint32_t deltaTime) {
	webSocket->loop();
	server->handleClient();

	if (__event_msg._filled) {
		CommunicationMessage* message = new CommunicationMessage(__event_msg._deviceId, __event_msg._cmdId, __event_msg._value);
		if (!parentModule->SendAsyncThreadMessage(message))
		{
			Serial.println(">> message buffer overflow <<");
		}
	}
	__event_msg._filled = false;
}

void WebSocket_Wifi_Device_Driver::InitComm() {
	webSocket->begin();
	webSocket->onEvent(webSocketEvent);

	if (MDNS.begin("esp8266")) {
		Serial.println("MDNS responder started");
	}

	// handle index
	server->serveStatic("/dist/js/jscolor.min.js", SPIFFS, "/dist/js/jscolor.min.js");
	server->serveStatic("/dist/js/kube.min.js", SPIFFS, "/dist/js/kube.min.js");

	server->serveStatic("/dist/css/kube.min.css", SPIFFS, "/dist/css/kube.min.css");
	server->serveStatic("/dist/css/bttn.min.css", SPIFFS, "/dist/css/bttn.min.css");
	//server->serveStatic("/", SPIFFS, "/Index.html");
	
	server->on("/", [&]() {
		WiFiClient client = server->client();
		//static const char header[] = "<meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/2.1.4/jquery.min.js\"></script> <script src=\"dist/js/kube.min.js\"></script> <script src=\"dist/js/jscolor.min.js\"></script> <script type=\"text/javascript\" src=\"https://rawgit.com/gionkunz/chartist-js/master/dist/chartist.js\"></script> <link rel=\"stylesheet\" type=\"text/css\" href=\"https://rawgit.com/gionkunz/chartist-js/master/dist/chartist.min.css\"> <link rel=\"stylesheet\" href=\"dist/css/kube.min.css\"> <link rel=\"stylesheet\" href=\"dist/css/bttn.min.css\"> <script>function update(){var data={labels: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri'], series: [ [5, 2, 4, 2, 0]]}; new Chartist.Line('.chart1', data); new Chartist.Bar('.chart2', data); new Chartist.Line('.chart3', data);}var connection=new WebSocket('ws://' + location.hostname + ':81/', ['arduino']); connection.onopen=function (){connection.send('Connect ' + new Date());}; connection.onerror=function (error){console.log('WebSocket Error ', error);}; connection.onmessage=function (e){if (e.data.indexOf('subProtocol')==-1) document.getElementById('response').innerHTML=e.data + '<br/>';}; function SendMessage(_deviceId, _cmdId) {var search_id = _deviceId.toString() + '_' + _cmdId.toString();var obj = new Object();obj.deviceId = _deviceId;obj.cmdId  = _cmdId;obj.value = document.getElementById(search_id).value;var jsonString= JSON.stringify(obj);connection.send(jsonString);}function sendRGB(){var r=parseInt(document.getElementById('r').value).toString(16); var g=parseInt(document.getElementById('g').value).toString(16); var b=parseInt(document.getElementById('b').value).toString(16); if (r.length < 2){r='0' + r;}if (g.length < 2){g='0' + g;}if (b.length < 2){b='0' + b;}var rgb='#' + r + g + b; console.log('RGB: ' + rgb); connection.send(rgb);}</script> <style>body{font-family: Tahoma, Verdana, Segoe, sans-serif; margin-left: 10%; margin-right: 10%; background-color: #DFFFDF; min-width: 400px;}.content{padding-left: 10px; padding-right: 10px;}.simple-text{color: #e3e3e3; text-shadow: 1px 1px 0px black;}a{text-align: center; text-decoration: none; display: inline-block;}a.hover{text-decoration: none;}.tabs{background: #e3e3e3; background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8));}.tabs a{width: 100%; background: transparent; padding: 10px 25px; position: center;}.tabs a:hover{background: white; text-decoration: none;}.tabs a:visited{background: white;}.tabs h3{font-family: \"Century Gothic\", CenturyGothic, Geneva, AppleGothic, sans-serif; font-size: 1.3em; line-height: 1.5em; margin: 0.3em 0.7em 0.3em 0.7em; letter-spacing: -0.1em; color: #454545; text-align: center;}.tabs li{font-size: 1.2em;}.container{background: #666; margin: auto; padding-top: 30px; font-family: helvetica, arial, sans-serif;}.head-line{background: #e3e3e3; position: relative; background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); padding: 10px 20px; margin-left: -20px; margin-top: 0; width: 70%; max-width: 550px; min-width: 400px; font-family: Georgia, 'Times New Roman', serif; -moz-box-shadow: 1px 1px 3px #292929; -webkit-box-shadow: 1px 1px 3px #292929; color: #454545; text-shadow: 0 1px 0 white;}.head-line.small{width: 30%; min-width: 200px; max-width: 250px; font-size: 1.3em; line-height: 0.8em; font-family: Tahoma, Verdana, Segoe, sans-serif;}.arrow{position: absolute; width: 0; height: 0; line-height: 0; border-left: 20px solid transparent; border-top: 10px solid #c8c8c8; top: 104%; left: 0;}.button{font-size: 18px; font-weight: bold; text-decoration: none; background: #e3e3e3; position: relative; background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); font-family: Georgia, 'Times New Roman', serif; -moz-box-shadow: 1px 1px 3px #292929; -webkit-box-shadow: 1px 1px 3px #292929; color: #454545; text-shadow: 0 1px 0 white; align: left;}.button:hover{background-color: #454545; color: #454545; box-shadow: 0 1px 5px #313131, 0 3px 3px #393939, 0 3px 10px rgba(0, 0, 0, 0.1), 0 2px 15px rgba(200, 200, 200, 0.4) inset, 0 -2px 9px rgba(0, 0, 0, 0.2) inset;}.button:active{box-shadow: 0 2px 0 #393939, 0 4px 4px rgba(0, 0, 0, 0.4), 0 2px 5px rgba(0, 0, 0, 0.2) inset; top: 2px;}.desc{color: #454545;}</style></head>";	
		//static const char html2[] = "<div class=\"content\" id=\"tabWifi\"><div class=\"simple-text\"><p> <strong> Wifi stellt die Verbindung zum heimischen Netzwerk her oder stellt selbst einen AcessPoint bereit </strong></p><div class=\"head-line small\">Features</div><p><ul><li>SSID ändern</li><li>Passwort ändern</li><li>Wechsel der Modi STA/AP</li><li>Reconnect</li></ul></p></div><div class=\"head-line small\">Steuerung</div> <br><p id=\"my_p\"><div class=\"form-item\"><div class=\"append w45 w100-sm small\"> <input type=\"text\" id=\"SSID\" class=\"small\" placeholder=\"SSID\"> <button class=\"button small\" onclick='SendSSID();'>Set</button></div><div class=\"desc\">Set SSID</div></div><div class=\"form-item\"><div class=\"append w45 w100-sm small\"> <input type=\"password\" id=\"Password\" class=\"small\" placeholder=\"Password\"> <button class=\"button small\" onclick='SendPassword();'>Set</button></div><div class=\"desc\">Set Password</div></div><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <select class=\"small\"><option>STA</option><option>AP</option><option>STA/AP</option> </select><button class=\"button small\">Set</button></div><div class=\"desc\">Set Wifi-Mode</div></div><div class=\"form-item\"> <button class=\"button small\">Reconnect</button></div></p> <br></div><div class=\"content\" id=\"tabDebug\"><div class=\"simple-text\"><p> <strong> Debug stellt die Controlle über alle verbauten elemente bereit. <br>Diese Funktion muss erst aktiviert werden </strong></p><div class=\"head-line small\">Features</div><p><ul><li>LED1 an/aus/blink</li><li>LED2 an/aus/blink</li><li>Button1 Pressn</li><li>Rohwert auslesen Luxmeter1</li></ul></p></div><div class=\"head-line small\">Steuerung</div> <br><p><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <select class=\"small\"><option>Debug Off</option><option>Debug On</option> </select> <button class=\"button small\">Set</button></div><div class=\"desc\">Set Debug-Mode</div></div></p></div><div class=\"content\" id=\"tabStripe\"><div class=\"simple-text\"><p> <strong> RGB-Stripe stellt die Steuerung der RGB-LEDs bereit es erlaubt die Kontrolle über die Muster und Farben </strong></p><div class=\"head-line small\">RGB Features</div><p><ul><li>Pick a Pattern</li><li>Pick a Color</li><li>Turn On/OFF</li></ul></p></div><div class=\"head-line small\">Steuerung</div> <br><p><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <select class=\"small\"><option value=\"0\">Cyclon</option><option value=\"1\">Random</option><option value=\"2\">Fire</option><option value=\"3\">Shine</option> </select> <button class=\"button small\">Set</button></div><div class=\"desc\">Pick a pattern</div></div><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <input value=\"e3e3e3\" class=\"small jscolor {mode:'HSV',width:243, height:150, position:'right', borderColor:'#e3e3e3', insetColor:'#666', backgroundColor:'#666'}\"> <button class=\"button small\">Set</button></div><div class=\"desc\">Pick a color</div></div><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <select class=\"small\"><option>On</option><option>OFF</option> </select> <button class=\"button small\">Set</button></div><div class=\"desc\">Set on/off</div></div></p></div><div class=\"content\" id=\"tabLuxmeter\"><div class=\"simple-text\"><p> <strong> Luxmeter stellt die Steuerung des Lichtsensors bereit es erlaubt die Kontrolle über die Ausleseparameter und stellt Live-Werte sowie Diagramme bereit </strong></p><div class=\"head-line small\">Lux Features</div><p><ul><li>Fahrenheit/Celcius</li><li>Integrationszeit</li><li>Verstärkung(Gain)</li><li>Lichtwert ausgeben</li></ul></p></div><div class=\"head-line small\">Steuerung</div> <br><p><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <select class=\"small\"><option>Celsius</option><option>Fahrenheit</option> </select> <button class=\"button small\">Set</button></div><div class=\"desc\">Pick a format</div></div><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <select class=\"small\"><option>13ms</option><option>101ms</option><option>402ms</option> </select> <button class=\"button small\">Set</button></div><div class=\"desc\">Pick a integration time</div></div><div class=\"form-item\"><div class=\"append w45 w100-sm\"> <select class=\"small\"><option>Auto</option><option>1x</option><option>16x</option> </select> <button class=\"button small\">Set</button></div><div class=\"desc\">Pick a Gain</div></div><br><center><strong>Helligkeitswerte der letzten 24h<strong></center> <br><div class=\"chart1\" .ct-double-octave></div><div class=\"chart2\" .ct-double-octave></div> <br></p></div><div id=\"response\"></div><div class=\"form-item\"><div><button style=\"width:100%; margin-bottom: 2px;\" class=\"button small\">Save Config</button></div></div></div></body></html>";
		//server->send(200, "text/html", "");
		//server->send(200, "text/html", "<html><head> <script> var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']); connection.onopen = function() { connection.send('Connect ' + new Date()); }; connection.onerror = function(error) { console.log('WebSocket Error ', error); }; connection.onmessage = function(e) { if (e.data.indexOf('subProtocol') == -1) document.getElementById('response').innerHTML = e.data + '<br/>'; }; function sendMessage(msg) { connection.send(msg); } function sendRGB() { var r = parseInt(document.getElementById('r').value).toString(16); var g = parseInt(document.getElementById('g').value).toString(16); var b = parseInt(document.getElementById('b').value).toString(16); if (r.length < 2) { r = '0' + r; } if (g.length < 2) { g = '0' + g; } if (b.length < 2) { b = '0' + b; } var rgb = '#' + r + g + b; console.log('RGB: ' + rgb); connection.send(rgb); } </script></head><body>LED Control: <br/> <br/>R: <input id='r' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>G: <input id='g' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>B: <input id='b' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/> <br/> <br/> <div id='response'> My Name is jWebSocketServer </div></body></html>");

		//static const char head[] = "<!DOCTYPE html><html><head> <title>Bilderrahme</title>";
		//server->sendContent(head);
		//client.println(head);
		//server->sendContent(header);
		//client.println(header);
		SendHeader(&client);
		if (__descriptor_list->count > 0) {
			String text = "";
			text += "<body onload=\"update();\">\n";
			text += "<div class=\"container\">\n";
			text += "<h2 class=\"head-line\">Bilderrahmen Steuerung <span class=\"arrow\"></span></h2>\n";
			//server->sendContent(text);
			client.println(text);
			text = "";
			//server->sendContent(GenerateNav(__descriptor_list));
			client.println(GenerateNav(__descriptor_list));
			yield;
			delay(10);
			for (int I = 0; I < __descriptor_list->count; I++) {
				if (__descriptor_list->GetElemByIndex(I)->published)
					//server->sendContent(GenerateTab(__descriptor_list->GetElemByIndex(I)));
					client.println(GenerateTab(__descriptor_list->GetElemByIndex(I)));
				yield;
				delay(10);
			}
			text += "</div>\n";
			text += "</body>\n";
			text += "</html>\n";
			//server->sendContent(text);
			client.println(text);
		}
		server->client().stop();
	});
	server->begin();

	// Add service to MDNS
	MDNS.addService("http", "tcp", 80);
	MDNS.addService("ws", "tcp", 81);
}




String WebSocket_Wifi_Device_Driver::GenerateNav(Descriptor_List *_descriptor_list) {
	String nav = "";
	nav += "<nav class =\"tabs\" data-component =\"tabs\">\n";
	nav += "<ul onclick =\"update();\">\n";
	nav += "<li><h3><strong>ArdWork</strong></h3></li>\n";
	for (int I = 0; I < __descriptor_list->count; I++)
		if (__descriptor_list->GetElemByIndex(I)->published) {
			nav += "<li ";
			if (I == 0)
				nav += "class = \"active\"";
			nav += ">";
			nav += "<a href=\"#tab" + String(__descriptor_list->GetElemByIndex(I)->name) + "\">" + String(__descriptor_list->GetElemByIndex(I)->name) + "</a>";
			nav += "</li>\n";
		}
	nav += "</ul>\n";
	nav += "</nav>\n";
	return nav;
}

String WebSocket_Wifi_Device_Driver::GenerateTab(Descriptor *_descriptor) {
	String tab = "";
	tab += " <div class=\"content\" id=\"tab" + String(_descriptor->name) + "\">\n";
	tab += " \t<div class=\"simple-text\">\n";
	tab += " \t\t<p>\n";
	tab += " \t\t<strong>\n";
	tab += " \t\t\t" + String(_descriptor->descr) + "\n";
	tab += " \t\t</strong>\n";
	tab += " \t\t</p>\n";
	tab += " \t\t<div class=\"head-line small\">Features</div>\n";
	tab += " \t\t<p>\n";
	tab += " \t\t\t<ul>\n";

	for (int J = 0; J < _descriptor->ctrl_count; J++)
		tab += "<li>" + String(_descriptor->GetCtrlElemByIndex(J)->descr) + "</li>\n";

	tab += " \t\t\t</ul>\n";
	tab += " \t\t</p>\n";
	tab += " \t</div>\n";
	tab += " \t<div class=\"head-line small\">Steuerung</div>\n";
	tab += " \t<br>\n";
	tab += " \t<p>\n";
	for (int I = 0; I < _descriptor->ctrl_count; I++)
		tab += GenerateForm(_descriptor->id, _descriptor->GetCtrlElemByIndex(I));

	tab += " \t</p>\n";
	tab += " \t<br>\n";
	tab += " </div>\n";
	return tab;
}

String WebSocket_Wifi_Device_Driver::GenerateForm(int _deviceId, Ctrl_Elem* _ctrl_elem) {
	String form = "";
	form += "<div class = \"form-item\">\n";
	form += "\t<div class = \"append w45 w100-sm\">\n";
	switch (_ctrl_elem->type)
	{
	case text:
	{
		form += GenerateInput(_deviceId, _ctrl_elem);
		form += GenerateSetButton(_deviceId, _ctrl_elem->id);
		break;
	}
	case pass:
	{
		form += GenerateInput(_deviceId, _ctrl_elem);
		form += GenerateSetButton(_deviceId, _ctrl_elem->id);
		break;
	}
	case select:
	{
		form += GenerateSelect(_deviceId, _ctrl_elem);
		form += GenerateSetButton(_deviceId, _ctrl_elem->id);
		break;
	}
	case color:
	{
		form += GenerateColor(_deviceId, _ctrl_elem);
		form += GenerateSetButton(_deviceId, _ctrl_elem->id);
		break;
	}
	case button:
	{
		form += GenerateButton(_deviceId, _ctrl_elem);
		break;
	}
	default:
		break;
	}

	form += "\t</div>\n";
	form += GenerateDecending(_ctrl_elem);
	form += "</div>\n";
	return form;
}

String WebSocket_Wifi_Device_Driver::GenerateDecending(Ctrl_Elem* _ctrl_elem) {
	String decending = "";
	decending += "\t<div class=\"desc\">";
	switch (_ctrl_elem->type)
	{
	case text:
	{
		decending += "Set ";
		break;
	}

	case pass:
	{
		decending += "Set ";
		break;
	}
	case select:
	{
		decending += "Pick a ";
		break;
	}
	case color:
	{
		decending += "Pick a ";
		break;
	}
	case button:
	{
		decending += _ctrl_elem->descr;
		break;
	}
	}
	decending += String(_ctrl_elem->name);
	decending += "</div>\n";

	return decending;
}


String WebSocket_Wifi_Device_Driver::GenerateColor(int _deviceId, Ctrl_Elem* _ctrl_elem) {
	String color = "";
	color += "\t\t<input ";
	color += "value=\"";

	for (int I = 0; I < _ctrl_elem->atomic_count; I++) {
		char buf[12];
		color += String(itoa(((Atomic<int>*)_ctrl_elem->GetAtomicByIndex(I))->value, buf, 16));
	}
	color += "\" class=\"small ";
	color += "jscolor{ mode:\'HSV\',width:225, height:130, position:\'right\', borderColor :\'#e3e3e3\', insetColor : \'#666\', backgroundColor : \'#666\'}\" ";
	color += "id=\"" + String(_deviceId) + "_" + String(_ctrl_elem->id) + "\">\n";
	return color;
}


String WebSocket_Wifi_Device_Driver::GenerateSelect(int _deviceId, Ctrl_Elem* _ctrl_elem) {
	String select = "";
	select += "\t\t<select class=\"small\" ";
	select += "id=\"" + String(_deviceId) + "_" + String(_ctrl_elem->id) + "\">\n";
	select += GenerateOption(_ctrl_elem);
	select += "\t\t</select>\n";
	return select;
}

String WebSocket_Wifi_Device_Driver::GenerateOption(Ctrl_Elem* _ctrl_elem) {
	String option = "";
	for (int I = 0; I < _ctrl_elem->atomic_count; I++) {
		option += "\t\t\t<option value=\"";
		option += String(((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->id);
		option += "\">";
		option += ((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->ValueToString();
		option += String(((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->unit);
		option += "</option>\n";
	}
	return option;
}


String WebSocket_Wifi_Device_Driver::GenerateInput(int _deviceId, Ctrl_Elem* _ctrl_elem) {
	String input = "";

	for (int I = 0; I < _ctrl_elem->atomic_count; I++) {
		if (_ctrl_elem->type == pass)
			input += "\t\t<input type=\"password\" class=\"small\" id=\"";
		else
			input += "\t\t<input type=\"text\" class=\"small\" id=\"";
		input += String(_deviceId) + "_" + String(_ctrl_elem->id);
		input += "\"  placeholder=\"";
		input += ((Atomic<String>*)_ctrl_elem->GetAtomicByIndex(I))->ValueToString();
		input += "\">\n";
	}
	return input;
}


String WebSocket_Wifi_Device_Driver::GenerateButton(int _deviceId, Ctrl_Elem* _ctrl_elem) {
	String button = "";
	button += "\t\t<button class=\"button small\" onclick=\"SendMessage(";
	button += String(_deviceId);
	button += ", ";
	button += String(_ctrl_elem->id);
	button += ");\">";
	button += _ctrl_elem->name;
	button += "</button>\n";
	return button;
}

String WebSocket_Wifi_Device_Driver::GenerateSetButton(int _deviceId, int _cmdId) {
	String button = "";
	button += "\t\t<button class=\"button small\" onclick=\"SendMessage(";
	button += String(_deviceId);
	button += ", ";
	button += String(_cmdId);
	button += ");\">Set</button>\n";
	return button;
}