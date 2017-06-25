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
	MessageClass_Server,
	MessageClass_Mqqt
};

enum Button_State
{
	buttonstate_released = 0b00000000,
	buttonstate_pressed = 0b00000001,
	buttonstate_autorepeat = 0b00000011,
	buttonstate_tracking = 0b10000001
};

enum Ctrl_Style
{
	Icon_Kind_none = -1,
	Icon_Kind_switch = 0,
	Icon_Kind_button = 1,
	Icon_Kind_Integer = 2,
	Icon_Kind_percent = 3,
	Icon_Kind_string = 4,
	Icon_Kind_comm = 5,
	Icon_Kind_multi = 6
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


class Control {
public:
	Control(uint16 _Id)
	{
		Serial.println("Control::Control");
		__Id = _Id;
		__SubId = 0;
		__Name = "";
		__Descript = "";
		__Command = "";
		__Data = "none";
		__Style = Icon_Kind_none;
	};

	Property<uint16, Control> Id{ this,nullptr,&Control::getId };
	Property<uint16, Control> SubId{ this,&Control::setSubId,&Control::getSubId };
	Property<String, Control> Name{ this,&Control::setName,&Control::getName };
	Property<String, Control> Description{ this,&Control::setDescript,&Control::getDescript };
	Property<String, Control> Command{ this,&Control::setCommand,&Control::getCommand };
	Property<String, Control> Data{ this,&Control::setData,&Control::getData };
	Property<Ctrl_Style, Control> Style{ this,&Control::setStyle,&Control::getStyle };
private:
	uint16 __Id;
	uint16 __SubId;
	String __Name;
	String __Descript;
	String __Command;
	String __Data;
	Ctrl_Style __Style;

	uint16 getId() const
	{
		return __Id;
	}

	void setSubId(uint16 _SubId)
	{
		__SubId = _SubId;
	}

	uint16 getSubId() const
	{
		return __SubId;
	}

	void setName(String _Name)
	{
		__Name = _Name;
	}

	String getName() const
	{
		return __Name;
	}

	void setDescript(String _Descript)
	{
		__Descript = _Descript;
	}

	String getDescript() const
	{
		return __Descript;
	}

	void setCommand(String _Command)
	{
		__Command = _Command;
	}

	String getCommand() const
	{
		return __Command;
	}

	String getData() const
	{
		return __Data;
	}

	void setData(String _Data)
	{
		__Data = _Data;
	}

	void setStyle(Ctrl_Style _Style)
	{
		__Style = _Style;
	}

	Ctrl_Style getStyle() const
	{
		return __Style;
	}
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




struct ServerMessage : ThreadMessage
{
	ServerMessage(uint16_t _Id, String _Cmd) :
		ThreadMessage(MessageClass_Server, sizeof(ServerMessage))
	{
		Id = _Id;
		Cmd = _Cmd;
	};
	uint16_t Id;
	String Cmd;
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

//const uint16_t MaxMessagesize = _max(_max(_max(_max(sizeof(HeartbeatMessage), sizeof(ButtonMessage)), sizeof(LuxMessage)), sizeof(ServerMessage)), sizeof(MqqtMessage));





#endif