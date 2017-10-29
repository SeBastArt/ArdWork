//
//
//

#include "Driver.h"

int Driver::driver_count = 1000;


Driver::Driver(uint8_t priority = THREAD_PRIORITY_NORMAL) :
	Thread(MsToThreadTime(priority))
{
	driver_count++;
	__descriptor = new Descriptor(driver_count);
	__DriverId = driver_count;
	__DriverType = -1;
}

bool Driver::OnStart() {

}

void Driver::OnStop() {

}

void Driver::Exec_Command(int _cmdId, String _value)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(_cmdId);
	message->AddParam(_value);
	PostMessage(&message);
}

void Driver::OnUpdate(uint32_t deltaTime) {
	unsigned long  start = micros();
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
				Build_Descriptor();
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
	/*unsigned long  end = micros(); 
	Serial.print("Driver: ");
	Serial.print(this->driver_name);
	Serial.print(" took ");
	Serial.print(end - start);
	Serial.println("ms");*/
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

Descriptor *Driver::GetDescriptor()
{
	return __descriptor;
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
