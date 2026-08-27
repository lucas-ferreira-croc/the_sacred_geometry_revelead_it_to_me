#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>

class Logger
{
public:
	template<typename...Args>
	static void log(unsigned int logLevel, Args...args) 
	{
		if(logLevel <= m_LogLevel)
		{
			std::printf(args...);
			// force output
			std::fflush(stdout);
		}
	}

	static void setLogLevel(unsigned int inLogLevel)
	{
		inLogLevel <= 9 ? m_LogLevel = inLogLevel : m_LogLevel = 9;
	}

private:
	static unsigned int m_LogLevel;
};


#endif