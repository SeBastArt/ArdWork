// 
// 
// 

#include "Mqqt_Wifi_Device_Driver.h"

String Mqqt_Wifi_Device_Driver::mqtt_message = ""; // initializer

Mqqt_Wifi_Device_Driver::Mqqt_Wifi_Device_Driver(Module_Driver * module, String _ssid, String _password, Led_Device_Driver * _statusLED, uint8_t priority) : 
	Wifi_Device_Driver(module, _ssid, _password, _statusLED, priority)
{
	driver_name = "Mqqt_Wifi_Device_Driver";

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
	__descriptor->name = "Luxmeter";
	__descriptor->descr = "messeaure brightness";
}


void Mqqt_Wifi_Device_Driver::EventMsgIn(char* topic, uint8_t* payload, unsigned int length) {
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

void Mqqt_Wifi_Device_Driver::DoExecuteCommand(String _command)
{
}

void Mqqt_Wifi_Device_Driver::SetMQQTBroker(String adress) {
	Serial.print("Set MQQT-Broker to: ");
	Serial.println(adress);
	mqqt_broker.fromString(adress);
}

void Mqqt_Wifi_Device_Driver::SendMessage(String msg)
{
	String *_msg = new String(msg);
	msg_list.PushBack(_msg);
}

void Mqqt_Wifi_Device_Driver::UpdateControls()
{
}

void Mqqt_Wifi_Device_Driver::SetOutTopic(String topic) {
	Serial.print("Set Mqqt-Server PublicTopic to: ");
	Serial.println(topic);
	outTopic = topic;
}

void Mqqt_Wifi_Device_Driver::SetInTopic(String topic) {
	
	Serial.print("SubScripe Mqqt-Server to: ");
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
	Serial.print("Connect to MQTT FHEM...");
	if (client->connect(&hostname[0])) {
		Serial.println("connected");
		// ... and resubscribe
		client->subscribe(&inTopic[0]);
		isCommConnected = true;
		statusLED->Exec_Set_IO_Pin_Low();
		connRetry = 0;
	}
	else {
		statusLED->Exec_Set_IO_Pin_High();
		Serial.print("failed, rc=");
		Serial.print(client->state());
		Serial.println(" try again in 5 seconds");
	}
}

void Mqqt_Wifi_Device_Driver::UpdateComm(uint32_t deltaTime) {
	CheckComm(deltaTime);
	if (client->connected()) {
		client->loop();
		if (mqtt_message != "") {
			MqqtMessage* message = new MqqtMessage("Teste mal das hier");
			if (!parentModule->SendAsyncThreadMessage(message))
			{
				Serial.println(">> message buffer overflow <<");
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
		statusLED->Exec_Set_IO_Pin_High();
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
				InitComm();
			}
		}
	}

}