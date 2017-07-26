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
	Serial.print("Driver::Driver - publisher->driverId: ");
	Serial.println(publisher->driverId);
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
	Serial.print("Driver::Exec_Command - _cmdId: ");
	Serial.println(_cmdId);
	Serial.print("Driver::Exec_Command - _command: ");
	Serial.println(_command);
	message->AddParam(_command);
	Serial.print("Done1");
	PostMessage(&message);
	Serial.print("Done2");
}

void Driver::OnUpdate(uint32_t deltaTime) {
	if (!message_queue.Empty()) {
		for (unsigned i = 0; i < message_queue.Size(); i++) {
			int messageID = message_queue[i]->GetID();
			switch (messageID)
			{
			case DRIVER_INIT:
			{
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
				Serial.println("DoMessage(*message_queue[i]);");
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
