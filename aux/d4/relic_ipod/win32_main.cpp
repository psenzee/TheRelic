// Includes

#define NOMINMAX 1
#include "net/Sockets.h"
#include <windows.h>
#include <gl/gl.h>

#include "core/core.h"
#include "core/global.h"
#include "core/Profiler.h"
#include "time/GameClock.h"
#include "time/FrameRateTracker.h"
#include "game/GameState.h"
#include "input/GameInput.h"
#include "render/OverheadCamera.h"
#include "time/Timer.h"
#include "platform/GLUtils.h"
#include "game/Character.h"
#include "render/GLStates.h"

#include "platform/Win32/input/xbox360controller.h"
#include "platform/Win32/input/dxkeyboard.h"
#include "platform/Win32/input/dxinput.h"

#include "luautil/LuaCall.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

core::Size frameSize(480, 320);

Fader             accelerometerFader(0.0f), accStartTimer(0.0f);

FrameRateTracker  renderfps("render", 400), updatefps("update", 400), totalfps("game", 40), totalx("total", 400);

float accel[3] = { 0.f, 0.f, 0.f };
bool  buttons[32] = { false };

float screenScaleX = 1.0f,//480.0f / (480 - 30),
      screenScaleY = 1.0f;//320.0f / (320 -  8);

extern Vector2 _globalTouchPoint;

Xbox360Controller controller;

void SetScreenTouch(int x, int y)
{
    core::Point p(static_cast<int>(x * .65f), static_cast<int>(y * 1.5f));
    _globalTouchPoint = Vector2(float(p.x), float(p.y));
    if (state)
        state->input->ScreenTouch(core::Point(p.x, p.y));
}

void Move(const Vector2 &move)
{
	state->input->move.SetVector(move);
//	state->camera->SetTilt(state->input->move.GetVector() * _g_tiltFactor);
//	state->camera->SetTilt(Vector2(0.f/*200.0f*/, 400.0f));
}

void Update(GameState *state)
{
    // I think this fixes the occasional stuck accelerometer issue
    // at least - I haven't seen it since
    //[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];	
    //[[UIAccelerometer sharedAccelerometer] setDelegate:self]; // because something goes wrong here..	

	state->camera->SetTilt(Vector2(0.f/*200.0f*/, 400.0f));

    DXKeyboard *keyboard = static_cast<DXKeyboard *>(DXInput::instance()->device(DXInput::KEYBOARD));
	keyboard->poll();

	if (controller.active())
        controller.poll();
    if (controller.active())
    {
        float rx = 0.f, ry = 0.f, lt = 0.f, rt = 0.f;
        controller.get(accel[1], accel[0], rx, ry, lt, rt);
        accel[1] *= -0.7f; accel[0] *= -0.7f;
        for (int i = 0; i < 32; i++)
            buttons[i] = controller.isButtonDown(i);
    }
	else
	{
		for (int i = 0; i < 100; i++)
		{
			if (keyboard->isPolledKeyDown(i))
				printf("k%d \n", i);
		}
		buttons[0] = keyboard->isPolledKeyDown(28/*VK_RETURN*/);    // hit
		buttons[1] = false;//keyboard->isPolledKeyDown(VK_A); // heal
		buttons[2] = false;//keyboard->isPolledKeyDown(VK_B); // hit circle
		buttons[3] = false;//keyboard->isPolledKeyDown(VK_C); // lighting change
	}

	if (!state)
		return;

	static int accState = 0;

	if (accState == 0)
        printf("accState = 0\n");	
	
	if (accState == 0)
	{
		accStartTimer.Fade(1.0f, 5000);
		accState = 1;
        printf("accState = 1\n");		
	}
	
    GLfloat length = sqrtf(accel[0] * accel[0] + accel[1] * accel[1]/* + accel[2] * accel[2]*/);	
	if (accState == 1 && accStartTimer.GetValue() >= 1.0f)
	{
		accelerometerFader.Fade(1.0f, 5000);
		accState = 2;		
        printf("accState = 2\n");
	}
	else if (accState >= 2/* && length >= 0.1f*/)
	{
		float factor = accelerometerFader.GetValue() * 200.0f;
		Vector2 move = Vector2(accel[1] * factor, accel[0] * factor);
		Move(move);
	}

	updatefps.StartFrame();
	state->Update(GameTime(updatefps.TotalLastFrameTime()));
	updatefps.EndFrame();
}

void Render(GameState *state)
{
    if (!state)
        return;

    renderfps.StartFrame();
	//glClearColor(0.1f, 0.1f, 0.5f, 0.0f); 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    state->Draw(GameTime(renderfps.TotalLastFrameTime()));
    renderfps.EndFrame();	
}

void Setup()
{
    UpdateAppTime();

    glViewport(0, 0, frameSize.width, frameSize.height);
    
    GLStates::lighting.Set(false);    
	
    //InitializeAudio();
	
    state = new GameState(frameSize);	

    state->Start();

	controller.poll();
	DXInput::instance()->device(DXInput::KEYBOARD)->initialize();
}

HDC _g_HDC = 0;

void UpdateAndRender(GameState *state)
{
	profiler->StartFrame("frame");

	UpdateAppTime();

	totalx.EndFrame();
	totalx.StartFrame();
	totalfps.StartFrame();

    Update(state);
    Render(state);

	unsigned totaltime = totalfps.TotalLastFrameTime();
	totalfps.EndFrame();
	
	profiler->SetTime("total",     totaltime);
    if (totaltime <= 0)
        profiler->SetTime("total_fps", 1000000000);	
    else
	    profiler->SetTime("total_fps", 1000 / totaltime);	
	profiler->EndFrame();

    SwapBuffers(_g_HDC);
}

// WinMain

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL quit = FALSE;
    float theta = 0.0f;
    
    // register window class
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass( &wc );
    
    core::Size frameEdges(8, 28);

    // create main window
    hWnd = CreateWindow( 
        "GLSample", "Viewer", 
        WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
        -3, 0, frameSize.width + frameEdges.width, frameSize.height + frameEdges.height,
        NULL, NULL, hInstance, NULL );
    
    // enable OpenGL for the window
    EnableOpenGL( hWnd, &hDC, &hRC );
    
    _g_HDC = hDC;

    Setup();
    SetTimer(hWnd, 1, 16, 0);

    // program main loop
    while ( !quit )
    {
        // check for messages
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
        {
            
            // handle or dispatch messages
            if ( msg.message == WM_QUIT ) 
            {
                quit = TRUE;
            } 
            else 
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            
        } 
    }
    
    // shutdown OpenGL
    DisableOpenGL( hWnd, hDC, hRC );
    
    // destroy the window explicitly
    DestroyWindow( hWnd );

    KillTimer(hWnd, 1);
    
    return static_cast<int>(msg.wParam);
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    const float SPEED     = 0.1f;
    const float MAX_SPEED = 0.5f;    

    switch (message)
    {
        
    case WM_CREATE:
        return 0;

    case WM_TIMER:
        UpdateAndRender(state);
        return 0;
        
    case WM_CLOSE:
        PostQuitMessage( 0 );
        return 0;
        
    case WM_DESTROY:
        return 0;

    case WM_LBUTTONDOWN:
        printf("Mouse click at %d, %d\n", LOWORD(lParam), HIWORD(lParam));
        SetScreenTouch(LOWORD(lParam), HIWORD(lParam));
        return 0;
        
    case WM_KEYDOWN:
        switch ( wParam )
        {
        case VK_SPACE:
            accel[0] = 0.f;
            accel[1] = 0.f;
            state->GetPlayer()->SetVelocity(Vector3(0.f, 0.f, 0.f));
            return 0;
        case VK_NEXT:
            if (state && state->camera)
                state->camera->SetDepthScale(state->camera->GetDepthScale() + 0.1f);
            return 0;
        case VK_PRIOR:
            if (state && state->camera)
                state->camera->SetDepthScale(state->camera->GetDepthScale() - 0.1f);
            return 0;
        case VK_UP:
            //printf("up\n");
//            if (accel[0] < 0.f)
//                accel[0] = 0.f;
            accel[0] += SPEED;
            if (accel[0] >= MAX_SPEED)
                accel[0] = MAX_SPEED;
            return 0;
        case VK_DOWN:
            //printf("down\n");
//            if (accel[0] > 0.f)
//                accel[0] = 0.f;
            accel[0] -= SPEED;
            if (accel[0] <= -MAX_SPEED)
                accel[0] = -MAX_SPEED;
            return 0;
        case VK_LEFT:
            //printf("left\n");
//            if (accel[1] < 0.f)
//                accel[1] = 0.f;
            accel[1] += SPEED;
            if (accel[1] >= MAX_SPEED)
                accel[1] = MAX_SPEED;
            return 0;
        case VK_RIGHT:
            //printf("right\n");
//            if (accel[1] > 0.f)
//                accel[1] = 0.f;
            accel[1] -= SPEED;
            if (accel[1] <= -MAX_SPEED)
                accel[1] = -MAX_SPEED;
            return 0;

            
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
            
        }
        return 0;
    
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
            
    }
    
}

// Enable OpenGL

void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int format;
    
    // get the device context (DC)
    *hDC = GetDC( hWnd );
    
    // set the pixel format for the DC
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    format = ChoosePixelFormat( *hDC, &pfd );
    SetPixelFormat( *hDC, format, &pfd );
    
    // create and enable the render context (RC)
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );
    
}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}

const char *tags[] =
{ 
  "update", "game", "total", "total_fps",
  "balls", "level_draw", "dl_render",
  "messages_hud", "fx_explosion_narration",
  "ui_draw", "gs_draw", 0
};

const char *_g_NetAddress    = 0;
int         _g_NetLocalPort  = 1500;
int         _g_NetRemotePort = 1501;

const char *_g_MainLuaFile = "game.lua";

void main(int argc, const char **argv)
{
    if (argc > 1)
    {
        _g_MainLuaFile = argv[1];
        /*
        _g_NetAddress  = argv[2];
        int port = _g_NetLocalPort;
        if (argc > 2)
            port = atoi(argv[3]);
        /*
        if (!port)
            port = _g_NetLocalPort;
        _g_NetLocalPort = port;
        port = _g_NetRemotePort;
        if (argc > 3)
            port = atoi(argv[3]);
        if (!port)
           port = _g_NetRemotePort;
        _g_NetRemotePort = port;
        */
    }
    InitializeSockets();
    profiler = new Profiler(globalTranslateReadWritePath("profile.csv"), tags, 40);
    WinMain(::GetModuleHandle(0), 0, ::GetCommandLine(), 1);
    ShutdownSockets();
}