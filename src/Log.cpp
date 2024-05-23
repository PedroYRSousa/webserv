#include "Log.hpp"

// STATIC LOCAL
static void getLocalDateTime(int *day, int *mon, int *year, int *hour, int *min, int *sec)
{
	std::time_t now;

	std::time(&now);
	std::tm *localDate = std::localtime(&now);

	(*day) = localDate->tm_mday;
	(*mon) = localDate->tm_mon + 1;
	(*year) = localDate->tm_year + 1900;
	(*hour) = localDate->tm_hour;
	(*min) = localDate->tm_min;
	(*sec) = localDate->tm_sec;
}
// STATIC PUBLIC
const std::string Log::eof = LOG_EOF;
Log Log::debug(DEBUG_COLOR, DEBUG_LEVEL);
Log Log::info(INFO_COLOR, INFO_LEVEL);
Log Log::warn(WARN_COLOR, WARN_LEVEL);
Log Log::error(ERROR_COLOR, ERROR_LEVEL);
Log Log::fatal(FATAL_COLOR, FATAL_LEVEL);
int Log::getLevelLog(void)
{
	return Log::levelLog;
}
void Log::setLevelLog(int level)
{
	if (level > DEBUG_LEVEL)
		level = ALL_LEVEL;
	if (level < FATAL_LEVEL)
		level = FATAL_LEVEL;

	Log::levelLog = level;
}
// STATIC PROTECTED
// STATIC PRIVATE
int Log::levelLog = WARN_LEVEL;
std::string Log::logFile = "";
// PUBLIC
// PROTECTED
// PRIVATE
Log::Log(const std::string &color, int level) : level(level), color(color)
{
	if (level == DEBUG_LEVEL)
		this->levelStr = "Debug";
	if (level == INFO_LEVEL)
		this->levelStr = "Info ";
	if (level == WARN_LEVEL)
		this->levelStr = "Warn ";
	if (level == ERROR_LEVEL)
		this->levelStr = "Error";
	if (level == FATAL_LEVEL)
		this->levelStr = "Fatal";
}
void Log::print(void)
{
	int day, mon, year;
	int hour, min, sec;

	getLocalDateTime(&day, &mon, &year, &hour, &min, &sec);

	std::stringstream ss;
	ss << std::setfill('0')
	   << "[" << this->levelStr << "]\t"														   // [ LEVEL ]
	   << "[" << std::setw(2) << mon << "/" << std::setw(2) << day << "/" << year << " "		   // 00/00/0000
	   << std::setw(2) << hour << ":" << std::setw(2) << min << ":" << std::setw(2) << sec << "] " // [00:00:00]
	   << this->buffer << ";";																	   // [ LEVEL ]	[00/00/0000 00:00:00] BUFFER;

	if (Log::logFile != "ERROR")
		printInFile(ss.str(), day, mon, year, hour, min, sec);

	std::cerr << this->color;
	std::cerr << ss.str();
	std::cerr << CLEAR_COLOR << std::endl;

	this->buffer.clear();
}
void Log::printInFile(const std::string &content, int day, int mon, int year, int hour, int min, int sec)
{
	std::ofstream file;

	if (Log::logFile == "")
	{
		char _buffer[BUFFER_SIZE] = {};
		std::sprintf(_buffer, "%s/log_%d_%d_%d_%d_%d_%d.txt", LOG_PATH, mon, day, year, hour, min, sec);
		Log::logFile = std::string(_buffer);
	}

	file.open(Log::logFile.c_str(), std::ios::out | std::ios::app);

	if (file.is_open())
	{
		file << content << std::endl;
		file.close();
	}
	else
	{
		Log::logFile = "ERROR";
		Log::fatal << "Unable to open log file for writing." << Log::eof;
	}
}
