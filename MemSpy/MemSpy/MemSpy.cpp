// MemSpy.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

#include "CommandLine.hpp"
#include "Application.hpp"

int _tmain(int argc, TCHAR** argv)
{
	ms::CommandLine cmd(argc, argv);

	ms::Application app;
	app.Run(cmd);
}
