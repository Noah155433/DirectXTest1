#include <iostream>
#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//Create a pointer to Systemclass
	SystemClass* System;
	bool result;

	//Create new systemclass
	System = new SystemClass;

	//Initialize the new system
	result = System->Initialize();
	if (result)
	{
		//Run the system
		System->Run();
	}

	//Delete the system
	System->Shutdown();
	delete System;
	System = 0;
	
	return 0;
}
