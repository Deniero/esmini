#include <stdarg.h> 
#include <iostream>

#include "CommonMini.hpp"

#define DEBUG_TRACE
#define LOG_FILENAME "log.txt"


#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)

	#include <windows.h>

	__int64 SE_getSystemTime()
	{
		return timeGetTime();
	}

	void SE_sleep(unsigned int msec)
	{
		Sleep(msec);
	}

#else

	#include <thread>
	#include <chrono>
	using namespace std::chrono;

	__int64 SE_getSystemTime()
	{
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	void SE_sleep(unsigned int msec)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(msec)));
	}

#endif

std::string DirNameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : fname.substr(0, pos);
}

std::string FileNameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : fname.substr(pos+1);
}

Logger::Logger()
{
	file_.open(LOG_FILENAME);
	if (file_.fail())
	{
		throw std::iostream::failure(std::string("Cannot open file: ") + LOG_FILENAME);
	}
	callback_ = 0;
}

Logger::~Logger()
{
	file_.close();
	callback_ = 0;
}

void Logger::Log(char const* file, char const* func, int line, char const* format, ...)
{
	static char complete_entry[2048];
	static char message[1024];

	va_list args;
	va_start(args, format);
	vsnprintf(message, 1024, format, args);

#ifdef DEBUG_TRACE
	snprintf(complete_entry, 2048, "%s / %d / %s(): %s", file, line, func, message);
#else
	strncpy(complete_entry, message, 1024);
#endif
	
	file_ << complete_entry << std::endl;
	if (callback_)
	{
		callback_(complete_entry);
	}

	file_.flush();
	va_end(args);
}

Logger& Logger::Inst()
{
	static Logger instance;
	return instance;
}
