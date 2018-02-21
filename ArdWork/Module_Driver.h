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

