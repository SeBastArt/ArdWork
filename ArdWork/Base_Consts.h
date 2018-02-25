#ifndef _BASE_CONSTS_h
#define _BASE_CONSTS_h

#pragma once

#include "Arduino.h"


static int const BASE_TYPE = 1000000;
static int const BASE_LAST_MESSAGE = BASE_TYPE;

static int const TASK_PRIORITY_VERY_HIGH = 1;
static int const TASK_PRIORITY_HIGH = 3;
static int const TASK_PRIORITY_NORMAL = 10;
static int const TASK_PRIORITY_LOW = 25;
static int const TASK_PRIORITY_VERY_LOW = 50;


//Task Messages
enum MessageClass
{
	MessageClass_Heartbeat,
	MessageClass_Button,
	MessageClass_Float,
	MessageClass_Extern,
	MessageClass_Mqqt
};


enum Button_State
{
	buttonstate_released = 0b00000000,
	buttonstate_pressed = 0b00000001,
	buttonstate_autorepeat = 0b00000011,
	buttonstate_tracking = 0b10000001
};

template<typename T, typename C>
class Property {
public:
	using SetterType = void (C::*)(T);
	using GetterType = T(C::*)() const;

	Property(C* theObject, SetterType theSetter, GetterType theGetter)
		:itsObject(theObject),
		itsSetter(theSetter),
		itsGetter(theGetter)
	{ }

	operator T() const
	{
		return (itsObject->*itsGetter)();
	}

	C& operator = (T theValue) {
		(itsObject->*itsSetter)(theValue);
		return *itsObject;
	}

private:
	C* const itsObject;
	SetterType const itsSetter;
	GetterType const itsGetter;
};



struct TaskMessage
{
	TaskMessage(uint8_t classEnum, uint8_t size) :
		size(size),
		Class(classEnum)
	{
	};

	uint8_t size;
	uint8_t Class;
};


struct HeartbeatMessage : TaskMessage
{
	HeartbeatMessage(bool state) :
		TaskMessage(MessageClass_Heartbeat, sizeof(HeartbeatMessage)),
		State(state)
	{
	};

	bool State;
};

struct ButtonMessage : TaskMessage
{
	ButtonMessage(int id, uint32_t state) :
		TaskMessage(MessageClass_Button, sizeof(ButtonMessage)),
		Id(id),
		State(state)
	{
	};

	int Id;
	uint32_t State;
};

//Template please
struct FloatMessage : TaskMessage
{
	FloatMessage(int _id, float _absvalue) :
		TaskMessage(MessageClass_Float, sizeof(FloatMessage)),
		__id(_id),
		__absValue(_absvalue)
	{
	};
	int __id;
	float __absValue;
};




struct ExternMessage : TaskMessage
{
	ExternMessage(int _Id, int _CmdId, String _Value) :
		TaskMessage(MessageClass_Extern, sizeof(ExternMessage))
	{
		__Id = _Id;
		__CmdId = _CmdId;
		__Value = _Value;
	};
	int __Id;
	int __CmdId;
	String __Value;
};

struct MqqtMessage : TaskMessage
{
	MqqtMessage(char message[50]) :
		TaskMessage(MessageClass_Mqqt, sizeof(MqqtMessage))
	{
		*Message = *message;
	};
	char Message[50];
};


#endif

