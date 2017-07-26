#pragma once

#include "Arduino.h"
#include "m_Vector.h"
#include "Base_Consts.h"

#define default_cmdId 0

class Publisher_Element
{
protected:
	String __class_name;
public:
	virtual String GetClassName();
	Publisher_Element(int _cmdId, String _lable, String _descr);
	virtual  ~Publisher_Element();
	int cmdId;
	String lable;
	String descr;
};

class Button_Publisher : public Publisher_Element
{
public:
	Button_Publisher(int _cmdId, String _lable, String _descr);
	virtual  ~Button_Publisher();
};

class Value_Publisher : public Publisher_Element
{
public:
	Value_Publisher(String _lable, String _descr, float *_value, String _unit);
	virtual  ~Value_Publisher();
	float *value;
	String unit;
};

class Switch_Publisher : public Publisher_Element
{
public:
	Switch_Publisher(int _cmdId, String _lable, String _descr);
	virtual  ~Switch_Publisher();
};


class Boolean_Publisher : public Publisher_Element
{
public:
	Boolean_Publisher(String _lable, String _descr, bool *_isActive);
	virtual  ~Boolean_Publisher();
	bool *isActive;
};

class Publisher
{
private:
	Vector<Publisher_Element*> *pub_elem_list;
	int __driverId;
	int __elem_count;
	int GetElemCount() const;
	int GetDriverId() const;
public:
	Publisher(int _driverId);

	String name;
	String descr;
	bool published;

	Publisher_Element *GetElemByIndex(int _index);
	void Add_Publisher_Element(Publisher_Element *_elem);

	Property<int, Publisher> elem_count{ this, nullptr, &Publisher::GetElemCount };
	Property<int, Publisher> driverId{ this, nullptr, &Publisher::GetDriverId };
};


