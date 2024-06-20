#pragma once
#ifndef D3DDEBUG_H
#define D3DDEBUG_H

#include <iostream>  
#include <iomanip>  
#include <fstream>  
#include <string>  
#include <cstdlib>  
#include <stdint.h>  
#include <sstream> 
#include <Windows.h>
#include <vector>
#include "MessageLogContext.h"

enum LogType
{
	Info,
	Warning,
	Error,
	Default,
};

// 负责处理Debug信息的类
class Debug
{
public:
	struct Stream {
		Stream() :ss(), space(true), context() {}
		Stream(std::string* s) :ss(*s), space(true), context() {}
		std::ostringstream ss;
		bool space;
		MessageLogContext context;
		LogType logType;
	} *stream;

	Debug() : stream(new Stream()) {  }
	inline Debug(std::string* s) : stream(new Stream(s)) {}
	~Debug();
	inline Debug& operator<<(bool t) { stream->ss << (t ? "true" : "false"); return maybeSpace(); }
	inline Debug& operator<<(char t) { stream->ss << t; return maybeSpace(); }
	inline Debug& operator<<(signed short t) { stream->ss << t; return maybeSpace(); }
	inline Debug& operator<<(unsigned short t) { stream->ss << t; return maybeSpace(); }
	inline Debug& operator<<(std::string s) { stream->ss << s; return maybeSpace(); }
	inline Debug& operator<<(const char* c) { stream->ss << c; return maybeSpace(); }
	inline Debug& space() { stream->space = true; stream->ss << ' '; return *this; }
	inline Debug& nospace() { stream->space = false; return *this; }
	inline Debug& maybeSpace() { if (stream->space) stream->ss << ' '; return *this; }

	template <typename T>
	inline Debug& operator<<(const std::vector<T>& vec)
	{
		stream->ss << ' ';
		for (int i = 0; i < vec.size(); ++i) {
			stream->ss << vec.at(i);
			stream->ss << ", ";
		}
		stream->ss << ' ';
		return maybeSpace();
	}

	void LogToConsole(LogType type, const MessageLogContext& context, std::string logBuffer);

private:
	static Debug* _instance;
};
#endif