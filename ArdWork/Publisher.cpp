#include "Publisher.h"

Publisher::Publisher(uint8_t _deviceId):
	__deviceId(_deviceId),
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
	return __deviceId;
}


String Publisher_Element::GetClassName()
{
	return __class_name;
}

Publisher_Element::Publisher_Element(uint16_t _cmdId, String _lable, String _descr) :
	cmdId(_cmdId),
	lable(_lable),
	descr(_descr)
{
};

Publisher_Element::~Publisher_Element()
{
}


Button_Publisher::Button_Publisher(uint16_t _cmdId, String _lable, String _descr) :
	Publisher_Element(_cmdId, _lable, _descr)
{
	__class_name = "Button_Publisher";
};

Button_Publisher::~Button_Publisher()
{
}


Value_Publisher::Value_Publisher(uint16_t _cmdId, String _lable, String _descr, float *_value, String _unit) :
	Publisher_Element(_cmdId, _lable, _descr),
	value(_value),
	unit(_unit)
{
	__class_name = "Value_Publisher";
};

Value_Publisher::~Value_Publisher()
{
}

Switch_Publisher::Switch_Publisher(uint16_t _cmdId, String _lable, String _descr, String _cmdOn, String, String _cmdOff) :
	Publisher_Element(_cmdId, _lable, _descr),
	cmdOn(_cmdOn),
	cmdOff(_cmdOff)
{
	__class_name = "Switch_Publisher";
}

Switch_Publisher::~Switch_Publisher()
{
}

Boolean_Publisher::Boolean_Publisher(uint16_t _cmdId, String _lable, String _descr, bool * _isActive) :
	Publisher_Element(_cmdId, _lable, _descr),
	isActive(_isActive)
{
	__class_name = "Boolean_Publisher";
}


Boolean_Publisher::~Boolean_Publisher()
{
}


//<!DOCTYPE html>
//<html>
//<body>
//
//<p>This example demonstrates how to assign an "onchange" event to an input element.< / p>
//
//Enter your name : <input type = "checkbox" id = "fname" onchange = "myFunction()">
//
//<p>When you leave the input field, a function is triggered which transforms the input text to upper case.< / p>
//
//<script>
//function myFunction() {
//	var x = document.getElementById("fname");
//	document.getElementById("demo").innerHTML = "You selected: " + x.checked;
//}
//< / script>
//
//<p id = "demo">< / p>
//
//< / body>
//< / html>
