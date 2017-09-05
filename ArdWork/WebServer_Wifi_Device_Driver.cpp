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
	sResponse = "<body>";
	sResponse += "<font color=\"#000000\"><body bgcolor=\"#d0d0f0\">";
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Demo f&uumlr ESP8266 Steuerung</h1>";
	sResponse += "<FONT size=+1>";

	sResponse += GenerateControlElemnts();

	sResponse += "<FONT SIZE=-2>";
	sResponse += "<BR>Aufrufz&auml;hler=";
	sResponse += ulReqcount;
	sResponse += " - Verbindungsz&auml;hler=";
	sResponse += ulReconncount;
	sResponse += "</body></html>";

	sHeader = GenerateHeader(sResponse, true);


	// Send the response to the client
	SendResponse(client, sHeader, sResponse);
}


void WebServer_Wifi_Device_Driver::CheckComm(WiFiClient _client) {
	// Check if a client has connected
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
	Serial.println(sRequest);
	ParseRequest(sRequest);
}


String WebServer_Wifi_Device_Driver::GenerateHeader(String _response, bool _found)
{
	String sHeader;
	sHeader = (_found == true) ? "HTTP/1.1 200 OK\r\n" : "HTTP/1.1 404 Not found\r\n";
	sHeader += "Content-Length: ";
	sHeader += _response.length();
	sHeader += "Content-Type: text/html";
	sHeader += "Connection: close";  // the connection will be closed after completion of the response
	//sHeader += "Refresh: 5";  // refresh the page automatically every 5 sec
	sHeader += "\r\n";
	sHeader += "<!DOCTYPE HTML>";
	sHeader += "<html>";
	sHeader += "<head>";
	sHeader += "<title>Adafruit HUZZAH ESP8266</title>";

	sHeader += "\r\n";
	sHeader += "\r\n";

	sHeader += GenerateCSS();
	sHeader += GenerateJavaScript();
	sHeader += "</head>";
	return sHeader;
}

String WebServer_Wifi_Device_Driver::GenerateJavaScript() {
	String sScript;

	sScript += "<script>";
	if (!__pub_list->Empty()) {
		for (int I = 0; I < __pub_list->Size(); I++) {
			for (int j = 0; j < (*__pub_list)[I]->elem_count; j++) {
				/*if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Button_Publisher")) {
					sScript += GenerateButton((*__pub_list)[I]->driverId, (Button_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}
				if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Value_Publisher")) {
					sScript += GenerateValue((*__pub_list)[I]->driverId, (Value_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}*/
				if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Switch_Publisher")) {
					sScript += GenerateSwitchJS((*__pub_list)[I]->driverId, (Switch_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}
			}
		}
	}
	sScript += "</script>";
	return sScript;
}

String WebServer_Wifi_Device_Driver::GenerateCSS() {
	String sStyle;

	sStyle += "<style type='text/css'>";

	//Styles for on/off switch
	sStyle += ".onoffswitch { position: relative; width: 90px; -webkit-user-select:none; -moz-user-select:none; -ms-user-select: none; }";
	sStyle += ".onoffswitch-checkbox { display: none; }";
	sStyle += ".onoffswitch-label { display: block; overflow: hidden; cursor: pointer; border: 2px solid #999999; border-radius: 20px; }";
	sStyle += ".onoffswitch-inner { display: block; width: 200%; margin-left: -100%; transition: margin 0.3s ease-in 0s;}";
	sStyle += ".onoffswitch-inner:before, .onoffswitch-inner:after { display: block; float: left; width: 50%; height: 30px; padding: 0; line-height: 30px; font-size: 14px; color: white; font-family: Trebuchet, Arial, sans-serif; font-weight: bold; box-sizing: border-box; }";
	sStyle += ".onoffswitch-inner:before { content: 'ON'; padding-left: 10px; background-color: #34A7C1; color: #FFFFFF; }";
	sStyle += ".onoffswitch-inner:after { content: 'OFF'; padding-right: 10px; background-color: #EEEEEE; color: #999999; text-align: right; }";
	sStyle += ".onoffswitch-switch { display: block; width: 18px; margin: 6px; background: #FFFFFF; position: absolute; top: 0; bottom: 0; right: 56px; border: 2px solid #999999; border-radius: 20px; transition: all 0.3s ease-in 0s; }";
	sStyle += ".onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-inner { margin-left: 0; }";
	sStyle += ".onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-switch { right: 0px; }";
	sStyle += "</style>";

	return sStyle;
}

void WebServer_Wifi_Device_Driver::SendResponse(WiFiClient _client, String _header, String _response)
{
	_client.print(_header);
	_client.print(_response); 

	// and stop the client
	_client.stop();
	Serial.println("Client disonnected");
}



String WebServer_Wifi_Device_Driver::GenerateControlElemnts() {
	String sResponse;
	if (!__pub_list->Empty()) {
		for (int I = 0; I < __pub_list->Size(); I++) {
			for (int j = 0; j < (*__pub_list)[I]->elem_count; j++) {
				if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Button_Publisher")) {
					sResponse += GenerateButton((*__pub_list)[I]->driverId, (Button_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}
				if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Value_Publisher")) {
					sResponse += GenerateValue((*__pub_list)[I]->driverId, (Value_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}
				if ((*__pub_list)[I]->GetElemByIndex(j)->GetClassName().equals("Switch_Publisher")) {
					sResponse += GenerateSwitchHtml((*__pub_list)[I]->driverId, (Switch_Publisher*)(*__pub_list)[I]->GetElemByIndex(j));
				}
			}
		}
	}
	return sResponse;
}

String WebServer_Wifi_Device_Driver::GenerateValue(int _driverId, Value_Publisher* _pub_elem)
{
	String response;
	response = "";
	response += "<p>";
	response += "ID: " + String(_driverId);
	response += "&nbsp;";
	response += " Wert = " + String(_pub_elem->ValueToString());
	response += "&nbsp;";
	response += "<FONT SIZE=-1>";
	response += _pub_elem->descr;
	response += "<FONT SIZE=+1>";
	response += "<BR>";
	return response;
}

String WebServer_Wifi_Device_Driver::GenerateSwitchJS(int _driverId, Switch_Publisher * _pub_elem) {
	String sScript;

	//sScript += "function LEDswitch(){";
	sScript += "function ";
	sScript += "Func";
	sScript += _pub_elem->lable;
	sScript += "(){";
	//sScript += "var LEDswitchCheck = document.getElementById('myonoffswitch').checked;";
	sScript += "var SwitchCheck = document.getElementById('";
	sScript += _pub_elem->lable + "switch').checked; ";
	sScript += "if(SwitchCheck){";
	sScript += "window.location.href = \"?";
	sScript += StrDeviceId;
	sScript += "=";
	sScript += _driverId;
	sScript += "&";
	sScript += StrCmdId;
	sScript += "=";
	sScript += _pub_elem->cmdOnId;
	sScript += "\"";
	sScript += "}";
	sScript += "else {";
	sScript += "window.location.href = \"?";
	sScript += StrDeviceId;
	sScript += "=";
	sScript += _driverId;
	sScript += "&";
	sScript += StrCmdId;
	sScript += "=";
	sScript += _pub_elem->cmdOffId;
	sScript += "\"";
	sScript += "}";
	sScript += "}";
	return sScript;
}

String WebServer_Wifi_Device_Driver::GenerateSwitchHtml(int _driverId, Switch_Publisher * _pub_elem)
{
	String sSwitch;
	sSwitch += "<p>";
	sSwitch += "ID: " + String(_driverId);
	sSwitch += "&nbsp;";
	sSwitch += "Lable: " + _pub_elem->lable;
	sSwitch += "&nbsp;";
	sSwitch += "<div class='onoffswitch'>";

	if (*(_pub_elem->status) == true)
	{
		//sSwitch += "<input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='myonoffswitch' checked='checked' onclick='LEDswitch()'>";
		sSwitch += "<input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='";
		sSwitch += _pub_elem->lable + "switch' ";
		sSwitch += "checked = 'checked' onclick = '";
		sSwitch += "Func";
		sSwitch += _pub_elem->lable;
		sSwitch += "()'>";
	} //end if
	else if (*(_pub_elem->status) == false)
	{
		//sSwitch += "<input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='myonoffswitch' onclick='LEDswitch()'>";
		sSwitch += "<input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='";
		sSwitch += _pub_elem->lable + "switch' ";
		sSwitch += " onclick='";
		sSwitch += "Func";
		sSwitch += _pub_elem->lable;
		sSwitch += "()'>";
	} //end else

	//sSwitch += "<label class='onoffswitch-label' for='myonoffswitch'>";
	sSwitch += "<label class='onoffswitch-label' for='";
	sSwitch += _pub_elem->lable + "switch'>";
	sSwitch += "<span class='onoffswitch-inner'></span>";
	sSwitch += "<span class='onoffswitch-switch'></span>";
	sSwitch += "</label>";
	sSwitch += "</div>";
	sSwitch += "&nbsp;";
	sSwitch += "<FONT SIZE=-1>";
	sSwitch += _pub_elem->descr;
	sSwitch += "<FONT SIZE=+1>";
	return sSwitch;
}

String WebServer_Wifi_Device_Driver::GenerateButton(int _driverId, Button_Publisher * _pub_elem)
{
	String response;
	response = "";
	response += "<p>";
	response += "ID: " + String(_driverId);
	response += "&nbsp;";
	response += "Lable: " + _pub_elem->lable;
	response += "&nbsp;";
	response += _pub_elem->lable;
	response += "&nbsp;";
	response += "<a href=\"?";
	response += StrDeviceId;
	response += "=";
	response += _driverId;
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
	Key = requestpart.substring(0, iEnd).c_str();
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

	int deviceId = -1;
	int cmdId = -1;
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
			Serial.print("get_params: ");
			Serial.println(get_params);
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
						if (Values.length() > 0) {
							Values += ':';
						}
						Values += GetValue(dataPart);
					}
					dataPart = "";
				}
				else
					dataPart.concat(get_params[i]);
			}
			if (GetKey(dataPart).equals(StrDeviceId)) {
				deviceId = atoi(GetValue(dataPart).c_str());
			}
			else if (GetKey(dataPart).equals(StrCmdId)) {
				cmdId = atoi(GetValue(dataPart).c_str());
			}
			else {
				Values += GetValue(dataPart);
			}

			Serial.print("deviceId: ");
			Serial.print(deviceId);
			Serial.print(", cmdId: ");
			Serial.print(cmdId);
			Serial.print(", Values: ");
			Serial.println(Values);
			CommunicationMessage* message = new CommunicationMessage(deviceId, cmdId, Values);
			if (!parentModule->SendAsyncThreadMessage(message))
			{
				Serial.println(">> message buffer overflow <<");
			}
		}
	}
}
