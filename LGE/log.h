#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

//namespace Cgl
//{
static std::ofstream LogFile;
class Log
{
public:
	static Log& GetInstance();
	~Log();
	static void PrintInfo(const std::string& message, bool bWriteMessageToLog = true);
	static void PrintWarning(const std::string& message, bool bWriteMessageToLog = true);
	static void PrintError(const std::string& message, bool bWriteMessageToLog = true);
private:

	Log();
	static void LogMessage(const std::string& level, const std::string& message);;
	Log(const Log&) = delete;
	Log& operator=(const Log&) = delete;
};


//}