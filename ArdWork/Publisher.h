#pragma once

#include "Arduino.h"
#include "m_Vector.h"
#include "Base_Consts.h"

class Publisher_Element
{
protected:
	String __class_name;
public:
	virtual String GetClassName();
	Publisher_Element(String _name, String _descr);
	virtual  ~Publisher_Element();
	String name;
	String descr;
};

class Button_Publisher : public Publisher_Element
{
public:
	Button_Publisher(String _name, String _descr, String _cmd);
	virtual  ~Button_Publisher();
	String cmd;
};

class Value_Publisher : public Publisher_Element
{
public:
	Value_Publisher(String _name, String _descr, float *_value, String _unit);
	virtual  ~Value_Publisher();
	float *value;
	String unit;
};

class Switch_Publisher : public Publisher_Element
{
public:
	Switch_Publisher(String _name, String _descr, String _cmdOn, String,  String _cmdOff);
	virtual  ~Switch_Publisher();
	String cmdOn;
	String cmdOff;
};


class Boolean_Publisher : public Publisher_Element
{
public:
	Boolean_Publisher(String _name, String _descr, bool *_isActive);
	virtual  ~Boolean_Publisher();
	bool *isActive;
};

class Publisher
{
private:
	Vector<Publisher_Element*> *pub_List;
	uint8_t __id;
	uint8_t __elem_count;
	uint8_t GetElemCount() const;
	uint8_t GetId() const;
public:
	Publisher(uint8_t _id);

	String name;
	String descr;
	bool published;

	Publisher_Element *GetElemByIndex(uint8_t _index);
	void Add_Publisher_Element(Publisher_Element *_elem);

	Property<uint8_t, Publisher> elem_count{ this, nullptr, &Publisher::GetElemCount };
	Property<uint8_t, Publisher> id{ this, nullptr, &Publisher::GetId };
};


