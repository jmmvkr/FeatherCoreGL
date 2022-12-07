// FeatherCoreGL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "CoreWindow.h"
#include "CoreInput.h"
#include "BgCos.h"


using feather::core::CoreWindow;
using feather::core::RenderContext;
using feather::core::KeyHold;


// custom background effect
#define IS_PAUSED    (g_wnd.ctx.bPause)
#define INC_REWIND(val, nMaxVal)  if((val) < (nMaxVal)) (val)++; else (val) = 1
typedef struct st_BgEffect
{
	int iCurFrame;
	int nMax;
	BgCos cosEffect;
} BgEffect;
static float PI_X2 = std::acosf(-1.0f) * 2.0f;


// globals
CoreWindow g_wnd = { 0 };
BgEffect g_bg;
KeyHold kPause = { 0, GLFW_KEY_SPACE };
KeyHold kFullScr = { 0, GLFW_KEY_F11 };

// logic related to render context
void init(void);
void render(void);
void processInput(void* window);

// logic related to window handling
void onWindowClose(GLFWwindow* pWnd);
void onResizeWindow(GLFWwindow* window, int w, int h);
int tryClose(void);


// main function
int main()
{
	RenderContext ctx = { init, render, processInput };
	
	std::cout << "\n ---- Hello FeatherCoreGL ---- \n\n";

	g_wnd.os.isMacOSX = false;
	g_wnd.coreControl.onResizeWindow = onResizeWindow;
	g_wnd.coreControl.onWindowClose = onWindowClose;
	g_wnd.ctx = ctx;

	return g_wnd.runApp();
}

void init(void)
{
	gladLoadGL();
	g_bg.nMax = 60;
	g_bg.cosEffect.color = BgCos::fromRgb(0.0f, 0.6f, 0.95f);
}

void render(void)
{
	BgEffect& bg = g_bg;
	BgCos cos = bg.cosEffect;
	cos.phi = BgCos::frame2Phi(bg.iCurFrame, bg.nMax, PI_X2);
	cos.render();

	if (!IS_PAUSED)
	{
		INC_REWIND(bg.iCurFrame, bg.nMax);
	}
}


void onWindowClose(GLFWwindow* window)
{
	if (tryClose())
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else
	{
		glfwSetWindowShouldClose(window, GLFW_FALSE);
	}
}

void onResizeWindow(GLFWwindow* window, int w, int h)
{
	if ((w > 0) && (h > 0))
	{
		glViewport(0, 0, w, h);
	}
}

void processInput(void* window)
{
	int code = 0;
	auto pWnd = g_wnd.pWnd;
	if (pWnd != window) return;

	// close window
	if (GLFW_PRESS == glfwGetKey(pWnd, GLFW_KEY_ESCAPE))
	{
		onWindowClose(pWnd);
	}

	// full screen	
	if (KeyHold::testInput(pWnd, kFullScr))
	{
		g_wnd.toggFullScreen(pWnd);
	}

	// pause
	if (KeyHold::testInput(pWnd, kPause))
	{
		IS_PAUSED = (!IS_PAUSED);
	}
}


// [platform specific] - begin

int wcharMain(int argc, wchar_t** argv)
{
	return main();
}

#ifdef _WIN64
#include <windows.h>
int confirm(const wchar_t* msg, const wchar_t* title)
{
	return MessageBoxW(0, msg, title, 0x4);
}

int tryClose()
{
#ifdef _DEBUG
	return TRUE;
#else
	return (0x06 == confirm(L"Quit sample program ?", L"Confirm"));
#endif // !_DEBUG
}

int CALLBACK wWinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPWSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	int argc = 0;
	auto argv = CommandLineToArgvW(lpCmdLine, &argc);
	return wcharMain(argc, argv);
}

#else
int tryClose()
{
	return GLFW_TRUE;
}
#endif // _WIN64

// [platform specific] - end
