#ifndef _PUBLISHER_h
#define _PUBLISHER_h

#include "Arduino.h"
#include <sstream>
#include "m_Vector.h"
#include "Base_Consts.h"

enum func_type
{
	text,
	pass,
	color,
	value,
	select,
	button
};

class Atomic_Base
{
private:
	int __id;
	String __unit;
	int GetId() const { return __id; }
	String GetUnit() const { return __unit; }
public:
	Atomic_Base(int _Id, String _unit) { __id = _Id; __unit = _unit; }
	virtual  ~Atomic_Base() {}

	Property<int, Atomic_Base> id{ this, nullptr, &Atomic_Base::GetId };
	Property<String, Atomic_Base> unit{ this, nullptr, &Atomic_Base::GetUnit };
};

class Atomic_Impl : public Atomic_Base
{
public:
	Atomic_Impl(int _id, String _unit) : Atomic_Base(_id, _unit) {}
	virtual String ValueToString() const = 0;
};

template <class T>
class Atomic : public Atomic_Impl
{
private:
	T __value;
	T GetValue() const { return __value; }
	void SetValue(T _value) { __value = _value; };
public:
	Atomic(int _id, T _value, String _unit = "") : Atomic_Impl(_id, _unit) { __value = _value; }
	virtual  ~Atomic() {}
	virtual String ValueToString() const { return String(__value); }
	Property<T, Atomic> value{ this,  &Atomic::SetValue, &Atomic::GetValue };
};

class Ctrl_Elem
{
private:
	Vector<Atomic_Base*> *__vec_atomic;
	func_type __type;
	int __id;
	String __name;
	String __descr;
	int __atomic_count;
	bool __published;

	bool GetPublished() const { return __published; }
	String GetName() const { return __name; }
	String GetDescr() const { return __descr; }
	int GetId() const { return __id; }
	func_type GetType() const { return __type; }
	void SetPublished(bool _published) { __published = _published; }
	int GetAtomicCount() const { return __atomic_count; }
public:
	Ctrl_Elem(int _id, String _name, func_type _type, String _descr = "Control Element Description") :
		__id(_id),
		__name(_name),
		__type(_type),
		__descr(_descr),
		__atomic_count(0)
	{
		__vec_atomic = new Vector<Atomic_Base*>;
	}

	virtual ~Ctrl_Elem() {
		delete __vec_atomic;
	}

	void AddAtomic(Atomic_Base *atomic) {
		__vec_atomic->PushBack(atomic);
		__atomic_count++;
	}

	Atomic_Base *GetAtomicByIndex(int _index) {
		if ((index >= 0) && (_index < __atomic_count)) {
			return (*__vec_atomic)[_index];
		}
		else {
			return nullptr;
		}
	}

	Property<int, Ctrl_Elem> id{ this, nullptr, &Ctrl_Elem::GetId };
	Property<String, Ctrl_Elem> name{ this, nullptr, &Ctrl_Elem::GetName };
	Property<func_type, Ctrl_Elem> type{ this, nullptr, &Ctrl_Elem::GetType };
	Property<String, Ctrl_Elem> descr{ this, nullptr, &Ctrl_Elem::GetDescr };
	Property<int, Ctrl_Elem> atomic_count{ this, nullptr, &Ctrl_Elem::GetAtomicCount };
	Property<bool, Ctrl_Elem> published{ this, &Ctrl_Elem::SetPublished, &Ctrl_Elem::GetPublished };
};


class Descriptor
{
private:
	Vector<Ctrl_Elem*> *__vec_ctrl_elem;
	int __id;
	String __name;
	String __descr;
	bool __published;
	int __ctrl_elem_count;

	int GetCtrlElemCount() const { return __ctrl_elem_count; }
	int GetId() const { return __id; }
	bool GetPublished() const { return __published; }
	void SetPublished(bool _published) { __published = _published; }
	String GetName() const { return __name; }
	void SetName(String _name) { __name = _name; }
	String GetDescr() const { return __descr; }
	void SetDescr(String _descr) { __descr = _descr; }
public:
	Descriptor(int _id) :
		__id(_id),
		__name("Driver Descriptor"),
		__descr("Driver Description"),
		__ctrl_elem_count(0),
		__published(false) {
		__vec_ctrl_elem = new Vector<Ctrl_Elem*>;
	};
	virtual ~Descriptor() { delete  __vec_ctrl_elem; }

	Ctrl_Elem *GetCtrlElemByIndex(int _index) {
		if ((index >= 0) && (_index < __ctrl_elem_count))
			return (*__vec_ctrl_elem)[_index];
		else
			return nullptr;
	}
	void Add_Descriptor_Element(Ctrl_Elem *_ctrl_elem) {
		__vec_ctrl_elem->PushBack(_ctrl_elem);
		__ctrl_elem_count++;
	}

	Property<int, Descriptor> id{ this, nullptr, &Descriptor::GetId };
	Property<String, Descriptor> name{ this, &Descriptor::SetName, &Descriptor::GetName };
	Property<String, Descriptor> descr{ this, &Descriptor::SetDescr, &Descriptor::GetDescr };
	Property<int, Descriptor> ctrl_count{ this, nullptr, &Descriptor::GetCtrlElemCount };
	Property<bool, Descriptor> published{ this, &Descriptor::SetPublished, &Descriptor::GetPublished };
};

class Descriptor_List
{
private:
	int __elem_count;
	Vector<Descriptor*> *__vec_descriptor_elem;
	int GetElemCount() const { return __elem_count; }
public:
	Descriptor_List() {
		__vec_descriptor_elem = new Vector<Descriptor*>;
		__elem_count = 0;
	}
	virtual ~Descriptor_List() { delete  __vec_descriptor_elem; }

	void Add_Descriptor(Descriptor *_new_elem) {
		__vec_descriptor_elem->PushBack(_new_elem);
		__elem_count++;
	}

	void Clear() {
		__vec_descriptor_elem->Clear();
		__elem_count = 0;
	}

	Descriptor *GetElemByIndex(int _index) {
		if ((index >= 0) && (_index < __elem_count))
			return (*__vec_descriptor_elem)[_index];
		else
			return nullptr;
	}

	Property<int, Descriptor_List> count{ this, nullptr, &Descriptor_List::GetElemCount };
};

//-----------------------------------------------------
// Observer Class			
//-----------------------------------------------------
class Observer
{
public:
	virtual ~Observer() {};		// Destructor
	virtual void Notify(Descriptor_List *_descriptor_list) = 0;
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
	bool NotifyObservers(Descriptor_List *_descriptor_list) {
		for (int i = 0; i < m_ObserverVec->Size(); i++) {
			(*m_ObserverVec)[i]->Notify(_descriptor_list);
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