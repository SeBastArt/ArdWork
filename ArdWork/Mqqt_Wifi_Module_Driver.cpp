// 
// 
// 

#include "Mqqt_Wifi_Module_Driver.h"
#include "Led_Device_Driver.h"

String Mqqt_Wifi_Module_Driver::mqtt_message = ""; // initializer

Mqqt_Wifi_Module_Driver::Mqqt_Wifi_Module_Driver(String _hostname, String _ssid, String _password, uint8_t priority) : WiFi_Module_Driver(_hostname, _ssid, _password, priority)
{
	outTopic = "/";
	inTopic = "/";
	value = 0;
	mqqt_broker.fromString("0.0.0.0");
	comm_delay = 5000;
	comm_delta = 0;
	isCommConnected = false;
	connRetry = 0;
	espClient = new WiFiClient;
	client = new PubSubClient(*espClient);
	client->setCallback(EventMsgIn);
}


void Mqqt_Wifi_Module_Driver::EventMsgIn(char* topic, uint8_t* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	Serial.print("Payload: ");
	mqtt_message = "";
	for (int i = 0; i < length; i++) {
		mqtt_message += (char)payload[i];
	}
	Serial.println(mqtt_message);
}

void Mqqt_Wifi_Module_Driver::SetMQQTBroker(String adress) {
	mqqt_broker.fromString(adress);
}

void Mqqt_Wifi_Module_Driver::SetOutTopic(String topic) {
	outTopic = topic;
}

void Mqqt_Wifi_Module_Driver::SetInTopic(String topic) {
	inTopic = topic;
	if (WiFi.status() == WL_CONNECTED) {
		if (client->connected()) {
			client->subscribe(&inTopic[0]);
		}
	}
}

void Mqqt_Wifi_Module_Driver::SetupComm() {
		client->setServer(mqqt_broker, 1883);
		Serial.print("Connect to MQTT FHEM...");
		if (client->connect(&hostname[0])) {
			Serial.println("connected");
			// ... and resubscribe
			client->subscribe(&inTopic[0]);
			isCommConnected = true;
			wifi_status_Led->Exec_Set_Led_Off();
			connRetry = 0;
		}
		else {
			wifi_status_Led->Exec_Set_Led_On();
			Serial.print("failed, rc=");
			Serial.print(client->state());
			Serial.println(" try again in 5 seconds");
		}
}

void Mqqt_Wifi_Module_Driver::UpdateComm(uint32_t deltaTime) {
	char msg[50];
	if (client->connected()) {
		client->loop();
		if (mqtt_message != "") {
			DoMQTTMessage(mqtt_message);
			mqtt_message = "";
		}
		while (!msg_list.Empty()) {
			String *p_msg = msg_list.Front();
			String msg = *p_msg;
			client->publish(&outTopic[0], &msg[0]);
			msg_list.PopBack();
		}
	}
}

void Mqqt_Wifi_Module_Driver::CheckComm(uint32_t deltaTime) {
	if (!isCommConnected) {
		wifi_status_Led->Exec_Set_Led_On();
	}
	if (!client->connected()) {
		if (connRetry < 3) {
			if (isCommConnected) {
				connRetry = 0;
				Serial.println("Lost Connection to MQQT Server...");
				Serial.println("Try to reconnect...");
				isCommConnected = false;
			}
			comm_delta += deltaTime;
			if (comm_delta > comm_delay) {
				comm_delta = 0;
				connRetry++;
				SetupComm();
			}
		}
	}
	
}
