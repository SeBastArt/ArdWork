#ifndef _BASE_CONSTS_h
#define _BASE_CONSTS_h


#include "Arduino.h"

static int const BASE_FIRST_MESSAGE = 1000000;
static int const BASE_LAST_MESSAGE = BASE_FIRST_MESSAGE;

static int const THREAD_PRIORITY_VERY_HIGH = 1;
static int const THREAD_PRIORITY_HIGH = 3;
static int const THREAD_PRIORITY_NORMAL = 10;
static int const THREAD_PRIORITY_LOW = 25;
static int const THREAD_PRIORITY_VERY_LOW = 50;


static unsigned int const THREAD_MSG_BUTTONSTATE_FIRST_MESSAGE = BASE_FIRST_MESSAGE + 1000000000;
static unsigned int const THREAD_MSG_BUTTONSTATE_TRACKING = THREAD_MSG_BUTTONSTATE_FIRST_MESSAGE;
static unsigned int const THREAD_MSG_BUTTONSTATE_PRESSED = THREAD_MSG_BUTTONSTATE_FIRST_MESSAGE + 1;
static unsigned int const THREAD_MSG_BUTTONSTATE_AUTOREPEAT = THREAD_MSG_BUTTONSTATE_FIRST_MESSAGE + 2;
static unsigned int const THREAD_MSG_BUTTONSTATE_RELEASED = THREAD_MSG_BUTTONSTATE_FIRST_MESSAGE + 3;
static unsigned int const THREAD_MSG_BUTTONSTATE_LAST_MESSAGE = THREAD_MSG_BUTTONSTATE_RELEASED;


//Thread Messages
enum MessageClass
{
	MessageClass_Heartbeat,
	MessageClass_Button,
	MessageClass_Float,
	MessageClass_Communication,
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



struct ThreadMessage
{
	ThreadMessage(uint8_t classEnum, uint8_t size) :
		size(size),
		Class(classEnum)
	{
	};

	uint8_t size;
	uint8_t Class;
};


struct HeartbeatMessage : ThreadMessage
{
	HeartbeatMessage(bool state) :
		ThreadMessage(MessageClass_Heartbeat, sizeof(HeartbeatMessage)),
		State(state)
	{
	};

	bool State;
};

struct ButtonMessage : ThreadMessage
{
	ButtonMessage(uint8_t id, uint32_t state) :
		ThreadMessage(MessageClass_Button, sizeof(ButtonMessage)),
		Id(id),
		State(state)
	{
	};

	uint8_t Id;
	uint32_t State;
};

//Template please
struct FloatMessage : ThreadMessage
{
	FloatMessage(uint8_t id, float _value) :
		ThreadMessage(MessageClass_Float, sizeof(FloatMessage)),
		Id(id),
		Value(_value)
	{
	};
	uint8_t Id;
	float Value;
};




struct CommunicationMessage : ThreadMessage
{
	CommunicationMessage(uint16_t _Id, uint16_t _CmdId, String _Values) :
		ThreadMessage(MessageClass_Communication, sizeof(CommunicationMessage))
	{
		Id = _Id;
		CmdId = _CmdId;
		Values = _Values;
	};
	uint16_t Id;
	uint16_t CmdId;
	String Values;
};

struct MqqtMessage : ThreadMessage
{
	MqqtMessage(char message[50]) :
		ThreadMessage(MessageClass_Mqqt, sizeof(MqqtMessage))
	{
		*Message = *message;
	};
	char Message[50];
};


#endif