#ifndef _PUBLISHER_h
#define _PUBLISHER_h
#pragma once 

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "m_Vector.h"
#include "Base_Consts.h"
#include "Filesystem.h"
#include "ArduinoJson.h"
#include "Driver.h"
#include "Timezone.h" //https://github.com/JChristensen/Timezone
#include "ping.h"
//#define DEBUG
//#define LOAD_SAVE_DEBUG



struct Color
{
	int R;
	int G;
	int B;
};

static String FloatToStr(float _value, uint8_t _digits) {
	int int_val = round(_value);
	unsigned count = 0;
	//Abfrage der Stellen
	do ++count; while (int_val /= 10);
	char val_text[10];
	dtostrf(_value, count + _digits, _digits, val_text);
	return String(val_text);
}

static FileSystem filesystem;

enum ctrl_type
{
	none,
	input,
	pass,
	color,
	value,
	select,
	group, 
	zeit
};

class CtrlElem
{
private:
	int __id;
	String __name;
	String __description;
	String __unit;
	bool __published;

	int GetId() const { return __id; }
	String GetName() const { return __name; }
	String GetDescrirption() const { return __description; }
	void SetUnit(String _unit) { __unit = _unit; }
	String GetUnit() const { return __unit; }
	ctrl_type GetType() const { return __type; }
	bool GetPublished() const { return __published; }
	void SetPublished(bool _published) { __published = _published; }
protected:
	void *__supervisor;
	ctrl_type __type;
public:
	CtrlElem(int _id, void* _supervisor = nullptr, String _name = "Control Element Name", String _description = "Control Element Description") :
		__id(_id),
		__supervisor(_supervisor),
		__name(_name),
		__description(_description)
	{
		__type = none;
		__unit = "";
		__published = true;
	};

	virtual String ToString() = 0;

	Property<int, CtrlElem> id{ this, nullptr, &CtrlElem::GetId };
	Property<String, CtrlElem> name{ this, nullptr, &CtrlElem::GetName };
	Property<ctrl_type, CtrlElem> type{ this, nullptr, &CtrlElem::GetType };
	Property<String, CtrlElem> unit{ this, &CtrlElem::SetUnit, &CtrlElem::GetUnit };
	Property<String, CtrlElem> description{ this, nullptr, &CtrlElem::GetDescrirption };
	Property<bool, CtrlElem> published{ this, &CtrlElem::SetPublished, &CtrlElem::GetPublished };
};

class Select_CtrlElem : public CtrlElem
{
private:
	class StringContainer
	{
	private:
		String __value;

	public:
		StringContainer(String _input) { __value = _input; }
		String ToString() { return __value; }
	};

	Vector<StringContainer*> *__string_container;
public:
	Select_CtrlElem(int _id, void* _supervisor, String _name = "Select", String _descr = "decr") :
		CtrlElem(_id, _supervisor, _name, _descr)
	{
		__type = select;
		__string_container = new Vector<StringContainer*>;
	};

	void AddMember(String _option) {
		StringContainer *temp = new StringContainer(_option);
		__string_container->PushBack(temp);
	}

	uint8_t MembersCount() {
		return __string_container->Size();
	}

	String GetMember(uint8_t _index) {
		String return_value = "miss_hit_Select_CtrlElem";
		if ((_index > -1) && (_index < __string_container->Size()))
			return_value = ((StringContainer*)(*__string_container)[_index])->ToString();
		return return_value;
	}

	String ToString() {
		return String(*reinterpret_cast<int*>(__supervisor));
	}

};


class Group_CtrlElem : public Select_CtrlElem
{
public:
	Group_CtrlElem(int _id, String _name = "Group", String _descr = "decr") :
		Select_CtrlElem(_id, nullptr, _name, _descr)
	{
		__type = group;
	}

	String ToString() {
		return "";
	}
};

class Input_CtrlElem :public CtrlElem
{
public:
	Input_CtrlElem(int _id, String* _supervisor, String _name = "Text", String _descr = "decr") :
		CtrlElem(_id, (void*)_supervisor, _name, _descr)
	{
		__type = input;
	};

	String ToString() {
		return String(*((String*)__supervisor)) + unit;
	};
};


class Password_CtrlElem :public CtrlElem
{
public:
	Password_CtrlElem(int _id, String* _supervisor, String _name = "Password", String _descr = "decr") :
		CtrlElem(_id, (void*)_supervisor, _name, _descr)
	{
		__type = pass;
	};

	String ToString() {
		return String(*((String*)__supervisor)) + unit;
	};
};

class Color_CtrlElem :public CtrlElem
{
public:
	Color_CtrlElem(int _id, void* _supervisor, String _name = "Color", String _descr = "decr") :
		CtrlElem(_id, _supervisor, _name, _descr)
	{
		__type = color;
	};

	String ToString() {
		Color v_color = *reinterpret_cast<Color*>(__supervisor);
		char buf[12];
		String result_string;

		char r[255];
		sprintf(r, "%.2X", v_color.R);
		result_string = result_string + r;

		char g[255];
		sprintf(g, "%.2X", v_color.G);
		result_string = result_string + g;

		char b[255];
		sprintf(b, "%.2X", v_color.B);
		result_string = result_string + b;

		return result_string;
	};
};


class Time_CtrlElem :public CtrlElem
{
private:
	bool __editable;
public:
	Time_CtrlElem(int _id, long int* _supervisor, bool _editable, String _name = "Time", String _descr = "decr") :
		CtrlElem(_id, (void*)_supervisor, _name, _descr),
		__editable(_editable)
	{
		__type = zeit;
	};

	bool IsEditable() { return __editable; }
	
	String TimeToString() {
		time_t v_time = *reinterpret_cast<time_t*>(__supervisor);
		String resultString = "";
		resultString += hour(v_time); resultString += (":");
		resultString += minute(v_time) / 10; resultString += minute(v_time) % 10; resultString += (":");
		resultString += second(v_time) / 10; resultString += second(v_time) % 10;
	}

	String DateToString() {
		time_t v_time = *reinterpret_cast<time_t*>(__supervisor);
		String resultString = "";
		resultString += day(v_time); resultString += ("."); resultString += month(v_time); resultString += ("."); resultString += year(v_time);
	}

	String ToString() {
		time_t v_time = *reinterpret_cast<time_t*>(__supervisor);
		String resultString = "";
		resultString += hour(v_time); resultString += (":");
		resultString += minute(v_time) / 10; resultString += minute(v_time) % 10; resultString += (":");
		resultString += second(v_time) / 10; resultString += second(v_time) % 10; resultString += (" - ");
		resultString += day(v_time); resultString += ("."); resultString += month(v_time); resultString += ("."); resultString += year(v_time);
		return resultString;
	};
};


class FValue_CtrlElem :public CtrlElem
{
private:
	bool __editable;
public:
	FValue_CtrlElem(int _id, float* _supervisor, bool _editable, String _name = "FValue", String _descr = "decr") :
		CtrlElem(_id, (void*)_supervisor, _name, _descr),
		__editable(_editable)
	{
		__type = value;
	};

	bool IsEditable() { return __editable; }

	String ToString() {
		String temp = FloatToStr(*((float*)__supervisor), 0);
		return temp;
	};
};

class IValue_CtrlElem :public CtrlElem
{
private:
	bool __editable;
public:
	IValue_CtrlElem(int _id, int* _supervisor, bool _editable, String _name = "IValue", String _descr = "decr") :
		CtrlElem(_id, (void*)_supervisor, _name, _descr),
		__editable(_editable)
	{
		__type = value;
	};

	bool IsEditable() { return __editable; }

	String ToString() {
		return String(*((int*)__supervisor));
	};
};


class Descriptor
{
private:
	Vector<CtrlElem*> *__vec_ctrlelem;
	int __id;
	String __name;
	String __descr;
	bool __published;
	uint8_t __ctrl_elem_count;

	uint8_t GetCtrlElemCount() const { return __ctrl_elem_count; }
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
		__vec_ctrlelem = new Vector<CtrlElem*>;
	};
	virtual ~Descriptor() { delete  __vec_ctrlelem; }

	CtrlElem *GetCtrlElemByIndex(uint8_t _index) {
		CtrlElem* return_value = nullptr;
		if ((index >= 0) && (_index < __ctrl_elem_count)) {
			return_value = (*__vec_ctrlelem)[_index];
		}
		return return_value;
	}

	bool GetCtrlElemByIndex(uint8_t _index, CtrlElem *_elem) {
		bool return_value = false;
		if ((index >= 0) && (_index < __ctrl_elem_count)) {
			_elem = (*__vec_ctrlelem)[_index];
			return_value = true;
		}
		return return_value;
	}

	bool GetCtrlElemById(int _id, CtrlElem *_elem) {
		bool return_value = false;
		for (size_t i = 0; i < __ctrl_elem_count; i++) {
			CtrlElem * t_ctrlelem = (*__vec_ctrlelem)[i];
			if (t_ctrlelem->id == _id) {
				_elem = (*__vec_ctrlelem)[i];
				return_value = true;
				break;
			}
		}
		return return_value;
	}

	void Add_Descriptor_Element(CtrlElem *_ctrl_elem) {
		__vec_ctrlelem->PushBack(_ctrl_elem);
		__ctrl_elem_count++;
	}

	Property<int, Descriptor> id{ this, nullptr, &Descriptor::GetId };
	Property<String, Descriptor> name{ this, &Descriptor::SetName, &Descriptor::GetName };
	Property<String, Descriptor> descr{ this, &Descriptor::SetDescr, &Descriptor::GetDescr };
	Property<uint8_t, Descriptor> ctrl_count{ this, nullptr, &Descriptor::GetCtrlElemCount };
	Property<bool, Descriptor> published{ this, &Descriptor::SetPublished, &Descriptor::GetPublished };
};


class Descriptor_List
{
private:
	uint8_t __elem_count;
	Vector<Descriptor*> *__vec_descriptor_elem;
	uint8_t GetElemCount() const { return __elem_count; }
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

	void Load(int _driverID, void(*fw_Exec_Command)(void*, int, String), void* context) {
#ifdef  LOAD_SAVE_DEBUG
		Serial.println("Start Descriptor_List::Load");
#endif //  LOAD_SAVE_DEBUG
		DynamicJsonBuffer jsonBuffer;
		String file_text;

		filesystem.OpenFile("/testfile.json");
		file_text = filesystem.FileAsString();
		JsonObject& root = jsonBuffer.parseObject(file_text);
		filesystem.CloseFile();

		if (!root.success()) {
			Serial.println("parseObject - failed");
			return;
		}

		JsonArray& arr_descriptors = root["Descriptors"];
		Descriptor *ptr_descriptor;
		for (auto obj_descriptor : arr_descriptors) {
			//for (size_t i = 0; i < __elem_count; i++) {
				//Descriptor * ptr_descriptor = (*__vec_descriptor_elem)[i];
			if (_driverID == (int)obj_descriptor["id"]) {
				JsonArray& arr_controls = obj_descriptor["Controls"];
				for (auto obj_control : arr_controls) {
					int device_id = obj_descriptor["id"];
					int contrl_id = obj_control["id"];
					int contrl_type = obj_control["type"];
					String value = obj_control["value"];

#ifdef  LOAD_SAVE_DEBUG
					Serial.print(" Load -> ");
					Serial.print(" - DeviceId: ");
					Serial.print(device_id);
					Serial.print(" - CtrlId: ");
					Serial.print(contrl_id);
					Serial.print(" - Type: ");
					Serial.print(contrl_type);
					Serial.print(" - Value: ");
					Serial.println(value);
#endif //  LOAD_SAVE_DEBUG	

					if ((contrl_id > 0) && !value.equals("") && (contrl_type != group)) {
						fw_Exec_Command(context, contrl_id, value);
					}
				}
			}
			//}
		}

#ifdef  LOAD_SAVE_DEBUG
		Serial.println("Loaded!");
		Serial.println("JSON-Text: ");
		root.printTo(Serial);
		Serial.println("JSON-Text Finished!");
#endif //  LOAD_SAVE_DEBUG	

#ifdef  LOAD_SAVE_DEBUG
		Serial.println("Ende Descriptor_List::Load");
#endif //  LOAD_SAVE_DEBUG
	}

	void Save() {
		DynamicJsonBuffer jsonBuffer;
#ifdef  LOAD_SAVE_DEBUG
		Serial.println("JSON-File - create new one");
#endif //  LOAD_SAVE_DEBUG	
		JsonObject& root = jsonBuffer.createObject();
		JsonArray& arr_descriptors = root.createNestedArray("Descriptors");
		for (size_t i = 0; i < __elem_count; i++)
		{
			JsonObject& descriptor = arr_descriptors.createNestedObject();
			descriptor["id"] = (int)(*__vec_descriptor_elem)[i]->id;
			JsonArray& arr_controls = descriptor.createNestedArray("Controls");
			for (size_t j = 0; j < (*__vec_descriptor_elem)[i]->ctrl_count; j++)
			{
				JsonObject& obj_control = arr_controls.createNestedObject();
#ifdef  LOAD_SAVE_DEBUG
				Serial.print(" Add - ");
				Serial.print("descriptorId: "); Serial.print((*__vec_descriptor_elem)[i]->id);
				Serial.print("ctrlId: "); Serial.print(j);
				Serial.print(" - id: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->id);
				Serial.print(" - type: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->type);
				Serial.print(" - value: "); Serial.println((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->ToString());
#endif //  LOAD_SAVE_DEBUG
				obj_control["id"] = (int)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->id;
				obj_control["type"] = (int)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->type;
				obj_control["value"] = (String)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->ToString();
			}
		}
		String text;
		root.printTo(text);
		filesystem.SaveToFile("/testfile.json", text);
	}

	void ModifySave() {
#ifdef  LOAD_SAVE_DEBUG
		Serial.println("Start Descriptor_List::Save");
#endif //  LOAD_SAVE_DEBUG
		DynamicJsonBuffer jsonBuffer;


		String file_text;
		filesystem.OpenFile("/testfile.json");
		file_text = filesystem.FileAsString();

#ifdef  LOAD_SAVE_DEBUG
		Serial.print("Load JSON-File:");
		Serial.println(file_text);
		Serial.println("- Ende JSON-File -");
#endif //  LOAD_SAVE_DEBUG	


		if (file_text.equals(""))
			Serial.println("File is empty");

		if (file_text.equals("{}")) {
#ifdef  LOAD_SAVE_DEBUG
			Serial.println("no entries found - erase file!");
#endif //  LOAD_SAVE_DEBUG	
			file_text = "";
		}

		JsonObject& root = jsonBuffer.parseObject(file_text);
		filesystem.CloseFile();

		if (!root.success()) {
#ifdef  LOAD_SAVE_DEBUG
			Serial.println("can not parse JSON-File - create new one");
#endif //  LOAD_SAVE_DEBUG	
			JsonObject& new_root = jsonBuffer.createObject();
			JsonArray& arr_descriptors = new_root.createNestedArray("Descriptors");
			for (size_t i = 0; i < __elem_count; i++)
			{
				JsonObject& descriptor = arr_descriptors.createNestedObject();
				descriptor["id"] = (int)(*__vec_descriptor_elem)[i]->id;
				JsonArray& arr_controls = descriptor.createNestedArray("Controls");
				for (size_t j = 0; j < (*__vec_descriptor_elem)[i]->ctrl_count; j++)
				{
					JsonObject& obj_control = arr_controls.createNestedObject();
#ifdef  LOAD_SAVE_DEBUG
					Serial.print(" Add - ");
					Serial.print("descriptorId: "); Serial.print((*__vec_descriptor_elem)[i]->id);
					Serial.print("ctrlId: "); Serial.print(j);
					Serial.print(" - id: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->id);
					Serial.print(" - type: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->type);
					Serial.print(" - value: "); Serial.println((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->ToString());
#endif //  LOAD_SAVE_DEBUG
					obj_control["id"] = (int)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->id;
					obj_control["type"] = (int)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->type;
					obj_control["value"] = (String)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->ToString();
				}
			}
			String text;
			new_root.printTo(text);
			filesystem.SaveToFile("/testfile.json", text);
#ifdef  LOAD_SAVE_DEBUG
			filesystem.OpenFile("/testfile.json");
			file_text = filesystem.FileAsString();
			JsonObject& test_root = jsonBuffer.parseObject(file_text);

			if (!test_root.success()) {
				Serial.println("parseObject() failed");
				return;
			}
			filesystem.CloseFile();
			Serial.println("Saved!");
			Serial.println(text);
#endif //  LOAD_SAVE_DEBUG	
		}
		else {
#ifdef  LOAD_SAVE_DEBUG
			Serial.println("JSON-File parsed successfully");
#endif //  LOAD_SAVE_DEBUG
			JsonArray& arr_descriptors = root["Descriptors"];
			Descriptor *ptr_descriptor;

			for (size_t i = 0; i < __elem_count; i++) {
				Descriptor * ptr_descriptor = (*__vec_descriptor_elem)[i];
				bool found = false;
				for (auto obj_descriptor : arr_descriptors) {
					if (ptr_descriptor->id == (int)obj_descriptor["id"]) {
						found = true;
#ifdef  LOAD_SAVE_DEBUG
						Serial.print("Found DeviceID: ");
						Serial.print(ptr_descriptor->id);
						Serial.println(" in JSON-file - modify entry!");
#endif //  LOAD_SAVE_DEBUG
						JsonArray& arr_controls = obj_descriptor["Controls"];
						for (auto obj_control : arr_controls) {
							for (size_t j = 0; j < ptr_descriptor->ctrl_count; j++) {
								CtrlElem * ptr_control = (*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j);
								if (ptr_control->id == (int)obj_control["id"]) {
									obj_control["type"] = (int)ptr_control->type;
									obj_control["value"] = (String)ptr_control->ToString();

#ifdef  LOAD_SAVE_DEBUG
									Serial.print(" Modify - ");
									Serial.print("descriptorId: "); Serial.print(ptr_descriptor->id);
									Serial.print("ctrlId: "); Serial.print(j);
									Serial.print(" - id: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->id);
									Serial.print(" - type: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->type);
									Serial.print(" - value: "); Serial.println((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->ToString());
#endif //  LOAD_SAVE_DEBUG

								}
							}
						}
					}
				}
				if (!found) {
#ifdef  LOAD_SAVE_DEBUG
					Serial.print("No match for DeviceID: ");
					Serial.print(ptr_descriptor->id);
					Serial.println(" in JSON-file - create new entry!");
#endif //  LOAD_SAVE_DEBUG
					JsonObject& descriptor = arr_descriptors.createNestedObject();
					descriptor["id"] = (int)ptr_descriptor->id;
					JsonArray& arr_controls = descriptor.createNestedArray("Controls");
					for (size_t j = 0; j < ptr_descriptor->ctrl_count; j++)
					{
						JsonObject& obj_control = arr_controls.createNestedObject();

#ifdef  LOAD_SAVE_DEBUG
						Serial.print(" Add - ");
						Serial.print("descriptorId: "); Serial.print(ptr_descriptor->id);
						Serial.print("ctrlId: "); Serial.print(j);
						Serial.print(" - id: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->id);
						Serial.print(" - type: "); Serial.print((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->type);
						Serial.print(" - value: "); Serial.println((*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->ToString());
#endif //  LOAD_SAVE_DEBUG
						obj_control["id"] = (int)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->id;
						obj_control["type"] = (int)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->type;
						obj_control["value"] = (String)(*__vec_descriptor_elem)[i]->GetCtrlElemByIndex(j)->ToString();
					}

				}
			}
			String text;
			root.printTo(text);
			filesystem.SaveToFile("/testfile.json", text);



#ifdef  LOAD_SAVE_DEBUG
			filesystem.OpenFile("/testfile.json");
			file_text = filesystem.FileAsString();
			JsonObject& test_root = jsonBuffer.parseObject(file_text);
			filesystem.CloseFile();

			if (!test_root.success()) {
				Serial.println("parseObject() failed");
				return;
			}


			Serial.println("Saved!");
			Serial.println(text);
#endif //  LOAD_SAVE_DEBUG	
		}



#ifdef  LOAD_SAVE_DEBUG
		Serial.println("Ende Descriptor_List::Save");
#endif //  LOAD_SAVE_DEBUG
	}

	void Clear() {
		__vec_descriptor_elem->Clear();
		__elem_count = 0;
	}

	bool GetElemByIndex(int _index, Descriptor * _descriptor) {
		bool return_value = false;
		if ((index >= 0) && (_index < __elem_count)) {
			_descriptor = (*__vec_descriptor_elem)[_index];
			return_value = true;
		}
		return return_value;
	}

	Descriptor *GetElemByIndex(int _index) {
		Descriptor * return_value = nullptr;
		if ((index >= 0) && (_index < __elem_count)) {
			return_value = (*__vec_descriptor_elem)[_index];
		}
		return return_value;
	}

	bool GetElemById(int _id, Descriptor * _descriptor) {
		bool return_value = false;
		for (size_t i = 0; i < __elem_count; i++) {
			Descriptor *t_descr = (*__vec_descriptor_elem)[i];
			if (t_descr->id == _id) {
				t_descr = (*__vec_descriptor_elem)[i];
				return_value = true;
				break;
			}
		}
		return return_value;
	}
	Property<uint8_t, Descriptor_List> count{ this, nullptr, &Descriptor_List::GetElemCount };
};



//-----------------------------------------------------
// Observer Class			
//-----------------------------------------------------
class Observer
{
public:
	virtual ~Observer() {};		// Destructor
	virtual void OnNotifyInitDone(int _DriverID) = 0;
	virtual void OnNotifyStartupDone(int _DriverID) = 0;
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

	void NotifyModuleStartupDone(int _DriverId) {
		for (int i = 0; i < m_ObserverVec->Size(); i++) {
			(*m_ObserverVec)[i]->OnNotifyStartupDone(_DriverId);
		}
	};

	void NotifyModuleInitDone(int _DriverId) {
		for (int i = 0; i < m_ObserverVec->Size(); i++) {
			(*m_ObserverVec)[i]->OnNotifyInitDone(_DriverId);
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



//-----------------------------------------------------
// CommunicationClient Class			
//-----------------------------------------------------
class CommunicationClient
{
public:
	virtual ~CommunicationClient() {};		// Destructor
	void NotifyConnected() { __isConnected = true; __isOnline = false;  OnNotifyConnected(); };
	void NotifyConnectionLost() { __isConnected = false; __isOnline = false;  OnNotifyConnectionLost(); };
	void NotifyOnline() { __isConnected = true; __isOnline = true; OnNotifyOnline(); };
	virtual void OnNotifyConnected() = 0;
	virtual void OnNotifyConnectionLost() = 0;
	virtual void OnNotifyOnline() = 0;
protected:
	bool __isOnline;
	bool __isConnected;
	//constructor is protected because this class is abstract, it’s only meant to be inherited!
	CommunicationClient() { __isOnline = false; __isConnected = false; };
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	CommunicationClient(const CommunicationClient& yRef) = delete;
	CommunicationClient& operator=(const CommunicationClient& yRef) = delete;
};

class CommunicationProvider
{
public:
	virtual ~CommunicationProvider() { m_CommunicationClientVec->Clear(); m_CommunicationClientVec = nullptr; };        //destructor
	bool AddCommunicationClient(CommunicationClient* _communicationClient) {
		if (m_CommunicationClientVec->Find(_communicationClient) > -1) {
			return false;
		}

		m_CommunicationClientVec->PushBack(_communicationClient);
		return true;
	};
	bool RemoveCommunicationClient(CommunicationClient* _communicationClient) {
		int temp = m_CommunicationClientVec->Find(_communicationClient);
		if (temp > -1) {
			m_CommunicationClientVec->Erase(temp);
			return true;
		}
		else {
			return false;
		}
	};

	void DoNotifyConnected() {
		for (int i = 0; i < m_CommunicationClientVec->Size(); i++) {
			(*m_CommunicationClientVec)[i]->NotifyConnected();
		}
	};

	void DoNotifyConnectionLost() {
		for (int i = 0; i < m_CommunicationClientVec->Size(); i++) {
			(*m_CommunicationClientVec)[i]->NotifyConnectionLost();
		}
	};

	void DoNotifyOnline() {
		for (int i = 0; i < m_CommunicationClientVec->Size(); i++) {
			(*m_CommunicationClientVec)[i]->NotifyOnline();
		}
	};

protected:
	//constructor is protected because this class is abstract, it’s only meant to be inherited!
	CommunicationProvider() { m_CommunicationClientVec = new Vector<CommunicationClient*>; };
private:
	Vector<CommunicationClient*> *m_CommunicationClientVec;
	// -------------------------
	// Disabling default copy constructor and default
	// assignment operator.
	// -------------------------
	CommunicationProvider(const CommunicationProvider& yRef) = delete;
	CommunicationProvider& operator=(const CommunicationProvider& yRef) = delete;
};



#endif