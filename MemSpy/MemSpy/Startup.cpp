// MemSpy.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

#include "CommandLine.hpp"
#include "Application.hpp"

int _tmain(int argc, TCHAR** argv)
{
	memspy::CommandLine cmd(argc, argv);

	memspy::Application app;
	return app.Run(cmd);
}
