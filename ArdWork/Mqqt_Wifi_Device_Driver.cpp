// 
// 
// 

#include "Mqqt_Wifi_Device_Driver.h"

String Mqqt_Wifi_Device_Driver::mqtt_message = ""; // initializer

Mqqt_Wifi_Device_Driver::Mqqt_Wifi_Device_Driver(Module_Driver * module, uint8_t priority) : 
	Device_Driver(module, priority)
{
	__DriverType = MQQT_WIFI_DEVICE_DRIVER_TYPE;

	outTopic = "/";
	inTopic = "/";
	value = 0;
	mqqt_broker.fromString("0.0.0.0");
	comm_delay = 5000;
	comm_delta = 0;
	connRetry = 0;
	espClient = new WiFiClient;
	client = new PubSubClient(*espClient);
	client->setCallback(EventMsgIn);
}

void Mqqt_Wifi_Device_Driver::EventMsgIn(char* topic, uint8_t* payload, unsigned int length) {
	Serial.print(F("Message arrived ["));
	Serial.print(topic);
	Serial.print("] ");
	Serial.print(F("Payload: "));
	mqtt_message = "";
	for (int i = 0; i < length; i++) {
		mqtt_message += (char)payload[i];
	}
	Serial.println(mqtt_message);
}

void Mqqt_Wifi_Device_Driver::SetMQQTBroker(String adress) {
	Serial.print(F("Set MQQT-Broker to: "));
	Serial.println(adress);
	mqqt_broker.fromString(adress);
}

void Mqqt_Wifi_Device_Driver::SendMessage(String msg)
{
	String *_msg = new String(msg);
	msg_list.PushBack(_msg);
}

void Mqqt_Wifi_Device_Driver::SetOutTopic(String topic) {
	Serial.print(F("Set Mqqt-Server PublicTopic to: "));
	Serial.println(topic);
	outTopic = topic;
}

void Mqqt_Wifi_Device_Driver::SetInTopic(String topic) {
	
	Serial.print(F("SubScripe Mqqt-Server to: "));
	Serial.println(topic);
	inTopic = topic;
	if (WiFi.status() == WL_CONNECTED) {
		if (client->connected()) {
			client->subscribe(&inTopic[0]);
		}
	}
}


void Mqqt_Wifi_Device_Driver::OnNotifyConnected() {
	client->setServer(mqqt_broker, 1883);
	Serial.print(F("Connect to MQTT FHEM..."));
	if (client->connect("ESP8266")) {
		Serial.println(F("connected"));
		// ... and resubscribe
		client->subscribe(&inTopic[0]);
		connRetry = 0;
	}
	else {
		Serial.print("failed, rc=");
		Serial.print(client->state());
		Serial.println(F(" try again in 5 seconds"));
	}
}

void Mqqt_Wifi_Device_Driver::OnNotifyConnectionLost()
{
}

void Mqqt_Wifi_Device_Driver::DoUpdate(uint32_t deltaTime) {
	if (__isOnline) {
		if (client->connected()) {
			client->loop();
			if (mqtt_message != "") {
				MqqtMessage* message = new MqqtMessage("Teste mal das hier");
				if (!__parentModule->SendAsyncTaskMessage(message))
				{
					Serial.println(F(">> message buffer overflow <<"));
				}
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
}

void Mqqt_Wifi_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
	switch (messageID)
	{
	default:
		break;
	}
}




