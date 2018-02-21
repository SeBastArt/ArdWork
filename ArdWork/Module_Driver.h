#ifndef _MODULE_DRIVER_h
#define _MODULE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <string>
#include <map>
#include "m_Vector.h"

#include "Driver.h"
#include "Module_Driver_Consts.h"
#include "CreatorImpl.h"

class Creator;
class Device_Driver;
class Led_Device_Driver;
class Button_Device_Driver;
class Uart_GRBW_Led_Device_Driver;
class Uart_RGB_Led_Device_Driver;
class Luxmeter_Device_Driver;
class Distance_Device_Driver;
class OLED_Display_Device_Driver;
class Mqqt_Wifi_Device_Driver;
class Temperature_Device_Driver;
class WebSocket_Wifi_Device_Driver;
class Ntp_Wifi_Device_Driver;
class GPS_Device_Driver;

class Module_Driver : public Driver, public Observer
{
	friend class Device_Driver;
private:
	int __isdebug;
	Vector<TaskMessage*> queue;
	void DoUpdate(uint32_t deltaTime);
	static std::map<std::string, Creator*>& get_table();
protected:
	Vector <Driver*> *device_list;

	Button_Device_Driver * Get_Button_DevDrv(uint8_t _index);
	Distance_Device_Driver * Get_Distance_DevDrv(uint8_t _index);
	Led_Device_Driver * Get_LED_DevDrv(uint8_t _index);
	Luxmeter_Device_Driver * Get_Luxmeter_DevDrv(uint8_t _index);
	Mqqt_Wifi_Device_Driver * Get_Mqqt_Wifi_DevDrv(uint8_t _index);
	OLED_Display_Device_Driver * Get_OLED_Display_DevDrv(uint8_t _index);
	Temperature_Device_Driver * Get_Temperatur_DevDrv(uint8_t _index);
	Uart_RGB_Led_Device_Driver * Get_Uart_RGB_Led_DevDrv(uint8_t _index);
	Uart_GRBW_Led_Device_Driver * Get_Uart_GRBW_Led_DevDrv(uint8_t _index);
	WebSocket_Wifi_Device_Driver * Get_WebSocket_Wifi_DevDrv(uint8_t _index);
	Ntp_Wifi_Device_Driver * Get_Ntp_Wifi_Device_DevDrv(uint8_t _index);
	GPS_Device_Driver * Get_GPS_Device_DevDrv(uint8_t _index);

	bool GetDeviceById(int _id, Driver * _device);
	bool HasDeviceWithId(int _id);
	Driver *GetDeviceById(int Id);

	virtual void OnMessage(Int_Task_Msg message);
	virtual void OnStartup();
	virtual void OnStop();
	virtual void OnInit();
	virtual void OnShutdown();
	virtual void OnSuspend();
	virtual void OnBuild_Descriptor();
	virtual void OnModuleUpdate(uint32_t deltaTime);
	bool PopMessage(TaskMessage** message);
	void Set_Debug_Mode(bool _state);
	void OnNotifyInitDone(int _DriverID);
	void OnNotifyStartupDone(int _DriverID);
protected:
	 virtual void DoModuleMessage(Int_Task_Msg message) = 0;
	 virtual void DoTaskMessage(TaskMessage *message) = 0;
	 virtual void Build_Discriptor() = 0;
public:
	Device_Driver* create(const std::string& classname);
	static void registerit(const std::string& classname, Creator* creator);
	Module_Driver(uint8_t priority = TASK_PRIORITY_NORMAL);
	~Module_Driver();
	bool SendAsyncTaskMessage(TaskMessage* message, bool withinIsr = false);
	void AddDevice(Device_Driver* device);

	void Exec_Set_Debug_Mode(bool _state);
};

#define REGISTER(classname) \
	private: \
	static const CreatorImpl<classname> creator;

#define REGISTERIMPL(classname) \
	const CreatorImpl<classname> classname::creator(#classname);


#endif

