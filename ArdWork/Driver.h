// Driver.h

#ifndef _DRIVER_h
#define _DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Thread/Thread.h"
#include "Driver_Consts.h"
#include "Int_Thread_Msg_Sys.h"
#include "Base_Consts.h"



class Driver : public Thread, public Int_Thread_Msg_Sys
{
protected:
	String driver_name;
	static int device_count;
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

	virtual void DoMessage(Int_Thread_Msg message) = 0;
	virtual void DoUpdate(uint32_t deltaTime) = 0;
	virtual void DoInit() = 0;
	virtual void DoShutdown() = 0;
	virtual void DoSuspend() = 0;
public:
	Driver(uint8_t priority = THREAD_PRIORITY_NORMAL) : Thread(MsToThreadTime(priority)) { device_count++; };

	void ExecInit();
	void ExecShutdown();
	void ExecSuspend();

	int GetError();
	bool IsError() const;
	bool isSuspend() const;
	bool isIdle() const;
	bool isBusy() const;
	bool isInactive() const;
	String GetDriverName();
};

#endif

