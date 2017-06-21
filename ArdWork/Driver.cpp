//
//
//

#include "Driver.h"

int Driver::device_count = 0;

bool Driver::OnStart() {

}

void Driver::OnStop() {

}

String Driver::GetDriverName()
{
	return driver_name;
}

void Driver::OnUpdate(uint32_t deltaTime) {
  if (!message_queue.Empty()) {
	  for (unsigned i = 0; i < message_queue.Size(); i++) {
		  int messageID = message_queue[i]->GetID();
		  switch (messageID)
		  {
		  case DRIVER_INIT:
			  DoInit();
			  break;
		  case DRIVER_SHUTDOWN:
			  DoShutdown();
			  break;
		  case DRIVER_SUSPEND:
			  DoSuspend();
			  break;
		  default:
			  DoMessage(*message_queue[i]);
		  }
		  delete message_queue[i];
	  }
	  message_queue.Clear();
  }
  DoUpdate(deltaTime);
  delay(1);
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
