#include "Util.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>


Util::Util()
{
}


Util::~Util()
{
}

void Util::ErrorExit()
{
	int c;
	printf("Engine exited in an errored state. Press ENTER to continue... ");
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
	exit(1);
}

void Util::SuccessExit()
{
	int c;
	printf("Engine shutdown complete. Press ENTER to continue... ");
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
}

void Util::Log(std::string log)
{
	std::ofstream ofs;
	ofs.open("relic_log.txt", std::ofstream::out | std::ofstream::app);
	std::cout << log << std::endl;
	ofs << log << std::endl;
	ofs.close();
}

void Util::CleanLogFile()
{
	std::ofstream ofs;
	ofs.open("relic_log.txt", std::ofstream::out | std::ofstream::trunc);
	ofs.close();
}
