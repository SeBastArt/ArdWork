// 
// 
// 

#include "WebServer_Wifi_Device_Driver.h"

WebServer_Wifi_Device_Driver::WebServer_Wifi_Device_Driver(Module_Driver * module, String _ssid, String _password, Led_Device_Driver * _statusLED, uint8_t priority) :
	Wifi_Device_Driver(module, _ssid, _password, _statusLED, priority)
{
	driver_name = "WebServer_Wifi_Device_Driver";
	Control *std;
	std = ctrl_List->Front();
	std->Name = "WebServer Wifi";
	std->Description = "WebServer Wifi";
	std->Style = Icon_Kind_comm;
	server = new WiFiServer(80);
}

void WebServer_Wifi_Device_Driver::UpdateControls()
{
}


void WebServer_Wifi_Device_Driver::InitComm() {
	ulReqcount = 0;
	ulReconncount = 0;
	server->begin();
}


void WebServer_Wifi_Device_Driver::CheckComm(WiFiClient _client) {
	// Check if a client has connected
	Serial.println("CheckComm");
	if (!_client)
	{
		return;
	}
	
	// Wait until the client sends some data
	Serial.println("new client");
	unsigned long ultimeout = millis() + 250;
	while (!_client.available() && (millis() < ultimeout))
	{
		delay(1);
	}
	if (millis() > ultimeout)
	{
		Serial.println("client connection time-out!");
		return;
	}

	// Read the first line of the request
	String sRequest = _client.readStringUntil('\r');
	_client.flush();

	// stop client, if request is empty
	if (sRequest == "")
	{
		Serial.println("empty request! - stopping client");
		_client.stop();
		return;
	}
	Serial.println("ParseRequest(sRequest);");
	ParseRequest(sRequest);
}


String WebServer_Wifi_Device_Driver::GenerateHeader(String _response, bool _found)
{
	String sHeader;
	sHeader = (_found == true) ? "HTTP/1.1 200 OK\r\n" : "HTTP/1.1 404 Not found\r\n";
	sHeader += "Content-Length: ";
	sHeader += _response.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";

	return sHeader;
}


void WebServer_Wifi_Device_Driver::SendResponse(WiFiClient _client, String _header, String _response)
{
	_client.print(_header);
	_client.print(_response);

	// and stop the client
	_client.stop();
	Serial.println("Client disonnected");
}

void WebServer_Wifi_Device_Driver::UpdateComm(uint32_t deltaTime) {

	WiFiClient client = server->available();
	if (!client)
	{
		return;
	}
	CheckComm(client);

	///////////////////////////
	// format the html response
	///////////////////////////
	String sResponse, sHeader;

	ulReqcount++;
	sResponse = "<html><head><title>Demo f&uumlr ESP8266 Steuerung</title></head><body>";
	sResponse += "<font color=\"#000000\"><body bgcolor=\"#d0d0f0\">";
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Demo f&uumlr ESP8266 Steuerung</h1>";
	sResponse += "Funktion 1 schaltet GPIO2 und erzeugt eine serielle Ausgabe.<BR>";
	sResponse += "Funktion 2 erzeugt nur eine serielle Ausgabe.<BR>";
	sResponse += "<FONT size=+1>";

	Control *temp;
	if (!__control_list->Empty()) {
		for (int I = 0; I < __control_list->Size(); I++) {
			if ((*__control_list)[I]->Style == Icon_Kind_button) {
				sResponse += GenerateButton((*__control_list)[I]);
			}
			if ((*__control_list)[I]->Style == Icon_Kind_switch) {
				sResponse += GenerateSwitch((*__control_list)[I]);
			}
			if ((*__control_list)[I]->Style == Icon_Kind_Integer) {
				sResponse += GenerateInteger((*__control_list)[I]);
			}

		}
	}
	sResponse += "<FONT SIZE=-2>";

	sResponse += "<FONT size=-2>";
	sResponse += "<BR>Aufrufz&auml;hler=";
	sResponse += ulReqcount;
	sResponse += " - Verbindungsz&auml;hler=";
	sResponse += ulReconncount;
	sResponse += "</body></html>";

	sHeader = GenerateHeader(sResponse, true);


	// Send the response to the client
	SendResponse(client, sHeader, sResponse);
}


String WebServer_Wifi_Device_Driver::GenerateInteger(Control *_conrtrol)
{
	String response;
	response = "";
	response += "<p>";
	response += "ID: " + String(_conrtrol->Id);
	response += "&nbsp;";
	response += _conrtrol->Name;
	response += "&nbsp;";
	response += " Wert = " + _conrtrol->Data;
	response += "&nbsp;";
	response += "<FONT SIZE=-1>";
	response += _conrtrol->Description;
	response += "<FONT SIZE=+1>";
	response += "<BR>";
	return response;
}

String WebServer_Wifi_Device_Driver::GenerateSwitch(Control *_conrtrol)
{
	String response;
	response = "";
	response += "<p>";
	response += "ID: " + String(_conrtrol->Id);
	response += "&nbsp;";
	response += _conrtrol->Name;
	response += "&nbsp;";
	response += "<a href=\"?";
	response += _conrtrol->Id;	
	response += "=" + _conrtrol->Command + "On" + "\">";
	response += "<button>einschalten</button>";
	response += "</a>";
	response += "&nbsp;";
	response += "<a href=\"?";
	response += _conrtrol->Id;
	response += "=" + _conrtrol->Command + "Off" + "\">";
	response += "<button>ausschalten</button>";
	response += "</a>";
	response += "&nbsp;";
	response += "<FONT SIZE=-1>";
	response += _conrtrol->Description;
	response += "<FONT SIZE=+1>";
	response += "<BR>";
	return response;
}

String WebServer_Wifi_Device_Driver::GenerateButton(Control *_conrtrol)
{
	String response;
	response = "";
	response += "<p>";
	response += "ID: " + String(_conrtrol->Id);
	response += "&nbsp;";
	response += _conrtrol->Name;
	response += "&nbsp;";
	response += "<a href=\"?";
	response += _conrtrol->Id;
	response += "=";
	response += _conrtrol->Command;
	response += "\">";
	response += "<button>Push</button>";
	response += "</a>";
	response += "&nbsp;";
	response += "<FONT SIZE=-1>";
	response += _conrtrol->Description;
	response += "<FONT SIZE=+1>";
	response += "<BR>";
	return response;
}

void WebServer_Wifi_Device_Driver::DoExecuteCommand(String _command)
{
}



void WebServer_Wifi_Device_Driver::FillCtrl(String requestpart) {
	int iEnd;
	uint16_t Key;
	String Value;

	iEnd = requestpart.indexOf("=");
	Key = atoi(requestpart.substring(0, iEnd).c_str());
	Serial.print("Key: ");
	Serial.println(Key);
	Value = requestpart.substring(iEnd + 1, requestpart.length());
	Serial.print("Value: ");
	Serial.println(Value);

	ServerMessage* message = new ServerMessage(Key, Value);
	if (!parentModule->SendAsyncThreadMessage(message))
	{
		Serial.println(">> message buffer overflow <<");
	}
}

void WebServer_Wifi_Device_Driver::ParseRequest(String _request) {
	String sGetstart = "GET ";
	String get_params;

	int iStart, iEnd;
	iStart = _request.indexOf(sGetstart);
	if (iStart >= 0)
	{
		//nach GET
		iStart += +sGetstart.length();
		iEnd = _request.indexOf("?", iStart);
		if (iEnd >= 0)
		{
			iStart = iEnd;
			iEnd = _request.indexOf(" ", iStart);
			get_params = _request.substring(iStart + 1, iEnd);
			String dataPart = "";
			for (int i = 0; i < get_params.length(); i++)
			{
				if (get_params[i] == '&') {
					FillCtrl(dataPart);
					dataPart = "";
				}
				else
					dataPart.concat(get_params[i]);
			}
			FillCtrl(dataPart);
		}
	}
}
