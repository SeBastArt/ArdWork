// 
// 
// 

#include "Mqqt_Wifi_Device_Driver.h"

String Mqqt_Wifi_Device_Driver::mqtt_message = ""; // initializer

Mqqt_Wifi_Device_Driver::Mqqt_Wifi_Device_Driver(Module_Driver * module, Led_Device_Driver * _statusLED, uint8_t priority) : 
	Wifi_Device_Driver(module, _statusLED, priority)
{
	__DriverType = MQQT_WIFI_DEVICE_DRIVER_TYPE;

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

void Mqqt_Wifi_Device_Driver::Build_Descriptor() {
	__descriptor->name = F("Mqqt Broker");
	__descriptor->descr = F("provide moquitto server-client communication");
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


void Mqqt_Wifi_Device_Driver::InitComm() {
	client->setServer(mqqt_broker, 1883);
	Serial.print(F("Connect to MQTT FHEM..."));
	if (client->connect(&hostname[0])) {
		Serial.println(F("connected"));
		// ... and resubscribe
		client->subscribe(&inTopic[0]);
		isCommConnected = true;
		statusLED->Exec_Set_Led_Off();
		connRetry = 0;
	}
	else {
		statusLED->Exec_Set_Led_On();
		Serial.print("failed, rc=");
		Serial.print(client->state());
		Serial.println(F(" try again in 5 seconds"));
	}
}

void Mqqt_Wifi_Device_Driver::UpdateComm(uint32_t deltaTime) {
	CheckComm(deltaTime);
	if (client->connected()) {
		client->loop();
		if (mqtt_message != "") {
			MqqtMessage* message = new MqqtMessage("Teste mal das hier");
			if (!parentModule->SendAsyncTaskMessage(message))
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

void Mqqt_Wifi_Device_Driver::CheckComm(uint32_t deltaTime) {
	if (!isCommConnected) {
		statusLED->Exec_Set_Led_On();
	}
	if (!client->connected()) {
		if (connRetry < 3) {
			if (isCommConnected) {
				connRetry = 0;
				Serial.println(F("Lost Connection to MQQT Server..."));
				Serial.println(F("Try to reconnect..."));
				isCommConnected = false;
			}
			comm_delta += deltaTime;
			if (comm_delta > comm_delay) {
				comm_delta = 0;
				connRetry++;
				InitComm();
			}
		}
	}

}

