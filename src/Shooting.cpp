#include "Shooting.h"

ShootingScene ss;

static HWND	hMainWindow=NULL;		// Holds Our Window Handle
static HINSTANCE	hInstance;		// Holds The Instance Of The Application
static HGLRC hRC = NULL;		// Permenant Rendering context
static HDC hDC = NULL;			// Private GDI Device context

static LPCTSTR lpszAppName = "OBB Collision";
int width = 800;
int height = 600;
int bits = 32;
char cOutBuffer[64];
float fps;

bool	keys[256];			// Arrball_a.y Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	IsKeyDown = FALSE;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

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
			IsKeyDown = TRUE;
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			IsKeyDown = FALSE;
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:
			break;

		case WM_DESTROY:
			{
				// Finally, shut down OpenGL Rendering context
				wglMakeCurrent(hDC, NULL);
				wglDeleteContext(hRC);

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
	bool sel_up = FALSE, sel_down = FALSE, sel_left = FALSE, sel_right = FALSE;
	bool up_moved = FALSE, down_moved = FALSE, left_moved = FALSE, right_moved = FALSE;
	bool pressing_PU = FALSE, pressing_PD = FALSE;
	bool start_game = FALSE;

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

	fullscreen=FALSE;

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



	// Display the window
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
	
	hMainWindow = hWnd;

	// Setup the rendering context
	SetupRC();
	ss.ChangeSize(width, height);

	QueryPerformanceCounter(&ss.globeTime);
	QueryPerformanceFrequency(&ss.timerFrequency);

	srand((unsigned)time(NULL));

	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
				break;
		}
		
		if(keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			break;							// ESC or DrawGLScene Signalled A Quit

		if(keys[VK_SHIFT])
			pressing_PU = TRUE;
		else
			pressing_PU = FALSE;

		if(keys[VK_NEXT])
			pressing_PD = TRUE;
		else
			pressing_PD = FALSE;

		if(GetAsyncKeyState(VK_LEFT))
			ss.roty += 1.0f;

		if(GetAsyncKeyState(VK_RIGHT))
			ss.roty -= 1.0f;
		
		if(keys[VK_SPACE])
		{
			ss.dp1.lin_mom.x += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp1.lin_mom.y += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp1.lin_mom.z += (float)(rand() - (RAND_MAX / 2)) * 0.05f;

			ss.dp2.lin_mom.x += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp2.lin_mom.y += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp2.lin_mom.z += (float)(rand() - (RAND_MAX / 2)) * 0.05f;

			ss.dp3.lin_mom.x += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp3.lin_mom.y += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp3.lin_mom.z += (float)(rand() - (RAND_MAX / 2)) * 0.05f;

			ss.dp4.lin_mom.x += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp4.lin_mom.y += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
			ss.dp4.lin_mom.z += (float)(rand() - (RAND_MAX / 2)) * 0.05f;
		}

		if(hRC)
		{
			ss.DrawScene();
			SwapBuffers(hDC);

		}//End hRC.
	}//End while.

	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	return msg.wParam;
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
	ss.LoadModel();
	ss.GenerateDisplayList();
}

GLuint LoadGLTexture( const char *filename )			// Load Bitmaps And Convert To Textures
{
	AUX_RGBImageRec *pImage;							// Create Storage Space For The Texture
	GLuint texture = 0;

	pImage = auxDIBImageLoad(filename);

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ( pImage != NULL && pImage->data != NULL )		// If Texture Image Exists
	{
		glGenTextures(1, &texture);						// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, pImage->sizeX, pImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		free(pImage->data);								// Free The Texture Image Memory
		free(pImage);									// Free The Image Structure
	}

	return texture;										// Return The Status
}

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