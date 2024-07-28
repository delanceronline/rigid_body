#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <math.h>
#include <process.h>
#include <time.h>
#include "ShootingScene.h"

ShootingScene ss;
static HWND	hMainWindow=NULL;		// Holds Our Window Handle
static HINSTANCE	hInstance;		// Holds The Instance Of The Application
static HGLRC hRC = NULL;		// Permenant Rendering context
static HDC hDC = NULL;			// Private GDI Device context

static LPCTSTR lpszAppName = "Shooting";
int width = 800;
int height = 600;
int bits = 32;

// Transformation matrix to project shadow


bool	keys[256];			// Arrball_a.y Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

CLoad3DS g_Load3ds;			// This is 3DS class.  This should go in a good model class.
t3DModel g_3DModel;			// This holds the 3D Model info that we load in

//BOOL flighting = TRUE;

#define DEGTORAD(x) ((x) * 0.017453292519943296)
#define RADTODEG(x) ((x) * 57.29577951308232088)

CRITICAL_SECTION	csThreadSafe;	// Critical section 
RENDER_STATE		rsRenderData;	// Communication between threads

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hMainWindow,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hMainWindow && !DestroyWindow(hMainWindow))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hMainWindow=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("Shooting",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
	

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	static HANDLE hThreadHandle = NULL;
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_CREATE:
		// Store the device context
		hDC = GetDC(hWnd);		

		// Initialize the critical section
		InitializeCriticalSection(&csThreadSafe);

		// Initialize interthread communication
		rsRenderData.uiFrames = 0;
		rsRenderData.bResize = FALSE;
		rsRenderData.bTerminate = FALSE;
		rsRenderData.bFatalError = FALSE;
		rsRenderData.bModifyFlag = TRUE;

		// Create the rendering thread
		hThreadHandle = (HANDLE) _beginthread(RenderingThreadEntryPoint,0,NULL);

		break;	
	
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:
			EnterCriticalSection(&csThreadSafe);
			rsRenderData.bResize = TRUE;
			rsRenderData.bModifyFlag = TRUE;
			LeaveCriticalSection(&csThreadSafe);
			break;

		case WM_DESTROY:
			{
			// Tell the rendering loop to terminate itself
			EnterCriticalSection(&csThreadSafe);
			rsRenderData.bTerminate = TRUE;
			rsRenderData.bModifyFlag = TRUE;
			LeaveCriticalSection(&csThreadSafe);

			// Need to wait for rendering thread to terminate so it can clean
			// up. Wait for the thread handle to become signaled. Give it five
			// seconds, then terminate anywball_a.y if it hasn't yet (it's probably
			// stuck if it hasn't terminated by now)
			WaitForSingleObject(hThreadHandle, 5000);

			DeleteCriticalSection(&csThreadSafe);

			KillGLWindow();
			PostQuitMessage(0);
			}


			break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
	{
	MSG			msg;		// Windows message structure
	WNDCLASS	wc;			// Windows class structure
	HWND		hWnd;		// Storeage for window handle
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;	
	bool done = FALSE;
	// Register Window style
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance 		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	
	// No need for background brush for OpenGL window
	wc.hbrBackground	= NULL;		
	
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= lpszAppName;

	// Register the window class
	if(RegisterClass(&wc) == 0)
		return FALSE;

	if (MessageBox(NULL,"Fullscreen or not?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
							lpszAppName,							// Class Name
							lpszAppName,						// Window Title
							dwStyle |							// Defined Window Style
							WS_CLIPSIBLINGS |					// Required Window Style
							WS_CLIPCHILDREN,					// Required Window Style
							0, 0,								// Window Position
							WindowRect.right-WindowRect.left,	// Calculate Window Width
							WindowRect.bottom-WindowRect.top,	// Calculate Window Height
							NULL,								// No Parent Window
							NULL,								// No Menu
							hInstance,							// Instance
							NULL);								// Dont Pass Anything To WM_CREATE

	// If window was not created, quit
	if(hWnd == NULL)
		return FALSE;

	// Displball_a.y the window
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
	
	hMainWindow = hWnd;


	while(GetMessage(&msg, NULL, 0, 0) && !done)
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}

		}

	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	return msg.wParam;
}

void RenderingThreadEntryPoint(void *pVoid)
	{
	char cOutBuffer[32];
	float fps;
	// Setup the rendering context
	SetupRC();
	
	ss.Load3DSModel();

	QueryPerformanceCounter(&ss.globeTime);
	QueryPerformanceFrequency(&ss.timerFrequency);

	// Main loop for rendering
	while(1)
	{
		// Check rendering state info, terminate if flagged
		if(CheckStates())
			break;	
		
		if(GetAsyncKeyState('O'))
		{
			ss.linearvelocity_x = -5.0f;
		}
		if(GetAsyncKeyState('P'))
		{
			ss.linearvelocity_x = 5.0f;
		}

		if(GetAsyncKeyState('F'))
		{
			if (ss.holdon)
			{
				//5.0 is scale factor (5 units in space equals 1 meter)
				ss.ball_v.y = 4.5f * 5.0f;
				ss.ball_v.x = -4.5f * sin(DEGTORAD(ss.roty)) * 5.0f;
				ss.ball_v.z = -4.5f * cos(DEGTORAD(ss.roty)) * 5.0f;
								
				ss.angularballspeedx = 10.0f * powf(ss.ball_v.x * ss.ball_v.x + ss.ball_v.y * ss.ball_v.y + ss.ball_v.z * ss.ball_v.z, 0.5f);
				ss.holdon = false;
				ss.on_shooting = true;
			}
		}

		if(GetAsyncKeyState('J'))
		{
			ss.ball_v.y = -6.0f * 5.0f;
			ss.holdon = false;
			ss.on_shooting = true;
		}

		if(GetAsyncKeyState(VK_PRIOR))
		{
			ss.angularspeedx = 45.0f;
		}
		if(GetAsyncKeyState(VK_NEXT))
		{
			ss.angularspeedx = -45.0f;
		}

		if(GetAsyncKeyState(VK_UP))
		{
			ss.linearvelocity_z = 20.0f;
		}
		if(GetAsyncKeyState(VK_DOWN))
		{
			ss.linearvelocity_z = -20.0f;
		}
		if(GetAsyncKeyState(VK_RIGHT))
		{
			ss.angularspeedy = -45.0f;
			ss.angularballspeedy = -45.0f;
		}
		if(GetAsyncKeyState(VK_LEFT))
		{
			ss.angularspeedy = 45.0f;
			ss.angularballspeedy = 45.0f;
		}
		
		if(GetAsyncKeyState('N'))
		{
			ss.posx -= 0.5f * (float)(sin(DEGTORAD(ss.roty)));
			ss.posz -= 0.5f * (float)(cos(DEGTORAD(ss.roty)));
			ss.posy += 0.5f * (float)(sin(DEGTORAD(ss.rotx)));
		}
		if(GetAsyncKeyState('M'))
		{
			ss.posx += 0.5f * (float)(sin(DEGTORAD(ss.roty)));
			ss.posz += 0.5f * (float)(cos(DEGTORAD(ss.roty)));
			ss.posy -= 0.5f * (float)(sin(DEGTORAD(ss.rotx)));
		}
		
		if(GetAsyncKeyState('A'))
		{
			ss.linearvelocity_y = 10.0f;
		}
		if(GetAsyncKeyState('Z'))
		{
			ss.linearvelocity_y = -10.0f;
		}
		
		if(GetAsyncKeyState('R'))
		{
			ss.holdon = true;
		}
		
		if(keys['D'])
			ss.keydown = true;
		if(!keys['D'] && ss.keydown)
			ss.keypressed = true;
		if(ss.keypressed)
		{
			if(ss.demo_mode)
			{
				ss.demo_mode = false;
				ss.posx = 25.0f;
				ss.posy = 6.5f;
				ss.posz = -24.0f;
				ss.rotx = 0.0f;
				ss.roty = 180.0f;
			}
			else
				ss.demo_mode = true;
			ss.keydown = false;
			ss.keypressed = false;
		}
		
		ss.DrawScene();
		
		fps = 1.0f / ss.interval;
		sprintf(cOutBuffer, "%0.0f", fps);
		SetWindowText(hMainWindow, cOutBuffer);
		// Swap buffers
		SwapBuffers(hDC);
	}

	// Finally, shut down OpenGL Rendering context
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(hRC);

	_endthread();	// Terminate the thread nicely
	}

BOOL CheckStates()
	{
	BOOL bRet = FALSE;

	// Is it time to leave or anything else
	EnterCriticalSection(&csThreadSafe);

	if(	rsRenderData.bModifyFlag)
		{
		// Flag set to terminate
		if(rsRenderData.bTerminate)
			bRet = TRUE;

		// Window resized
		if(rsRenderData.bResize)
		{
			// Get the Window dimensions
			RECT rect;
			GetClientRect(hMainWindow, &rect);
			ss.ResetViewport(rect);
		}

		// Reset the modify flag and exit
		rsRenderData.bModifyFlag = FALSE;
		}

	// Increment frame count
	rsRenderData.uiFrames++;

	LeaveCriticalSection(&csThreadSafe);

	return bRet;
	}

void SetDCPixelFormat(HDC hDC)
	{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		bits,							// Want 32 bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		16,								// Size of depth buffer
		1,								// Not used to select mode
		0,								// Not used to select mode
		0,	            				// Not used to select mode
		0,								// Not used to select mode
		0,0,0 };						// Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
	}



void SetupRC(void)
{
	SetDCPixelFormat(hDC);

	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	ss.Initialize();
	ss.CD_Data();
}
