#pragma once
#include <string>
#include "RelicBehaviour.h"

class Util
{
public:
	Util();
	~Util();

	static void ErrorExit();
	static void SuccessExit();
	static std::string GetClassName(RelicBehaviour* behaviour);
	static void Log(std::string log);
	static void CleanLogFile();
};

