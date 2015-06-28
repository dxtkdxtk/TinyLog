//desc: a small logging library
//author: jebin


#ifndef TINY_LOG_H
#define TINY_LOG_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

namespace TL
{
	namespace Type
	{
		typedef const char* MsgType;
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

	using namespace Type;
	class TinyLog
	{
		
	public:
		TinyLog();
		TinyLog(const string &logger_name);
		void SetFilePath(const string &path);
		void SetLoggerName(const string &logger_name);
		// info logging
		template<typename T, typename... Args>
		void info(MsgType msg, const T& value, const Args&... args)
		{
			lock_guard<mutex> lg(msg_lock_);
			Log_(Level::Info, msg, value, args);
		}
		// error logging
		template<typename T, typename... Args>
		void error(MsgType msg, const T& value, const Args&... args)
		{
			lock_guard<mutex> lg(msg_lock_);
			Log_(Level::Error, msg, value, args);
		}

	private:
		// mutex logging
		template<typename T, typename... Args>
		Log_(Level level, MsgType msg, const T& value)
		{
			
		}

		// mutex logging
		template<typename T, typename... Args>
		Log_(Level level, MsgType msg, const T& value, const Args&... args)
		{
			auto now = std::chrono::system_clock::now();
			
		}

		//get level string from level
		string GetLevelString_(Level level)
		{
			switch (level)
			{
			case Level::Global:
				return "GLOBAL";
			case Level::Error:
				return "ERROR";
			case Level::Fatal:
				return "FATAL";
			case Level::Debug:
				return "DEBUG";
			case Level::Info:
				return "INFO";
			case Level::Trace:
				return "TRACE";
			case Level::Verbose:
				return "VERBOSE";
			case Level::Warning:
				return "WARNING";
			default:
				return "UNKNOWN";
			}
		}
		string GetNowString_()
		{
			auto now = std::chrono::system_clock::now();

			chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(
				now.time_since_epoch());
			
			
			auto in_time_t = std::chrono::system_clock::to_time_t(now);

			std::stringstream ss;
			ss << std::put_time(std::localtime(&in_time_t), "[%F %T.");
			
			return ss.str() + to_string(ms.count() % 1000) + "]";
		}
	private:
		string filepath_;
		string logger_name_;
		mutex msg_lock_;
		
	};
}



#endif