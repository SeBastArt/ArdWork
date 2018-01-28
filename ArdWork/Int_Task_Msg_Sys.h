#ifndef _INT_TASK_MSG_SYS_h
#define _INT_TASK_MSG_SYS_h 

#include "m_Vector.h"
#include "Int_Task_Msg.h"

class Int_Task_Msg_Sys
{
protected:
	Vector <Int_Task_Msg*> message_queue;
	virtual void DoMessage(Int_Task_Msg message) = 0;

	void PostMessage(Int_Task_Msg **message) {
		message_queue.PushBack(*message);
	}
public:
	Int_Task_Msg_Sys()
	{
		message_queue.Clear();
	}

	~Int_Task_Msg_Sys()
	{
		message_queue.Clear();
	}


};

#endif


