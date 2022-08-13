// 
// Main.cpp
//
// This is the entry point for the application. This file is a simple, thin
// wrapper for creating the App object and handling command line parameters
//

#include "App/App.h"
#include "App/PhysicsTest.h"

int WinMain(const int argc, const char** argv)
{
 	App app;
//	PhysicsTest app;

	app.Initialize();
	int returnCode = app.Run();

	return returnCode;
}
