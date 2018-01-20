#ifndef _FILESYSTEM_h
#define _FILESYSTEM_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <ArduinoJson.h>
#include "FS.h"




class FileSystem
{
private:
	File actFile;
	bool FileIsOpen;
	String s_file;
	
public:
	FileSystem() {
		FileIsOpen = false;
		s_file = "";
		Serial.println("Mounting FS...");
		if (!SPIFFS.begin())
			Serial.println("Failed to mount file system");
	}

	bool OpenFile(String FileName) {
		actFile = SPIFFS.open(FileName.c_str(), "r");
		if (!actFile) {
			Serial.println("Failed to open config file");
			return false;
		}
		size_t size = actFile.size();
		if (size > 2048) {
			Serial.println("Config file size is too large");
			return false;
		}
		s_file = actFile.readString();
		FileIsOpen = true;
		actFile.close();
		return true;
	}
	bool CloseFile() {
		actFile.close();
		FileIsOpen = false;
		s_file = "";
	}


	bool SaveToFile(String FileName, JsonObject &json) {
		actFile = SPIFFS.open(FileName.c_str(), "w");
		if (!actFile) {
			Serial.println("Failed to open config file for writing");
			return false;
		}
		json.printTo(actFile);
		actFile.close();
		return true;
	}

	bool SaveToFile(String FileName, String text) {
		actFile = SPIFFS.open(FileName.c_str(), "w");
		if (!actFile) {
			Serial.println("Failed to open config file for writing");
			return false;
		}
		actFile.println(text.c_str());
		actFile.close();
		return true;
	}
	
	String FileAsString() {
		String value = "";
		if (FileIsOpen)
			value = s_file;
		return s_file;
	}

	String Json_GetvalueFromKey(String key) {
		String value = "";
		std::unique_ptr<char[]> buf(new char[s_file.length() + 1]);
		// We don't use String here because ArduinoJson library requires the input
		// buffer to be mutable. If you don't use ArduinoJson, you may as well
		// use configFile.readString instead.
		
		s_file.toCharArray(buf.get(), s_file.length() + 1);
		
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(buf.get());
		
		if (!root.success()) {
			Serial.println("Failed to parse config file");
		}
		value = root[key].as<String>();
		return value;
	}
};




#endif


