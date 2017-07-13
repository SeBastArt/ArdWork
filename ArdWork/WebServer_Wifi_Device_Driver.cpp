// 
// 
// 

#include "WebServer_Wifi_Device_Driver.h"

#define StrDeviceId "Id"
#define StrCmdId "cmdId"

WebServer_Wifi_Device_Driver::WebServer_Wifi_Device_Driver(Module_Driver * module, String _ssid, String _password, Led_Device_Driver * _statusLED, uint8_t priority) :
	Wifi_Device_Driver(module, _ssid, _password, _statusLED, priority)
{
	driver_name = "WebServer_Wifi_Device_Driver";
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

	if (!__pub_list->Empty()) {
		for (int I = 0; I < __pub_list->Size(); I++) {
			for (int j = 0; j < (*__pub_list)[I]->elem_count; j++) {
				if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Button_Publisher")) {
					sResponse += GenerateButton((*__pub_list)[I]->id, (Button_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}
				if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Value_Publisher")) {
					sResponse += GenerateValue((*__pub_list)[I]->id, (Value_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}
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


String WebServer_Wifi_Device_Driver::GenerateValue(uint16_t _deviceId, Value_Publisher * _pub_elem)
{
	String response;
	response = "";
	response += "<p>";
	response += "ID: " + String(_deviceId);
	response += "&nbsp;";
	response += _pub_elem->cmdId;
	response += "&nbsp;";
	response += " Wert = " + String(*_pub_elem->value);
	response += "&nbsp;";
	response += "<FONT SIZE=-1>";
	response += _pub_elem->descr;
	response += "<FONT SIZE=+1>";
	response += "<BR>";
	return response;
}

String WebServer_Wifi_Device_Driver::GenerateSwitch(uint16_t _deviceId, Switch_Publisher * _pub_elem)
{
	String response;
	response = "";
	response += "<p>";
	response += "DeviceID: " + String(_deviceId);
	response += "&nbsp;";
	response += _pub_elem->lable;
	response += "&nbsp;";
	response += "<a href=\"?";
	response += StrDeviceId;
	response += "=" + _pub_elem->cmdOn + "\">";
	response += "<button>einschalten</button>";
	response += "</a>";
	response += "&nbsp;";
	response += "<a href=\"?";
	response += _id;
	response += "=" + _pub_elem->cmdOff + "\">";
	response += "<button>ausschalten</button>";
	response += "</a>";
	response += "&nbsp;";
	response += "<FONT SIZE=-1>";
	response += _pub_elem->descr;
	response += "<FONT SIZE=+1>";
	response += "<BR>";
	return response;
}

String WebServer_Wifi_Device_Driver::GenerateButton(uint16_t _deviceId, Button_Publisher * _pub_elem)
{
	String response;
	response = "";
	response += "<p>";
	response += "ID: " + String(_id);
	response += "&nbsp;";
	response += _pub_elem->lable;
	response += "&nbsp;";
	response += "<a href=\"?";
	response += StrDeviceId;
	response += "=";
	response += _id;
	response += "&";
	response += StrCmdId;
	response += "=";
	response += _pub_elem->cmdId;
	response += "\">";
	response += "<button>" + String(_pub_elem->lable) + "</button>";
	response += "</a>";
	response += "&nbsp;";
	response += "<FONT SIZE=-1>";
	response += _pub_elem->descr;
	response += "<FONT SIZE=+1>";
	response += "<BR>";
	return response;
}


String WebServer_Wifi_Device_Driver::GetKey(String requestpart) {
	int iEnd;
	String Key;
	iEnd = requestpart.indexOf("=");
	Key = requestpart.substring(0, iEnd).c_str());
	return Key;
}

String WebServer_Wifi_Device_Driver::GetValue(String requestpart) {
	int iEnd;
	String Value;
	iEnd = requestpart.indexOf("=");
	Value = requestpart.substring(iEnd + 1, requestpart.length());
	return Value;
}

void WebServer_Wifi_Device_Driver::ParseRequest(String _request) {
	String sGetstart = "GET ";
	String get_params;

	uint16 deviceId = -1;
	uint16 cmdId = -1;
	String Values = "";

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
					if (GetKey(dataPart).equals(StrDeviceId)) {
						deviceId = atoi(GetValue(dataPart).c_str());
					}
					else if (GetKey(dataPart).equals(StrCmdId)) {
						cmdId = atoi(GetValue(dataPart).c_str());
					}
					else {
						Values += GetValue(dataPart) + ':';
					}
					dataPart = "";
				}
				else
					dataPart.concat(get_params[i]);
			}
			Values += GetValue(dataPart);

			CommunicationMessage* message = new CommunicationMessage(deviceId, cmdId, Values);
			if (!parentModule->SendAsyncThreadMessage(message))
			{
				Serial.println(">> message buffer overflow <<");
			}
		}
	}
}
