//desc: a tiny logging library
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

#include <Windows.h>
#include <direct.h>
using namespace std;

namespace tinylog
{
	
	namespace type
	{
		//log msg type
		typedef const char* MsgType;
		//enum type
		typedef unsigned short EnumType;
		// verbose level type
		typedef unsigned short VLevelType;
		//log level type
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

	namespace consts
	{
		const string default_logger_name = "default";
		const string default_logpath = "./logs/";
		const type::VLevelType default_vlevel = 0;
		const string default_file_name = "tinylog";
		const string default_file_suffix = ".log";
	}

	using namespace type;
	using namespace consts;
	class TinyLog
	{
		
	public:
		TinyLog() : 
			logger_name_(default_logger_name),
			log_path_(default_logpath),
			vlevel_(default_vlevel),
			b_toconsole_(true),
			b_tofile_(true),
			log_path_suffix_(0),
			file_name_(default_file_name)
		{
			PreprocessLogger_();
		}
		TinyLog(const string &logger_name) :
			logger_name_(logger_name),
			log_path_(default_logpath),
			vlevel_(default_vlevel),
			b_toconsole_(true),
			b_tofile_(true),
			log_path_suffix_(0),
			file_name_(default_file_name)
		{
			PreprocessLogger_();
		}
		~TinyLog()
		{
			ofile_.close();
		}
		void SetLogPath(const string &log_path)
		{
			log_path_ = log_path;
			log_path_suffix_ = 0;
			PreprocessLogger_();
		}
		void SetLoggerName(const string &logger_name)
		{
			logger_name_ = logger_name;
			log_path_suffix_ = 0;
			PreprocessLogger_();
		}
		void SetFileName(const string &file_name)
		{
			file_name_ = file_name;
			log_path_suffix_ = 0;
			PreprocessLogger_();
		}
		void SwitchToConsole(bool to_console)
		{
			b_toconsole_ = to_console;
		}
		void SwitchToFile(bool to_file)
		{
			b_tofile_ = to_file;
		}

//macro of all loggers
#define TL_LOGGER_WRITTER(FUNCTION_NAME, LOG_LEVEL) \
	template<typename T, typename... Args>\
	void FUNCTION_NAME(MsgType msg, const T& value, const Args&... args)\
		{\
			lock_guard<mutex> lg(msg_lock_);\
			if (b_toconsole_)\
				cout << GetNowString_() << " [" << logger_name_ << "] " << GetLevelString_(LOG_LEVEL) + " ";\
			if (b_tofile_)\
				ofile_ << GetNowString_() << " [" << logger_name_ << "] " << GetLevelString_(LOG_LEVEL) + " ";\
			Log_(msg, value, args...);\
		}\
		void FUNCTION_NAME(MsgType msg)\
		{\
			if (b_toconsole_)\
				cout << GetNowString_() << " [" << logger_name_ << "] " << GetLevelString_(LOG_LEVEL) + " "; \
			if (b_tofile_)\
				ofile_ << GetNowString_() << " [" << logger_name_ << "] " << GetLevelString_(LOG_LEVEL) + " "; \
			Log_(msg);\
		}

		// define all logger function
		TL_LOGGER_WRITTER(info, type::Level::Info);
		TL_LOGGER_WRITTER(error, type::Level::Error);
		TL_LOGGER_WRITTER(debug, type::Level::Debug);
		TL_LOGGER_WRITTER(global, type::Level::Global);
		TL_LOGGER_WRITTER(fatal, type::Level::Fatal);
		TL_LOGGER_WRITTER(warning, type::Level::Warning);
		TL_LOGGER_WRITTER(trace, type::Level::Trace);

	private:
		// mutex logging
		void Log_(MsgType msg)
		{
			if (b_tofile_)
				ofile_ << msg << endl;
			if (b_toconsole_)
				cout << msg << endl;
		}

		// mutex logging
		template<typename T, typename... Args>
		void Log_(MsgType msg, const T& value, const Args&... args)
		{
			while (*msg)
			{
				if (*msg == '%')
				{
					if (*(msg + 1) == '%')
					{
						++msg;
					}
					else
					{
						if (b_toconsole_)
							cout << value;
						if (b_tofile_)
							ofile_ << value;
						msg += 2;
						Log_(msg, args...);
						return;
					}
				}
				if (b_toconsole_)
					cout << *msg++;
				if (b_tofile_)
					ofile_ << *msg++;
			}
			Log_("");
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

		//get now string
		string GetNowString_()
		{
			time_t t;
			tm timeinfo;
			_time64(&t);
			localtime_s(&timeinfo, &t);

			timeval tv;
			gettimeofday_(&tv);

			char buffer[80];
			sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
				timeinfo.tm_year + 1900,
				timeinfo.tm_mon + 1,
				timeinfo.tm_mday,
				timeinfo.tm_hour,
				timeinfo.tm_min,
				timeinfo.tm_sec,
				tv.tv_usec / 1000);

			return string(buffer);
		}

		void gettimeofday_(struct timeval* tv)
		{

			if (tv != nullptr) {
				const unsigned __int64 delta_ = 11644473600000000Ui64;
				const double secOffSet = 0.000001;
				const unsigned long usecOffSet = 1000000;
				FILETIME fileTime;
				GetSystemTimeAsFileTime(&fileTime);
				unsigned __int64 present = 0;
				present |= fileTime.dwHighDateTime;
				present = present << 32;
				present |= fileTime.dwLowDateTime;
				present /= 10;  // mic-sec
				// Subtract the difference
				present -= delta_;
				tv->tv_sec = static_cast<long>(present * secOffSet);
				tv->tv_usec = static_cast<long>(present % usecOffSet);
			}
		}
		
		bool IsExistFile_(const string &file)
		{
			ifstream f(file.c_str());
			if (f.good())
			{
				f.close();
				return true;
			}
			else
			{
				f.close();
				return false;
			}
			return false;
		}

		//makedir
		void makedirs_(const char *dir)
		{
			if (NULL == dir)
				return;

			size_t len = strlen(dir);

			char * p = new char[len + 1];
			strcpy(p, dir);
			for (int i = 0; i < len; ++i)
			{
				char ch = p[i];
				if ('\\' == ch || '/' == ch)
				{
					p[i] = '\0';
					_mkdir(p);
					p[i] = ch;
				}
			}
			if (p[len - 1] != '\\' && p[len - 1] != '/')
				_mkdir(p);
			delete[] p;
		}
		//pre-process logger name
		void PreprocessLogger_()
		{
			ofile_.close();
			makedirs_(log_path_.c_str());
			if (log_path_[log_path_.size() - 1] != '/' && log_path_[log_path_.size() - 1] != '\\')
				log_path_ += '/';

			string logfile = log_path_ + file_name_;
			string tmpfile = logfile;
			//每次获取log文件不重复
			if (file_name_ != default_file_name)
			{
				while (IsExistFile_(tmpfile + ".log"))
				{
					tmpfile = logfile + "_" + to_string(log_path_suffix_);
					++log_path_suffix_;
				}
			}
			
			logfile = tmpfile + ".log";
			ofile_.open(logfile, ios::app);
		}

	private:
		string log_path_;
		int log_path_suffix_;
		string logger_name_;
		string file_name_;
		mutex msg_lock_;
		VLevelType vlevel_;
		bool b_toconsole_;
		bool b_tofile_;
		ofstream ofile_;
	};
}

#endif
