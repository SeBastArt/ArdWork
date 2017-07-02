#include "Publisher.h"

Publisher::Publisher(uint8_t _id):
	__id(_id),
	name("Device"),
	descr("Description Text")
{
		pub_List = new Vector<Publisher_Element*>;
		__elem_count = 0;
		published = false;
}

void Publisher::Add_Publisher_Element(Publisher_Element *_elem)
{
	pub_List->PushBack(_elem);
	__elem_count++;
}

Publisher_Element *Publisher::GetElemByIndex(uint8_t _index)
{
	Publisher_Element *result_elem = nullptr;
	if ((index >= 0) && (_index < __elem_count))
		result_elem = (*pub_List)[_index];	
		return result_elem;
}

uint8_t Publisher::GetElemCount() const
{
	return __elem_count;
}
uint8_t Publisher::GetId() const
{
	return __id;
}


String Publisher_Element::GetClassName()
{
	return __class_name;
}

Publisher_Element::Publisher_Element(String _name, String _descr) :
	name(_name),
	descr(_descr)
{
};

Publisher_Element::~Publisher_Element()
{
}


Button_Publisher::Button_Publisher(String _name, String _descr, String _cmd) :
	Publisher_Element(_name, _descr),
	cmd(_cmd)
{
	__class_name = "Button_Publisher";
};

Button_Publisher::~Button_Publisher()
{
}


Value_Publisher::Value_Publisher(String _name, String _descr, float *_value, String _unit) :
	Publisher_Element(_name, _descr),
	value(_value),
	unit(_unit)
{
	__class_name = "Value_Publisher";
};

Value_Publisher::~Value_Publisher()
{
}

Switch_Publisher::Switch_Publisher(String _name, String _descr, String _cmdOn, String, String _cmdOff) :
	Publisher_Element(_name, _descr),
	cmdOn(_cmdOn),
	cmdOff(_cmdOff)
{
	__class_name = "Switch_Publisher";
}

Switch_Publisher::~Switch_Publisher()
{
}

Boolean_Publisher::Boolean_Publisher(String _name, String _descr, bool * _isActive) :
	Publisher_Element(_name, _descr),
	isActive(_isActive)
{
	__class_name = "Boolean_Publisher";
}


Boolean_Publisher::~Boolean_Publisher()
{
}
