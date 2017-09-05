//
//
//

#include "Driver.h"

int Driver::driver_count = 1000;


Driver::Driver(uint8_t priority = THREAD_PRIORITY_NORMAL) :
	Thread(MsToThreadTime(priority))
{
	driver_count++;
	publisher = new Publisher(driver_count);
	__DriverId = driver_count;
}

bool Driver::OnStart() {

}

void Driver::OnStop() {

}

String Driver::GetDriverName()
{
	return driver_name;
}

void Driver::Exec_Command(int _cmdId, String _command)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(_cmdId);
	message->AddParam(_command);
	PostMessage(&message);
}

void Driver::OnUpdate(uint32_t deltaTime) {
	if (!message_queue.Empty()) {	
		for (unsigned i = 0; i < message_queue.Size(); i++) {
			int messageID = message_queue[i]->GetID();
			switch (messageID)
			{
			case DRIVER_INIT:
			{
				//Serial.print("Driver::OnUpdate::DRIVER_INIT with ");
				//Serial.println(GetDriverName());
				DoInit();
				break;
			}
			case DRIVER_SHUTDOWN:
			{
				DoShutdown();
				break;
			}
			case DRIVER_SUSPEND:
			{
				DoSuspend();
				break;
			}
			default:
			{
				DoMessage(*message_queue[i]);
			}
			}
			delete message_queue[i];
		}
		message_queue.Clear();
	}
	DoUpdate(deltaTime);
	yield;
}


void Driver::ExecInit() {
	//Serial.print("Driver::ExecInit() with ");
	//Serial.print(GetDriverName());
	//Serial.print(" and message: ");
	//Serial.println(DRIVER_INIT);
	Int_Thread_Msg *message = new Int_Thread_Msg(DRIVER_INIT);
	PostMessage(&message);
}

void Driver::ExecShutdown() {
	Int_Thread_Msg *message = new Int_Thread_Msg(DRIVER_SHUTDOWN);
	PostMessage(&message);
}

void Driver::ExecSuspend() {
	Int_Thread_Msg *message = new Int_Thread_Msg(DRIVER_SUSPEND);
	PostMessage(&message);
}

Publisher *Driver::GetPublisher()
{
	return publisher;
}



int Driver::GetError() {
	return error_Code;
}

bool Driver::IsError() const
{
	return is_Error;
}

bool Driver::isSuspend() const
{
	return is_Suspend;
}
bool Driver::isIdle() const
{
	return is_Idle;
}
bool Driver::isBusy() const
{
	return is_Busy;
}
bool Driver::isInactive() const
{
	return is_Inactive;
}
