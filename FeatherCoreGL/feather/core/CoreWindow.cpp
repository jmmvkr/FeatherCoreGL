#include "CoreWindow.h"
#include <iostream>

const char* boolStr(BOOL v)
{
	if (v) { return "true"; }
	return "false";
}

void feather::core::st_CoreWindow::onCoreInfo(const char* msg)
{
	std::cout << "[info] " << msg << NL;
}

void feather::core::st_CoreWindow::onCoreError(const char* msg)
{
	std::cout << "[error] " << msg << NL;
}

int feather::core::st_CoreWindow::runApp(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (os.isMacOSX)
	{
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}

	GLFWwindow* window = glfwCreateWindow(width = DEF_WIDTH, height = DEF_HEIGHT, "FeatherCoreGL", NULL, NULL);
	if (NULL == window)
	{

		onCoreError("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, coreControl.onResizeWindow);
	glfwSetWindowCloseCallback(window, coreControl.onWindowClose);
	glfwSetWindowPos(window, x = DEF_X, y = DEF_Y);

	pWnd = window;
	int nRetCode = runMessageLoop();
	return nRetCode;
}


int feather::core::st_CoreWindow::runMessageLoop(void)
{
	GLFWwindow* window = pWnd;

	onCoreInfo(" # messageLoop() - begin");
	ctx.init();

	auto ptr = glViewport;
	if (!ptr)
	{
		onCoreError("glViewport not set, probably missing call to gladLoadGL() in init()");
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		ctx.processInput(window);
		ctx.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	onCoreInfo(" # messageLoop() - end");
	return 0;
}

void feather::core::st_CoreWindow::toggFullScreen(GLFWwindow* window)
{
	if (!isFullScr)
	{
		switchFullScreen(window);
	}
	else
	{
		switchNormalScreen(window);
	}
}

void feather::core::st_CoreWindow::switchFullScreen(GLFWwindow* window)
{
	GLFWmonitor* scr = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(scr);

	isFullScr = SCR_CHANGING;
	{
		glfwGetWindowPos(window, &x, &y);
		glfwGetWindowSize(window, &width, &height);
		glfwSetWindowMonitor(window, scr, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	isFullScr = SCR_FULL;
}

void feather::core::st_CoreWindow::switchNormalScreen(GLFWwindow* window)
{
	isFullScr = SCR_CHANGING;
	{
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		{
			x = DEF_X;
			y = DEF_Y;
			width = DEF_WIDTH;
			height = DEF_HEIGHT;
		}
		glfwSetWindowMonitor(window, nullptr, x, y, width, height, 0);
	}
	isFullScr = SCR_NORMAL;
}
