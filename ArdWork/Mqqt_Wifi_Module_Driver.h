// Mqqt_Wifi_Module_Driver.h

#ifndef _MQQT_WIFI_MODULE_DRIVER_h
#define _MQQT_WIFI_MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wifi_Module_Driver.h"
#include "m_Vector.h"
#include <PubSubClient.h>

class Mqqt_Wifi_Module_Driver : public WiFi_Module_Driver
{
public:
	Mqqt_Wifi_Module_Driver(String _hostname, String _ssid, String _password, uint8_t priority = THREAD_PRIORITY_NORMAL);

private:
	uint8 connRetry;
	String outTopic;
	String inTopic;
	IPAddress mqqt_broker;

	WiFiClient *espClient;
	PubSubClient *client;
	unsigned int value;
	uint16_t comm_delta;
	uint16_t comm_delay;
	bool isCommConnected;

	static String mqtt_message;
	static void EventMsgIn(char* topic, uint8_t* payload, unsigned int length);
protected:
	Vector <String*> msg_list;
	void SetupComm();
	void UpdateComm(uint32_t deltaTime);
	void CheckComm(uint32_t deltaTime);

	virtual void DoMQTTMessage(String message) = 0;
public:
	void SetOutTopic(String topic);
	void SetInTopic(String topic);
	void SetMQQTBroker(String adress);
};




#endif

