// MessageQ.h

#ifndef _MESSAGE_h
#define _MESSAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Base_Consts.h"

class Int_Task_Msg
{
private:
	uint32 __id;
	uint8_t __param_count;
	String __str_param;
	String GetStringPartByNr(String data, char separator, int index);
	uint32 GetID() const { return __id; }
	uint8_t GetParamCount() const { return __param_count; }
	String GetParamString()const { return __str_param; }
public:
	Int_Task_Msg(uint32 _id);
	void AddParam(bool boolean);
	void AddParam(int intNumber);
	void AddParam(unsigned char c);
	void AddParam(String text);
	void AddParam(float floatNumber);
	uint8_t GetUint8ParamByIndex(int index);
	int GetIntParamByIndex(int index);
	float GetFloatParamByIndex(int index);
	bool GetBoolParamByIndex(int index);
	unsigned char GetCharParamByIndex(int index);
	String GetStringParamByIndex(int index);

	Property<uint32, Int_Task_Msg> id{ this, nullptr, &Int_Task_Msg::GetID };
	Property<uint8_t, Int_Task_Msg> param_count{ this, nullptr, &Int_Task_Msg::GetParamCount };
	Property<String, Int_Task_Msg> param_string{ this, nullptr, &Int_Task_Msg::GetParamString };
};


#endif


