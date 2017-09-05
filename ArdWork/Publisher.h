#ifndef _PUBLISHER_h
#define _PUBLISHER_h

#include "Arduino.h"
#include <sstream>
#include "m_Vector.h"
#include "Base_Consts.h"

#define default_cmdId 0

class Atomic_Element
{

};

class Atomic_Command_Element
{
private:
	int __cmdId;
	int GetCmdId() const { return __cmdId; }
public:
	Atomic_Command_Element(int _cmdId) { __cmdId = _cmdId; }
	virtual  ~Atomic_Command_Element() {}
	Property<int, Atomic_Command_Element> cmdId{ this, nullptr, &Atomic_Command_Element::GetCmdId };
};


template <class T>
class Atomic_Attribute_Element
{
private:
	String __unit;
	T *__value;
	T *GetAttribute() const { return __value; }
public:
	Atomic_Attribute_Element(T *_pAttribute) { __value = _pAttribute; }
	virtual  ~Atomic_Attribute_Element() {}
	Property<T*, Atomic_Attribute_Element> atrribute{ this, nullptr, &Atomic_Attribute_Element::GetAttribute };
};

class Publisher_Element
{
protected:
	String __class_name;
public:
	virtual String GetClassName();
	Publisher_Element(String _lable, String _descr);
	virtual  ~Publisher_Element();
	String lable;
	String descr;
};


class Value_Publisher : public Publisher_Element
{
private:
	String __unit;
	String __type;
	void SetUnit(String _unit) { __unit = _unit; }
	void SetType(String _type) { __type = _type; }
	String GetUnit() const { return __unit; }
	String GetType() const { return __type; }
public:
	Value_Publisher(String _lable, String _descr) :
		Publisher_Element(_lable, _descr) {
		__class_name = "Value_Publisher";
		__unit = "";
		__type = "text";
	}
	virtual ~Value_Publisher() {}
	virtual String ValueToString() const = 0;
	Property<String, Value_Publisher> unit{ this, &Value_Publisher::SetUnit, &Value_Publisher::GetUnit };
	Property<String, Value_Publisher> type{ this, &Value_Publisher::SetType, &Value_Publisher::GetType };
};


template <class T>
class Value_Publisher_Impl : public Value_Publisher
{
private:
	Atomic_Attribute_Element<T> *__atomic_value_attr_elem;
	T *GetValue() const {
		if (__atomic_value_attr_elem)
		{
			return __atomic_value_attr_elem->atrribute;
		}
		else {
			return nullptr;
		}
	}

	void SetValue(T *_valuePointer) {
		if (__atomic_value_attr_elem)
		{
			delete __atomic_value_attr_elem;
			__atomic_value_attr_elem = nullptr;
		}
		__atomic_value_attr_elem = new Atomic_Attribute_Element<T>(_valuePointer);
	}
public:
	Value_Publisher_Impl(String _lable, String _descr) :
		Value_Publisher(_lable, _descr) {
		__atomic_value_attr_elem = nullptr;
	}
	
	virtual String ValueToString() const
	{
		String result = "";
		if (__atomic_value_attr_elem) {
			result = String(*__atomic_value_attr_elem->atrribute);
		}
		return result;
	}
	Property<T*, Value_Publisher_Impl> value{ this, &Value_Publisher_Impl::SetValue, &Value_Publisher_Impl::GetValue };
};


class Button_Publisher : public Publisher_Element
{
private:
	Atomic_Command_Element *atomic_cmd_elem;
	int GetCommand() const;
	void SetCommand(int _cmdId);
public:
	Button_Publisher(String _lable, String _descr);
	virtual  ~Button_Publisher();
	Property<int, Button_Publisher> cmdId{ this, &Button_Publisher::SetCommand, &Button_Publisher::GetCommand };
};


class Switch_Publisher : public Publisher_Element
{
private:
	Atomic_Attribute_Element<bool> *__atomic_status_attr_elem;
	Atomic_Command_Element *__atomic_On_cmd_elem;
	Atomic_Command_Element *__atomic_Off_cmd_elem;
	int GetOnCommand() const;
	void SetOnCommand(int _cmdId);
	int GetOffCommand() const;
	void SetOffCommand(int _cmdId);

	bool *GetStatus() const;
	void SetStatus(bool *_status);
public:
	Switch_Publisher(String _lable, String _descr);
	virtual  ~Switch_Publisher() {}
	Property<int, Switch_Publisher> cmdOnId{ this, &Switch_Publisher::SetOnCommand, &Switch_Publisher::GetOnCommand };
	Property<int, Switch_Publisher> cmdOffId{ this, &Switch_Publisher::SetOffCommand, &Switch_Publisher::GetOffCommand };
	Property<bool*, Switch_Publisher> status{ this, &Switch_Publisher::SetStatus, &Switch_Publisher::GetStatus };
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


//-----------------------------------------------------
// Observer Class			
//-----------------------------------------------------
class Observer
{
public:
	virtual ~Observer() {};		// Destructor
	virtual void Notify(Vector <Publisher*> *pub_list) = 0;
protected:
	//constructor is protected because this class is abstract, it’s only meant to be inherited!
	Observer() {};
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Observer(const Observer& yRef) = delete;
	Observer& operator=(const Observer& yRef) = delete;
};

class Observee
{
public:
	virtual ~Observee() { m_ObserverVec->Clear(); m_ObserverVec = nullptr; };        //destructor
	bool AddObserver(Observer* observer) {
		if (m_ObserverVec->Find(observer) > -1) {
			return false;
		}

		m_ObserverVec->PushBack(observer);
		return true;
	};
	bool RemoveObserver(Observer* observer) {
		int temp = m_ObserverVec->Find(observer);
		if (temp > -1) {
			m_ObserverVec->Erase(temp);
			return true;
		}
		else {
			return false;
		}
	};
	bool NotifyObservers(Vector <Publisher*> *pub_list) {
		for (int i = 0; i < m_ObserverVec->Size(); i++) {
			(*m_ObserverVec)[i]->Notify(pub_list);
		}
	};
protected:
	//constructor is protected because this class is abstract, it’s only meant to be inherited!
	Observee() { m_ObserverVec = new Vector<Observer*>; };
private:
	Vector<Observer*> *m_ObserverVec;
	// -------------------------
	// Disabling default copy constructor and default
	// assignment operator.
	// -------------------------
	Observee(const Observee& yRef) = delete;
	Observee& operator=(const Observee& yRef) = delete;
};


#endif