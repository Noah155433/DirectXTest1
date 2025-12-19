#include "systemclass.h"

//Handles windows creation, message loops and subsystem initialization

SystemClass::SystemClass()
{
	//Initialize all pointers to zero
	m_Input = 0;
	m_Application = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	//Create a new window
	InitializeWindows(screenWidth, screenHeight);

	//Create and initalize input system
	m_Input = new InputClass;
	m_Input->Initialize();

	//Create new application
	m_Application = new ApplicationClass;
	
	//Initialize the application
	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;

}

void SystemClass::Shutdown()
{

	//Shut down and delete all pointers

	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	ShutdownWindows();

	return;

}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//Clear MSG large block of memory at the address of msg
	ZeroMemory(&msg, sizeof(MSG));
	
	done = false;
	while (!done)
	{
		//Process windows messages from the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		//if the message is WM_QUIT, quit the while loop
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		//Run the frame function
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
	return;
}

bool SystemClass::Frame()
{
	bool result;

	//Check if esc is pressed
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//Run m_applications frame function
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;

}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	//Switch depending on what message is sent
	switch (umsg)
	{
		//If the message is key down, set that key to pressed in m_Input
	case WM_KEYDOWN:
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
		//If the message is key up, set that key to not pressed in m_Input
	case WM_KEYUP:
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		break;
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//Get a reference to the application
	ApplicationHandle = this;

	//Get the EXE's instance handle
	m_hinstance = GetModuleHandle(NULL);

	//Set the applications name to TEST
	m_applicationName = L"TEST";

	//Set window styling options
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register a window class with the OS
	RegisterClassEx(&wc);

	//Set the screen width and height to the width and height of the primary window
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Check if the application should be in fullscreen or not
	if (FULL_SCREEN)
	{
		//Set the address at dmScreenSettings to 0
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		//Set the screen settings
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
		//Set windowed application size
		screenWidth = 800;
		screenHeight = 600;

		//Set windowed application position
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window and assign it to the hwnd variable
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//Show the window
	ShowWindow(m_hwnd, SW_SHOW);
	//Set the window to the foreground
	SetForegroundWindow(m_hwnd);
	//Focus the window
	SetFocus(m_hwnd);

	//Hide the cursor
	ShowCursor(false);

	return;

}

void SystemClass::ShutdownWindows()
{
	//Show the cursor
	ShowCursor(true);

	//Check if applications is in fullscreen or not
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//Destroy the window and set the hwnd to NULL
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//Unregister the window class with the OS
	UnregisterClass(m_applicationName, m_hinstance);
	//Set the instance to null
	m_hinstance = NULL;

	//Set the application handle to null
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	//Handle different quit messages
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}