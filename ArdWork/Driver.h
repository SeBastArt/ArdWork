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


class Driver : public Task, public Int_Task_Msg_Sys
{
private:
	int __DriverId;
	bool __isPublished;
	int GetDriverId() const { return __DriverId; }
	unsigned int GetDriverType() const { return __DriverType; }
protected:
	bool __canBeLoad;
	static Descriptor_List *__descriptor_list;
	unsigned int __DriverType;
	Descriptor *__descriptor;
	static int driver_count;
	int error_Code;
	bool is_Error;
	bool is_Suspend;
	bool is_Idle;
	bool is_Busy;
	bool is_Inactive;
protected:
	bool OnStart();
	void OnStop();
	void OnUpdate(uint32_t deltaTime);

	virtual void DoMessage(Int_Task_Msg message) = 0;
	virtual void DoUpdate(uint32_t deltaTime) = 0;
	virtual void DoInit() = 0;
	virtual void Build_Descriptor() = 0;
	virtual void DoShutdown() = 0;
	virtual void DoSuspend() = 0;
public:
	
	Driver(uint8_t priority);
	Property<int, Driver> DriverId{ this,nullptr,&Driver::GetDriverId };
	Property<unsigned int, Driver> DriverType{ this,nullptr,&Driver::GetDriverType };

	void ExecInit();
	void ExecShutdown();
	void ExecSuspend();

	int GetError();
	bool IsError() const;
	bool isSuspend() const;
	bool isIdle() const;
	bool isBusy() const;
	bool isInactive() const;
	bool isPublished();
	Descriptor *GetDescriptor();
	void Exec_Command(int _cmdId, String _command);
};

#endif


