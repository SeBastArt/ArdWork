// Mqqt_Wifi_Device_Driver.h

#ifndef _MQQT_WIFI_DEVICE_DRIVER_h
#define _MQQT_WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Wifi_Device_Driver.h"
#include "m_Vector.h"
#include <PubSubClient.h>

class Mqqt_Wifi_Device_Driver : public Wifi_Device_Driver
{
public:
	Mqqt_Wifi_Device_Driver(Module_Driver* module, String _ssid, String _password, Led_Device_Driver *_statusLED = NULL, uint8_t priority = THREAD_PRIORITY_NORMAL);
	void UpdateControls();
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

	void DoExecuteCommand(String _command);
protected:
	Vector <String*> msg_list;
	void UpdateComm(uint32_t deltaTime);
	void CheckComm(uint32_t deltaTime);
	void InitComm();
	void Build_Descriptor();
public:
	void SetOutTopic(String topic);
	void SetInTopic(String topic);
	void SetMQQTBroker(String adress);
	void SendMessage(String msg);

};


#endif

