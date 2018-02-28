// 
// 
// 

#include "Int_Task_Msg.h"
#include "Support.h"


Int_Task_Msg::Int_Task_Msg(uint32 _id) {
	__id = _id;
	__param_count = 0;
}

void Int_Task_Msg::AddParam(bool boolean)
{
	__str_param += boolean;
	__str_param += ":";
	__param_count++;
}

void Int_Task_Msg::AddParam(int intNumber) {
	__str_param += intNumber;
	__str_param += ":";
	__param_count++;
}

void Int_Task_Msg::AddParam(unsigned char c) {
	__str_param += char(c);
	__str_param += ":";
	__param_count++;
}

void Int_Task_Msg::AddParam(String text) {
	__str_param += text;
	__str_param += ":";
	__param_count++;
}

void Int_Task_Msg::AddParam(float floatNumber) {
	__str_param += floatNumber;
	__str_param += ":";
	__param_count++;
}

uint8_t Int_Task_Msg::GetUint8ParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return (uint8_t)ParamStr.toInt();
}

int Int_Task_Msg::GetIntParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return ParamStr.toInt();
}
bool Int_Task_Msg::GetBoolParamByIndex(int index) {
	String ParamStr;
	bool tempResult = false;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	if (ParamStr.toInt() > 0) {
		tempResult = true;
	}
	else {
		tempResult = false;
	}
	return tempResult;
}

float Int_Task_Msg::GetFloatParamByIndex(int index)
{
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return ParamStr.toFloat();
}

String Int_Task_Msg::GetStringParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return ParamStr;
}

uint8_t Int_Task_Msg::GetCharParamByIndex(int index) {
	String ParamStr;
	unsigned char result;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	result = (unsigned char)*(ParamStr.c_str());
	return result;
}



