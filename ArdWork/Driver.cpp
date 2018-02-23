//
//
//

#include "Driver.h"

//#define DEBUG

int Driver::driver_count = 1000;
Descriptor_List *Driver::__descriptor_list = new Descriptor_List;
TaskManager Driver::taskManager;
Pin_Manager Driver::pinManager;

Driver::Driver(uint8_t priority = TASK_PRIORITY_NORMAL) :
	Task(MsToTaskTime(priority))
{
#ifdef DEBUG
	Serial.print(F("Start Constructor Driver with ID: "));
	Serial.println(this->DriverId);
#endif // DEBUG
	driver_count++;
	__descriptor = new Descriptor(driver_count);
	__descriptor->published = true;
	__DriverId = driver_count;
	__DriverType = -1;
	__isPublished = false;

	__isError = false;
	__isSuspend = false;
	__isIdle = false;
	__isBusy = false;
	__isInactive = true;
	timer_delay = 2000;
	timer_delta = 0;
	taskManager.StartTask(this);
#ifdef DEBUG
	Serial.print("Ende Constructor Driver");
#endif // DEBUG
}

void Driver::fw_Exec_Command(void* context, int i0, String i1) {
	static_cast<Driver*>(context)->Exec_Command(i0, i1);
}

void Driver::TimerTick()
{
#ifdef DEBUG
	Serial.print("Start Driver::TimerTick for DeviceID: ");
	Serial.println(this->DriverId);
#endif // DEBUG
	//
#ifdef DEBUG
	Serial.print("Ende Driver::TimerTick");
#endif // DEBUG
}

void Driver::Exec_Command(int _cmdId, String _value)
{
#ifdef DEBUG
	Serial.print("Start Driver::Exec_Command for DeviceID: ");
	Serial.println(this->DriverId);
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(_cmdId);
	message->AddParam(_value);
	PostMessage(&message);
	__isBusy = true;
#ifdef DEBUG
	Serial.println("Ende Driver::Exec_Command");
#endif // DEBUG
}

void Driver::OnUpdate(uint32_t deltaTime) {
	unsigned long  start = micros();
	if (!message_queue.Empty()) {
		for (unsigned i = 0; i < message_queue.Size(); i++) {
			int messageID = message_queue[i]->id;
#ifdef DEBUG
			Serial.print("messageID: ");
			Serial.println(messageID);
#endif // DEBUG

			switch (messageID)
			{
			case DRIVER_START:
			{
#ifdef DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_START");
#endif // DEBUG
				DoStartup();
				break;
			}

			case DRIVER_INIT:
			{
#ifdef DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_INIT");
#endif // DEBUG
				DoInit();
#ifdef DEBUG
				Serial.print("Initialize Driver with ID: ");
				Serial.println(DriverId);
#endif // DEBUG

#ifdef DEBUG
				Serial.println("Ende Driver::OnUpdate DRIVER_INIT");
#endif // DEBUG
				break;
			}


			case DRIVER_LOADPRESETS:
			{
#ifdef DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_LOADPRESETS");
#endif // DEBUG
				DoLoadPresets();
				break;
			}


			case DRIVER_STOP:
			{
#ifdef DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_STOP");
#endif // DEBUG
				DoStop();
				break;
			}


			case DRIVER_SHUTDOWN:
			{
#ifdef DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_SHUTDOWN");
#endif // DEBUG
				DoShutdown();
				break;
			}


			case DRIVER_SUSPEND:
			{
#ifdef DEBUG
				Serial.println("Start Driver::OnUpdate DRIVER_SUSPEND");
#endif // DEBUG
				DoSuspend();
				break;
			}
			default:
			{
#ifdef DEBUG
				Serial.println("Start Driver::OnUpdate default");
#endif // DEBUG
				DoMessage(*message_queue[i]);
			}
			}
			delete message_queue[i];
		}
		message_queue.Clear();
	}
	else
		__isBusy = false;
	DoUpdate(deltaTime);

	timer_delta += deltaTime;
	if (timer_delta > timer_delay) {
		timer_delta = 0;
		if (__isIdle) {
#ifdef DEBUG
			Serial.println("Start Driver::OnUpdate TimerTick");
#endif // DEBUG
			TimerTick();
		}
	}
}

void Driver::SetTimerDelay(uint delay_ms) {
	timer_delay = delay_ms;
}


void Driver::DoMessage(Int_Task_Msg message) {
#ifdef DEBUG
	Serial.print("Start Driver::DoMessage");
#endif // DEBUG
	OnMessage(message);
#ifdef DEBUG
	Serial.println("Ende Driver::DoMessage");
#endif // DEBUG
}

void Driver::DoStartup() {
#ifdef DEBUG
	Serial.print("Start Driver::DoStartup");
#endif // DEBUG
	OnStartup();
	__isInactive = false;
#ifdef DEBUG
	Serial.print("Ende Driver::DoStartup");
#endif // DEBUG
}

void Driver::DoInit() {
#ifdef DEBUG
	Serial.print("Start Driver::DoInit");
#endif // DEBUG
	OnInit();
	OnBuild_Descriptor();
	__isPublished = __descriptor->published;
	__descriptor_list->Add_Descriptor(__descriptor);
#ifdef DEBUG
	Serial.print("Ende Driver::DoInit");
#endif // DEBUG
}

void Driver::DoLoadPresets() {
#ifdef DEBUG
	Serial.print("Start Driver::DoLoadPresets");
#endif // DEBUG
	__descriptor_list->Load(DriverId, &fw_Exec_Command, this);
	__isIdle = true;
#ifdef DEBUG
	Serial.print("Ende Driver::DoLoadPresets");
#endif // DEBUG
}

void Driver::DoStop() {
#ifdef DEBUG
	Serial.print("Start Driver::DoStop");
#endif // DEBUG
	OnStop();
	__isInactive = true;
#ifdef DEBUG
	Serial.print("Ende Driver::DoStop");
#endif // DEBUG
}

void Driver::DoShutdown() {
#ifdef DEBUG
	Serial.print("Start Driver::DoShutdown");
#endif // DEBUG
	OnShutdown();
#ifdef DEBUG
	Serial.print("Ende Driver::DoShutdown");
#endif // DEBUG
}

void Driver::DoSuspend() {
#ifdef DEBUG
	Serial.print("Start Driver::DoSuspend");
#endif // DEBUG
	OnSuspend();
	__isSuspend = true;
#ifdef DEBUG
	Serial.print("Ende Driver::DoSuspend");
#endif // DEBUG
}


void Driver::ExecInit() {
#ifdef DEBUG
	Serial.println("Start Driver::ExecInit");
#endif // DEBUG
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_INIT);
	PostMessage(&message);
#ifdef DEBUG
	Serial.println("Ende Driver::ExecInit");
#endif // DEBUG
}

void Driver::ExecShutdown() {
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_SHUTDOWN);
	PostMessage(&message);
}

void Driver::ExecSuspend() {
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_SUSPEND);
	PostMessage(&message);
}

void Driver::ExecStart() {
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_START);
	PostMessage(&message);
}

void Driver::ExecStop() {
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_STOP);
	PostMessage(&message);
}

void Driver::ExecLoadPresets() {
	Int_Task_Msg *message = new Int_Task_Msg(DRIVER_LOADPRESETS);
	PostMessage(&message);
}


Descriptor *Driver::GetDescriptor()
{
	return __descriptor;
}

bool Driver::isSuspend() const
{
	return __isSuspend;
}
bool Driver::isIdle() const
{
	return __isIdle;
}
bool Driver::isBusy() const
{
	return __isBusy;
}
bool Driver::isInactive() const
{
	return __isInactive;
}

bool Driver::isPublished()
{
	return __isPublished;
}
