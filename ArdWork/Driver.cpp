//
//
//

#include "Driver.h"

//#define DEBUG

int Driver::driver_count = 1000;
Descriptor_List *Driver::__descriptor_list = new Descriptor_List;

Driver::Driver(uint8_t priority = TASK_PRIORITY_NORMAL) :
	Task(MsToTaskTime(priority))
{
#ifdef  DEBUG
	Serial.print("Start Constructor Driver with ID: ");
	Serial.println(this->DriverId);
#endif //  DEBUG
	driver_count++;
	__descriptor = new Descriptor(driver_count);
	__descriptor->published = true;
	__DriverId = driver_count;
	__DriverType = -1;
	__isPublished = false;
	__canBeLoad = false;
#ifdef  DEBUG
	Serial.print("Ende Constructor Driver");
#endif //  DEBUG
}

bool Driver::OnStart() {

}

void Driver::OnStop() {

}

void fw_Exec_Command(void* context, int i0, String i1) {
	static_cast<Driver*>(context)->Exec_Command(i0, i1);
}

void Driver::Exec_Command(int _cmdId, String _value)
{
#ifdef  DEBUG
	Serial.print("Start Driver::Exec_Command for DeviceID: ");
	Serial.println(this->DriverId);
#endif //  DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(_cmdId);
	message->AddParam(_value);
	PostMessage(&message);
#ifdef  DEBUG
	Serial.println("Ende Driver::Exec_Command");
#endif //  DEBUG
}

void Driver::OnUpdate(uint32_t deltaTime) {
	unsigned long  start = micros();
	if (!message_queue.Empty()) {
		for (unsigned i = 0; i < message_queue.Size(); i++) {
			int messageID = message_queue[i]->id;
#ifdef  DEBUG
			Serial.print("messageID: ");
			Serial.println(messageID);
#endif //  DEBUG

			switch (messageID)
			{
			case DRIVER_SUSPEND:
			{
#ifdef  DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_SUSPEND");
#endif //  DEBUG
				DoSuspend();
				break;
			}
			case DRIVER_INIT:
			{
#ifdef  DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_INIT");
#endif //  DEBUG
				DoInit();
				Build_Descriptor();
				__isPublished = __descriptor->published;
				__descriptor_list->Add_Descriptor(__descriptor);
#ifdef  DEBUG
				Serial.print("Initialize Driver with ID: ");
				Serial.println(DriverId);
#endif //  DEBUG
				if (__canBeLoad) {
					__descriptor_list->Load(DriverId, &fw_Exec_Command, this);
				}
#ifdef  DEBUG
				Serial.println("Ende Driver::OnUpdate DRIVER_INIT");
#endif //  DEBUG
				break;
			}
			case DRIVER_SHUTDOWN:
			{
#ifdef  DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_SHUTDOWN");
#endif //  DEBUG
				DoShutdown();
				break;
			}		
			default:
			{
#ifdef  DEBUG
				Serial.println("Start Driver::OnUpdate default");
#endif //  DEBUG
				DoMessage(*message_queue[i]);
			}
			}
			delete message_queue[i];
		}
		message_queue.Clear();
	}
	DoUpdate(deltaTime);
}


void Driver::ExecInit() {
#ifdef  DEBUG
	Serial.println("Start Driver::ExecInit");
#endif //  DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_INIT);
	PostMessage(&message);
#ifdef  DEBUG
	Serial.println("Ende Driver::ExecInit");
#endif //  DEBUG
}

void Driver::ExecShutdown() {
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_SHUTDOWN);
	PostMessage(&message);
}

void Driver::ExecSuspend() {
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_SUSPEND);
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

bool Driver::isPublished()
{
	return __isPublished;
}
