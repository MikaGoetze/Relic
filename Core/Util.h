#ifndef UTIL_H
#define UTIL_H

#include <string>

class Util
{
public:
	Util();
	~Util();

	static void ErrorExit();
	static void SuccessExit();
	static void Log(std::string log);
	static void CleanLogFile();
};

#endif
