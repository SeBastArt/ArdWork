// Driver.h

#ifndef _DRIVER_h
#define _DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Task.h>

#include "Driver_Consts.h"
#include "Int_Task_Msg_Sys.h"
#include "Base_Consts.h"
#include "Publisher.h"
#include "Pin_Manager.h"

static Pin_Manager pinManager;

class Driver : public Task, public Int_Task_Msg_Sys
{
private:
	int __DriverId;
	bool __isPublished;
	int GetDriverId() const { return __DriverId; }
	unsigned int GetDriverType() const { return __DriverType; }

	void OnUpdate(uint32_t deltaTime);
	void DoMessage(Int_Task_Msg message);
	void DoStartup();
	void DoStop();
	void DoInit();
	void DoLoadPresets();
	void DoShutdown();
	void DoSuspend();

	uint timer_delta;
	uint timer_delay;
protected:
	static Descriptor_List *__descriptor_list;
	unsigned int __DriverType;
	Descriptor *__descriptor;
	static int driver_count;
	int error_Code;
	bool __isError;
	bool __isSuspend;
	bool __isIdle;
	bool __isBusy;
	bool __isInactive;
	void SetTimerDelay(uint delay_ms);
protected:

	static void fw_Exec_Command(void * context, int i0, String i1);
	virtual void TimerTick(); 
	virtual void DoUpdate(uint32_t deltaTime) = 0;
	virtual void OnMessage(Int_Task_Msg message) = 0;
	virtual void OnStartup() = 0;
	virtual void OnStop() = 0;
	virtual void OnInit() = 0;
	virtual void OnShutdown() = 0;
	virtual void OnSuspend() = 0;
	virtual void OnBuild_Descriptor() = 0;

public:
	Driver(uint8_t priority);
	virtual ~Driver() {}
	Property<int, Driver> DriverId{ this,nullptr,&Driver::GetDriverId };
	Property<unsigned int, Driver> DriverType{ this,nullptr,&Driver::GetDriverType };

	void ExecStart();
	void ExecStop();
	void ExecInit();
	void ExecShutdown();
	void ExecSuspend();
	void ExecLoadPresets();

	bool isSuspend() const;
	bool isIdle() const;
	bool isBusy() const;
	bool isInactive() const;
	bool isPublished();
	Descriptor *GetDescriptor();
	void Exec_Command(int _cmdId, String _command);
};

#endif


