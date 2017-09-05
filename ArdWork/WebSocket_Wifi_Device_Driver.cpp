#include "WebSocket_Wifi_Device_Driver.h"
#include <FS.h>

ESP8266WebServer *WebSocket_Wifi_Device_Driver::server;
WebSocketsServer *WebSocket_Wifi_Device_Driver::webSocket;

static const char html[] PROGMEM = "<!DOCTYPE html><html><head> <title>Bilderrahme</title> <meta charset = \"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <!-- SCRIPT --> <script src=\"dist/js/jquery.min.js\"></script> <script src=\"dist/js/kube.min.js\"></script> <script src=\"dist/js/jscolor.min.js\"></script> <script src=\"dist/js/chartist.min.js\"></script> <!-- CSS --> <link rel=\"stylesheet\" href=\"dist/css/chartist.min.css\"> <link rel=\"stylesheet\" href=\"dist/css/kube.min.css\"> <link rel=\"stylesheet\" href=\"dist/css/bttn.min.css\"> <script> function update() { var data = { labels: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri'], series: [ [5, 2, 4, 2, 0] ] }; new Chartist.Line('.chart1', data); new Chartist.Bar('.chart2', data); new Chartist.Line('.chart3', data); } </script> <style> body { font-family: Tahoma, Verdana, Segoe, sans-serif; margin-left: 10%; margin-right: 10%; background-color: #DFFFDF; min-width: 300px; } .content { padding-left: 10px; padding-right: 10px; } .simple-text { color: #e3e3e3; text-shadow: 1px 1px 0px black; } a { text-align: center; text-decoration: none; display: inline-block; } a.hover { text-decoration: none; } .tabs { background: #e3e3e3; background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); } .tabs a { width: 100%; background: transparent; padding: 10px 25px; position: center; } .tabs a:hover { background: white; text-decoration: none; } .tabs a:visited { background: white; } .tabs h3 { font-family: \"Century Gothic\", CenturyGothic, Geneva, AppleGothic, sans-serif; font-size: 1.3em; line-height: 1.5em; margin: 0.3em 0.7em 0.3em 0.7em; letter-spacing: -0.1em; color: #454545; text-align: center; } .tabs li { font-size: 1.2em; } .container { background: #666; margin: auto; padding-top: 30px; font-family: helvetica, arial, sans-serif; } .head-line { background: #e3e3e3; position: relative; background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); padding: 10px 20px; margin-left: -20px; margin-top: 0; width: 70%; max-width: 550px; min-width: 300px; font-family: Georgia, 'Times New Roman', serif; -moz-box-shadow: 1px 1px 3px #292929; -webkit-box-shadow: 1px 1px 3px #292929; color: #454545; text-shadow: 0 1px 0 white; } .head-line.small { width: 30%; min-width: 200px; max-width: 250px; font-size: 1.3em; line-height: 0.8em; font-family: Tahoma, Verdana, Segoe, sans-serif; } .arrow { position: absolute; width: 0; height: 0; line-height: 0; border-left: 20px solid transparent; border-top: 10px solid #c8c8c8; top: 104%; left: 0; } .button { font-size: 18px; font-weight: bold; text-decoration: none; background: #e3e3e3; position: relative; background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); font-family: Georgia, 'Times New Roman', serif; -moz-box-shadow: 1px 1px 3px #292929; -webkit-box-shadow: 1px 1px 3px #292929; color: #454545; text-shadow: 0 1px 0 white; align: left; } .button:hover { background-color: #454545; color: #454545; box-shadow: 0 1px 5px #313131, 0 3px 3px #393939, 0 3px 10px rgba(0, 0, 0, 0.1), 0 2px 15px rgba(200, 200, 200, 0.4) inset, 0 -2px 9px rgba(0, 0, 0, 0.2) inset; } .button:active { box-shadow: 0 2px 0 #393939, 0 4px 4px rgba(0, 0, 0, 0.4), 0 2px 5px rgba(0, 0, 0, 0.2) inset; top: 2px; } .desc { color: #454545; } </style></head><body onload=\"update()\"> <div class=\"container\"> <h2 class=\"head-line\">Bilderrahmen Steuerung <span class=\"arrow\"></span> </h2> <div data-component=\"sticky\"> <nav class=\"tabs\" data-component=\"tabs\"> <ul onclick=\"update()\"> <li><h3><strong>ArdWork</strong></h3></li> <li class=\"active\"><a href=\"#tabWifi\">Wifi</a></li> <li><a href=\"#tabStripe\">RGB-Stripe</a></li> <li><a href=\"#tabLuxmeter\">Luxmeter</a></li> <li><a href=\"#tabDebug\">Debug</a></li> </ul> </nav> </div> <div class=\"content\" id=\"tabWifi\"> <div class=\"simple-text\"> <p> <strong> Wifi stellt die Verbindung zum heimischen Netzwerk her oder stellt selbst einen AcessPoint bereit </strong> </p> <div class=\"head-line small\">Features</div> <p> <ul> <li>SSID &auml;ndern</li> <li>Passwort &auml;ndern</li> <li>Wechsel der Modi STA/AP</li> <li>Reconnect</li> </ul> </p> </div> <div class=\"head-line small\">Steuerung</div> <br> <p id=\"my_p\"> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm small\"> <input type=\"text\" name=\"Wifi-SSID\" class=\"small\" placeholder=\"SSID\"> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Set SSID</div> </div> </form> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm small\"> <input type=\"password\" name=\"Wifi-Password\" class=\"small\" placeholder=\"Password\"> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Set Password</div> </div> </form> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <select class=\"small\"> <option>STA</option> <option>AP</option> <option>STA/AP</option> </select> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Set Wifi-Mode</div> </div> </form> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <button class=\"button small\">Reconnect</button> </div> </form> </p> <br> </div> <div class=\"content\" id=\"tabDebug\"> <div class=\"simple-text\"> <p> <strong> Debug stellt die Controlle &uuml;ber alle verbauten elemente bereit. <br>Diese Funktion muss erst aktiviert werden </strong> </p> <div class=\"head-line small\">Features</div> <p> <ul> <li>LED1 an/aus/blink</li> <li>LED2 an/aus/blink</li> <li>Button1 Pressn</li> <li>Rohwert auslesen Luxmeter1</li> </ul> </p> </div> <div class=\"head-line small\">Steuerung</div> <br> <p> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <select class=\"small\"> <option>Debug Off</option> <option>Debug On</option> </select> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Set Debug-Mode</div> </div> </form> </p> </div> <div class=\"content\" id=\"tabStripe\"> <div class=\"simple-text\"> <p> <strong> RGB-Stripe stellt die Steuerung der RGB-LEDs bereit es erlaubt die Kontrolle &uuml;ber die Muster und Farben </strong> </p> <div class=\"head-line small\">RGB Features</div> <p> <ul> <li>Pick a Pattern</li> <li>Pick a Color</li> <li>Turn On/OFF</li> </ul> </p> </div> <div class=\"head-line small\">Steuerung</div> <br> <p> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <select class=\"small\"> <option value=\"0\">Cyclon</option> <option value=\"1\">Random</option> <option value=\"2\">Fire</option> <option value=\"3\">Shine</option> </select> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Pick a pattern</div> </div> </form> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <input value=\"e3e3e3\" class=\"small jscolor {mode:'HSV',width:243, height:150, position:'right',borderColor:'#e3e3e3', insetColor:'#666', backgroundColor:'#666'}\"> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Pick a color</div> </div> </form> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <select class=\"small\"> <option>On</option> <option>OFF</option> </select> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Set on/off</div> </div> </form> </p> </div> <div class=\"content\" id=\"tabLuxmeter\"> <div class=\"simple-text\"> <p> <strong> Luxmeter stellt die Steuerung des Lichtsensors bereit es erlaubt die Kontrolle &uuml;ber die Ausleseparameter und stellt Live-Werte sowie Diagramme bereit </strong> </p> <div class=\"head-line small\">Lux Features</div> <p> <ul> <li>Fahrenheit/Celcius</li> <li>Integrationszeit</li> <li>Verst&auml;rkung(Gain)</li> <li>Lichtwert ausgeben</li> </ul> </p> </div> <div class=\"head-line small\">Steuerung</div> <br> <p> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <select class=\"small\"> <option>Celsius</option> <option>Fahrenheit</option> </select> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Pick a format</div> </div> </form> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <select class=\"small\"> <option>13ms</option> <option>101ms</option> <option>402ms</option> </select> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Pick a integration time</div> </div> </form> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div class=\"append w45 w100-sm\"> <select class=\"small\"> <option>Auto</option> <option>1x</option> <option>16x</option> </select> <button class=\"button small\">Set</button> </div> <div class=\"desc\">Pick a Gain</div> </div> </form> <br> <center><strong>Helligkeitswerte der letzten 24h<strong></center> <br> <div class=\"chart1\" .ct-double-octave></div> <div class=\"chart2\" .ct-double-octave></div> <br> </p> </div> <form method=\"post\" action=\"\" class=\"form\"> <div class=\"form-item\"> <div><button style=\"width:100%; margin-bottom: 2px;\" class=\"button small\">Save Config</button></div> </div> </form> </div></body></html>";

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

		if (payload[0] == '#') {
			// we get RGB data

			// decode rgb data
			uint32_t rgb = (uint32_t)strtol((const char *)&payload[1], NULL, 16);
			
			webSocket->sendTXT(num, (const char *)&payload[1]);
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

}

void WebSocket_Wifi_Device_Driver::UpdateControls(){

}


void WebSocket_Wifi_Device_Driver::UpdateComm(uint32_t deltaTime){
	webSocket->loop();
	server->handleClient();
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
	server->serveStatic("/dist/js/jscolor.min.js", SPIFFS, "/dist/js/jscolor.min.js");
	server->serveStatic("/dist/js/chartist.min.js", SPIFFS, "/dist/js/chartist.min.js");
	server->serveStatic("/dist/js/jquery.min.js", SPIFFS, "/dist/js/jquery.min.js");

	server->serveStatic("/dist/css/chartist.min.css", SPIFFS, "/dist/css/chartist.min.css");
	server->serveStatic("/dist/css/kube.min.css", SPIFFS, "/dist/css/kube.min.css");
	server->serveStatic("/dist/css/bttn.min.css", SPIFFS, "/dist/css/bttn.min.css");

	server->on("/", []() {
		server->send_P(200, "text/html", (html));
		// send index.html
		//server->send(200, "text/html", "<html><head> <script> var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']); connection.onopen = function() { connection.send('Connect ' + new Date()); }; connection.onerror = function(error) { console.log('WebSocket Error ', error); }; connection.onmessage = function(e) { if (e.data.indexOf('subProtocol') == -1) document.getElementById('response').innerHTML = e.data + '<br/>'; }; function sendMessage(msg) { connection.send(msg); } function sendRGB() { var r = parseInt(document.getElementById('r').value).toString(16); var g = parseInt(document.getElementById('g').value).toString(16); var b = parseInt(document.getElementById('b').value).toString(16); if (r.length < 2) { r = '0' + r; } if (g.length < 2) { g = '0' + g; } if (b.length < 2) { b = '0' + b; } var rgb = '#' + r + g + b; console.log('RGB: ' + rgb); connection.send(rgb); } </script></head><body>LED Control: <br/> <br/>R: <input id='r' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>G: <input id='g' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/>B: <input id='b' type='range' min='0' max='255' step='1' oninput='sendRGB(); ' /> <br/> <br/> <br/> <div id='response'> My Name is jWebSocketServer </div></body></html>");
	});

	server->begin();

	// Add service to MDNS
	MDNS.addService("http", "tcp", 80);
	MDNS.addService("ws", "tcp", 81);
}
