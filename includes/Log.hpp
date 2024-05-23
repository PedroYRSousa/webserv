#ifndef LOG_HPP
#define LOG_HPP

#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <csignal>
#include <unistd.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 255
#endif // BUFFER_SIZE
#ifndef LOG_PATH
#define LOG_PATH "./logs"
#endif // LOG_PATH
#ifndef ClEAR_COLOR
#define CLEAR_COLOR "\033[0m"
#endif // ClEAR_COLOR
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0b0010000
#endif // DEBUG_LEVEL
#ifndef DEBUG_COLOR
#define DEBUG_COLOR "\033[0;36m"
#endif // DEBUG_COLOR
#ifndef INFO_LEVEL
#define INFO_LEVEL 0b1000
#endif // INFO_LEVEL
#ifndef INFO_COLOR
#define INFO_COLOR "\033[0;32m"
#endif // INFO_COLOR
#ifndef WARN_LEVEL
#define WARN_LEVEL 0b0100
#endif // WARN_LEVEL
#ifndef WARN_COLOR
#define WARN_COLOR "\033[0;33m"
#endif // WARN_COLOR
#ifndef ERROR_LEVEL
#define ERROR_LEVEL 0b0010
#endif // ERROR_LEVEL
#ifndef ERROR_COLOR
#define ERROR_COLOR "\033[0;31m"
#endif // ERROR_COLOR
#ifndef FATAL_LEVEL
#define FATAL_LEVEL 0b0001
#endif // FATAL_LEVEL
#ifndef FATAL_COLOR
#define FATAL_COLOR "\033[1;31m"
#endif // FATAL_COLOR
#ifndef ALL_LEVEL
#define ALL_LEVEL FATAL_LEVEL | ERROR_LEVEL | WARN_LEVEL | INFO_LEVEL | DEBUG_LEVEL
#endif // ALL_LEVEL
#ifndef LOG_EOF
#define LOG_EOF "EOF\n\0"
#endif // LOG_EOF

class Log
{
public:
	static Log debug;
	static Log info;
	static Log warn;
	static Log error;
	static Log fatal;
	static const std::string eof;

	static int getLevelLog(void);
	static void setLevelLog(int level);
	template <typename T>
	Log &operator<<(const T &value)
	{
		if (Log::getLevelLog() >= this->level)
		{
			std::stringstream ss;
			ss << value;

			if (ss.str() == LOG_EOF)
			{
				this->print();

				if (level == FATAL_LEVEL)
					kill(getpid(), SIGINT);
			}
			else
				this->buffer.append(ss.str());
		}
		return *this;
	}

private:
	static int levelLog;
	static std::string logFile;

	int level;
	std::string color;
	std::string buffer;
	std::string levelStr;

	Log(const std::string &color, int level);

	void print(void);
	void printInFile(const std::string &content, int day, int mon, int year, int hour, int min, int sec);
};

#endif // LOG_HPP
