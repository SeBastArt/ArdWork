#include "Publisher.h"

Publisher::Publisher(int _driverId) :
	__driverId(_driverId),
	name("Device"),
	descr("Description Text")
{
	pub_elem_list = new Vector<Publisher_Element*>;
	__elem_count = 0;
	published = false;
}

void Publisher::Add_Publisher_Element(Publisher_Element *_elem)
{
	pub_elem_list->PushBack(_elem);
	__elem_count++;
}

Publisher_Element *Publisher::GetElemByIndex(int _index)
{
	Publisher_Element *result_elem = nullptr;
	if ((index >= 0) && (_index < __elem_count))
		result_elem = (*pub_elem_list)[_index];
	return result_elem;
}

int Publisher::GetElemCount() const
{
	return __elem_count;
}
int Publisher::GetDriverId() const
{
	return __driverId;
}


String Publisher_Element::GetClassName()
{
	return __class_name;
}

Publisher_Element::Publisher_Element(int _cmdId, String _lable, String _descr) :
	cmdId(_cmdId),
	lable(_lable),
	descr(_descr)
{
};

Publisher_Element::~Publisher_Element()
{
}


Button_Publisher::Button_Publisher(int _cmdId, String _lable, String _descr) :
	Publisher_Element(_cmdId, _lable, _descr)
{
	__class_name = "Button_Publisher";
};

Button_Publisher::~Button_Publisher()
{
}


Value_Publisher::Value_Publisher(String _lable, String _descr, float *_value, String _unit) :
	Publisher_Element(default_cmdId, _lable, _descr),
	value(_value),
	unit(_unit)
{
	__class_name = "Value_Publisher";
};

Value_Publisher::~Value_Publisher()
{
}

Switch_Publisher::Switch_Publisher(int _cmdId, String _lable, String _descr) :
	Publisher_Element(_cmdId, _lable, _descr)
{
	__class_name = "Switch_Publisher";
}

Switch_Publisher::~Switch_Publisher()
{
}

Boolean_Publisher::Boolean_Publisher(String _lable, String _descr, bool * _isActive) :
	Publisher_Element(default_cmdId, _lable, _descr),
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
