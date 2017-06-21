#ifndef _INT_THREAD_MSG_SYS_ht
#define _INT_THREAD_MSG_SYS_h 

#include "m_Vector.h"
#include "Int_Thread_Msg.h"

class Int_Thread_Msg_Sys
{
protected:
	Vector <Int_Thread_Msg*> message_queue;
	virtual void DoMessage(Int_Thread_Msg message) = 0;

	void PostMessage(Int_Thread_Msg **message) {
		message_queue.PushBack(*message);
	}
public:
	Int_Thread_Msg_Sys()
	{
		message_queue.Clear();
	}

	~Int_Thread_Msg_Sys()
	{
		message_queue.Clear();
	}


};

#endif

