#pragma once

#if (!defined(__FEATHER_CORE_WINDOW__))

#define GLFW_INCLUDE_NONE
#define __FEATHER_CORE_WINDOW__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#define NL    "\n"
#define DEF_WIDTH    1280
#define DEF_HEIGHT   720
#define DEF_X        80
#define DEF_Y        80

#define SCR_NORMAL      (0)
#define SCR_FULL        (1)
#define SCR_CHANGING    (2)

typedef void(*fpPtr)(void*);
typedef void(*fpVoid)();
typedef void(*fpWindowOp)(GLFWwindow*);
typedef void(*fpWindowResize)(GLFWwindow*, int w, int h);

namespace feather
{
	namespace core
	{
		typedef struct st_OsInfo {
			bool isMacOSX;
		} OsInfo;

		typedef struct st_CoreControl {
			fpWindowOp onWindowClose;
			fpWindowResize onResizeWindow;
		} CoreControl;

		typedef struct st_RenderContext {
			fpVoid init;
			fpVoid render;
			fpPtr processInput;
			int bPause;
		} RenderContext;

		typedef struct st_CoreWindow {

			OsInfo os;
			CoreControl coreControl;
			RenderContext ctx;

			GLFWwindow* pWnd;
			int x;
			int y;
			int width;
			int height;
			int isFullScr;

			void onCoreInfo(const char* msg);
			void onCoreError(const char* msg);
			int runApp(void);
			int runMessageLoop(void);

			void toggFullScreen(GLFWwindow* window);
			void switchFullScreen(GLFWwindow* window);
			void switchNormalScreen(GLFWwindow* window);

		} CoreWindow;

	}
}

#endif



#ifndef __FEATHER_CORE_NO_EXTERN__

extern feather::core::CoreWindow g_wnd;

#endif
