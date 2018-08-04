#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Timer = 0;
	m_Fps = 0;
	m_ShaderManager = 0;
	m_Zone = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{

}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;

	m_Input = new InputClass;
	if (!m_Input)
		return false;

	//Create and initialize the input object. The input class will handle registering all the keyboard and mouse input
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}


	//Create and initialize the Direct3D object. This object will handle the main portions of the DirectX 11 graphics interface
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
		return false;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	//Create and initialize the shader manager object. The ShaderManager is the wrapper classes for all DirectX 11 shader functionality. 
	//Encapsulating everything into this manager style class allows us to pass a single pointer into any class or function and they now have access to all of the shaders we have written.
	m_ShaderManager = new ShaderManagerClass;
	if (!m_ShaderManager)
		return false;

	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}


	m_Timer = new TimerClass;
	if (!m_Timer)
		return false;

	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}


	m_Fps = new FpsClass;
	if (!m_Fps)
		return false;
	m_Fps->Initialize();


	//Create and initialize the zone object. The zone object is where we put all of our terrain rendering and processing. 
	//This class will be used to easily instantiate new terrain zones and encapsulate anything else related to the terrain such as trees, skies, foliage, and so forth. 
	//We will also modify this class to be data driven so that we can edit a single text file and the zone will populate itself accordingly without any code changes.	m_Zone = new ZoneClass;
	m_Zone = new ZoneClass;
	if (!m_Zone)
		return false;

	result = m_Zone->Initialize(m_Direct3D, hwnd, screenWidth, screenHeight, SCREEN_DEPTH);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the zone object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_Zone)
	{
		m_Zone->Shutdown();
		delete m_Zone;
		m_Zone = 0;
	}

	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}

//The Frame function does the per frame loop processing for the entire application. 
//All of the major objects frame processing must be called here.
bool ApplicationClass::Frame()
{
	bool result;

	m_Fps->Frame();
	m_Timer->Frame();

	result = m_Input->Frame();
	if (!result)
		return false;

	if (m_Input->IsEscapePressed() == true)
		return false;

	result = m_Zone->Frame(m_Direct3D, m_Input, m_ShaderManager, m_Timer->GetTime() / 100, m_Fps->GetFps());
	if (!result)
		return false;

	return result;
}