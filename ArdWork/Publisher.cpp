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

Publisher_Element::Publisher_Element(String _lable, String _descr) :
	lable(_lable),
	descr(_descr)
{
};

Publisher_Element::~Publisher_Element()
{
}



Button_Publisher::Button_Publisher(String _lable, String _descr) :
	Publisher_Element(_lable, _descr)
{
	atomic_cmd_elem = nullptr;
	__class_name = "Button_Publisher";
};

Button_Publisher::~Button_Publisher()
{
}


int Button_Publisher::GetCommand() const
{
	if (atomic_cmd_elem)
	{
		return atomic_cmd_elem->cmdId;
	}
	else {
		return -1;
	}
}

void Button_Publisher::SetCommand(int _cmdId)
{
	if (atomic_cmd_elem)
	{
		delete atomic_cmd_elem;
		atomic_cmd_elem = nullptr;
	}
	atomic_cmd_elem = new Atomic_Command_Element(_cmdId);
}


Switch_Publisher::Switch_Publisher(String _lable, String _descr) :
	Publisher_Element(_lable, _descr)
{
	__atomic_On_cmd_elem = nullptr;
	__atomic_Off_cmd_elem = nullptr;
	__atomic_status_attr_elem = nullptr;
	__class_name = "Switch_Publisher";
}

int Switch_Publisher::GetOnCommand() const {
	if (__atomic_On_cmd_elem)
	{
		return __atomic_On_cmd_elem->cmdId;
	}
	else {
		return -1;
	}
}

void Switch_Publisher::SetOnCommand(int _cmdId) {
	if (__atomic_On_cmd_elem)
	{
		delete __atomic_On_cmd_elem;
		__atomic_On_cmd_elem = nullptr;
	}
	__atomic_On_cmd_elem = new Atomic_Command_Element(_cmdId);
}

int Switch_Publisher::GetOffCommand() const {
	if (__atomic_Off_cmd_elem)
	{
		return __atomic_Off_cmd_elem->cmdId;
	}
	else {
		return -1;
	}
}

void Switch_Publisher::SetOffCommand(int _cmdId) {
	if (__atomic_Off_cmd_elem)
	{
		delete __atomic_Off_cmd_elem;
		__atomic_Off_cmd_elem = nullptr;
	}
	__atomic_Off_cmd_elem = new Atomic_Command_Element(_cmdId);
}


bool *Switch_Publisher::GetStatus() const {
	if (__atomic_status_attr_elem)
	{
		return __atomic_status_attr_elem->atrribute;
	}
	else {
		return nullptr;
	}
}


void Switch_Publisher::SetStatus(bool *_status) {
	if (__atomic_status_attr_elem)
	{
		delete __atomic_status_attr_elem;
		__atomic_status_attr_elem = nullptr;
	}
	__atomic_status_attr_elem = new Atomic_Attribute_Element<bool>(_status);
}




//template <class T>
//Value_Publisher<T>::Value_Publisher(String _lable, String _descr)
//{
//	Publisher_Element(_lable, _descr);
//	__atomic_value_attr_elem = nullptr;
//	__class_name = "Value_Publisher";
//};
//
//template <class T>
//T *Value_Publisher<T>::GetValue() const {
//	if (__atomic_value_attr_elem)
//	{
//		return __atomic_value_attr_elem->atrribute;
//	}
//	else {
//		return nullptr;
//	}
//}
//
//template <class T>
//void Value_Publisher<T>::SetValue(T *_status) {
//	if (__atomic_value_attr_elem)
//	{
//		delete __atomic_value_attr_elem;
//		__atomic_value_attr_elem = nullptr;
//	}
//	__atomic_value_attr_elem = new Atomic_Attribute_Element<T>(_status);
//}

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