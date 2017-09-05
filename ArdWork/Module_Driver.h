#ifndef _MODULE_DRIVER_h
#define _MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "m_Vector.h"

#include "Driver.h"
#include "Module_Driver_Consts.h"

class Device_Driver;
class Led_Device_Driver;
class Button_Device_Driver;
class Uart_GRBW_Led_Device_Driver;
class Luxmeter_Device_Driver;
class Distance_Device_Driver;
class OLED_Display_Device_Driver;
class WebServer_Wifi_Device_Driver;
class Mqqt_Wifi_Device_Driver;
class Temperature_Device_Driver;
class WebSocket_Wifi_Device_Driver;

class Module_Driver : public Driver, public Observee
{
private:
	bool isdebug;
	Vector<Publisher*> *pub_List;
	Vector<ThreadMessage*> queue;

	int8_t button_index;
	int8_t distance_index;
	int8_t led_index;
	int8_t luxmeter_index;
	int8_t mqqt_wifi_index;
	int8_t oled_display_index;
	int8_t temperature_index;
	int8_t Uart_GRBW_Led_index;
	int8_t webserver_wifi_index;
	int8_t websocket_wifi_index;

	Vector <Button_Device_Driver*> *button_list;
	Vector <Distance_Device_Driver*> *distance_list;
	Vector <Led_Device_Driver*> *led_list;
	Vector <Luxmeter_Device_Driver*> *luxmeter_list;
	Vector <Mqqt_Wifi_Device_Driver*> *mqqt_wifi_list;
	Vector <OLED_Display_Device_Driver*> *oled_display_list;
	Vector <Temperature_Device_Driver*> *temperature_list;
	Vector <Uart_GRBW_Led_Device_Driver*> *Uart_GRBW_Led_list;
	Vector <WebServer_Wifi_Device_Driver*> *webserver_wifi_list;
	Vector <WebSocket_Wifi_Device_Driver*> *websocket_wifi_list;

	Button_Device_Driver* Get_Selected_Button_Device() const;
	Distance_Device_Driver* Get_Selected_Distance_Device() const;
	Led_Device_Driver* Get_Selected_Led_Device() const;
	Luxmeter_Device_Driver* Get_Selected_Luxmeter_Device() const;
	Mqqt_Wifi_Device_Driver* Get_Selected_Mqqt_Wifi_Device() const;
	OLED_Display_Device_Driver* Get_Selected_OLED_Display_Device() const;
	Temperature_Device_Driver* Get_Selected_Temperature_Device() const;
	Uart_GRBW_Led_Device_Driver* Get_Selected_Uart_GRBW_Led_Device() const;
	WebServer_Wifi_Device_Driver* Get_Selected_WebServer_Wifi_Device() const;
	WebSocket_Wifi_Device_Driver* Get_Selected_WebSocket_Wifi_Device() const;

	void DoUpdate(uint32_t deltaTime);
	void UpdateControls();
protected:
	Vector <Driver*> *device_list;
	Vector <Publisher*> *GetPublisherList();
	Property<Button_Device_Driver*, Module_Driver> Selected_Button_Device{ this, nullptr, &Module_Driver::Get_Selected_Button_Device };
	Property<Distance_Device_Driver*, Module_Driver> Selected_Distance_Device{ this, nullptr, &Module_Driver::Get_Selected_Distance_Device };
	Property<Led_Device_Driver*, Module_Driver> Selected_Led_Device{ this, nullptr, &Module_Driver::Get_Selected_Led_Device };
	Property<Luxmeter_Device_Driver*, Module_Driver> Selected_Luxmeter_Device{ this, nullptr, &Module_Driver::Get_Selected_Luxmeter_Device };
	Property<Mqqt_Wifi_Device_Driver*, Module_Driver> Selected_Mqqt_Wifi_Device{ this, nullptr, &Module_Driver::Get_Selected_Mqqt_Wifi_Device };
	Property<OLED_Display_Device_Driver*, Module_Driver> Selected_OLED_Display_Device{ this, nullptr, &Module_Driver::Get_Selected_OLED_Display_Device };
	Property<Temperature_Device_Driver*, Module_Driver> Selected_Temperature_Device{ this, nullptr, &Module_Driver::Get_Selected_Temperature_Device };
	Property<Uart_GRBW_Led_Device_Driver*, Module_Driver> Selected_Uart_GRBW_Led_Device{ this, nullptr, &Module_Driver::Get_Selected_Uart_GRBW_Led_Device };
	Property<WebServer_Wifi_Device_Driver*, Module_Driver> Selected_WebServer_Wifi_Device{ this, nullptr, &Module_Driver::Get_Selected_WebServer_Wifi_Device };
	Property<WebSocket_Wifi_Device_Driver*, Module_Driver> Selected_WebSocket_Wifi_Device{ this, nullptr, &Module_Driver::Get_Selected_WebSocket_Wifi_Device };

	Driver *GetDeviceById(int Id);

	void DoMessage(Int_Thread_Msg message);
	void DoInit();
	void DoShutdown();
	void DoSuspend();
	bool PopMessage(ThreadMessage** message);

	void Set_Debug_On();
	void Set_Debug_Off();
 protected:
	 virtual void DoBeforeSuspend() = 0;
	 virtual void DoBeforeShutdown() = 0;
	 virtual void DoAfterInit() = 0;
	 virtual void DoModuleMessage(Int_Thread_Msg message) = 0;
	 virtual void DoThreadMessage(ThreadMessage *message) = 0;
public:
	Module_Driver(uint8_t priority = THREAD_PRIORITY_NORMAL);
	~Module_Driver();
	bool SendAsyncThreadMessage(ThreadMessage* message, bool withinIsr = false);
	void AddDevice(Device_Driver* device);

	void Exec_Set_Debug_On();
	void Exec_Set_Debug_Off();
};

#endif
