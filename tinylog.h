//desc: a small logging library
//author: jebin


#ifndef TINY_LOG_H
#define TINY_LOG_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

using namespace std;

namespace TL
{
	namespace type
	{
		typedef unsigned short EnumType;
		enum class Level : EnumType
		{
			/// @brief Generic level that represents all the levels. Useful when setting global configuration for all levels
			Global = 1,
			/// @brief Information that can be useful to back-trace certain events - mostly useful than debug logs.
			Trace = 2,
			/// @brief Informational events most useful for developers to debug application
			Debug = 4,
			/// @brief Severe error information that will presumably abort application
			Fatal = 8,
			/// @brief Information representing errors in application but application will keep running
			Error = 16,
			/// @brief Useful when application has potentially harmful situtaions
			Warning = 32,
			/// @brief Information that can be highly useful and vary with verbose logging level.
			Verbose = 64,
			/// @brief Mainly useful to represent current progress of application
			Info = 128,
			/// @brief Represents unknown level
			Unknown = 1010
		};
	}


	class TinyLog
	{
	public:
		TinyLog();
		TinyLog(const string &logger_name);
		void SetFilePath(const string &path);
		void SetLoggerName(const string &logger_name);
		//logging
		template<typename T, typename... Args>
		void info();
		void error(const char *, ...);
	private:
		string filepath_;
		string logger_name_;
	};
}



#endif