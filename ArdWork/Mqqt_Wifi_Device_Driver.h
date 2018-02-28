// Mqqt_Wifi_Device_Driver.h

#ifndef _MQQT_WIFI_DEVICE_DRIVER_h
#define _MQQT_WIFI_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Wifi_Device_Driver.h"
#include "Mqqt_Wifi_Device_Driver_Consts.h"
#include "m_Vector.h"
#include <PubSubClient.h>

class Mqqt_Wifi_Device_Driver : public Device_Driver, public CommunicationClient
{
	REGISTER(Mqqt_Wifi_Device_Driver);
public:
	Mqqt_Wifi_Device_Driver(Module_Driver* module, uint8_t priority = TASK_PRIORITY_NORMAL);
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
	void DoUpdate(uint32_t deltaTime);
	void DoDeviceMessage(Int_Task_Msg message);
	void OnNotifyConnected() override;
	void OnNotifyConnectionLost() override;
	void OnNotifyOnline() override;
public:
	void SetOutTopic(String topic);
	void SetInTopic(String topic);
	void SetMQQTBroker(String adress);
	void SendMessage(String msg);

};


#endif


