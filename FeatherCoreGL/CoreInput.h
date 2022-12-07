#pragma once

#include <GLFW/glfw3.h>


namespace feather
{
	namespace core
	{

		typedef struct st_KeyHold {

			int nLastCode;
			int nKey;

			int test(int code)
			{
				int testCond = 0;
				int bCodeChange = (code != nLastCode);
				if (GLFW_PRESS == code)
				{
					if (bCodeChange)
					{
						testCond = 1;
					}
				}
				if (bCodeChange)
				{
					nLastCode = code;
				}
				return testCond;
			}

			static inline int testInput(GLFWwindow* pWnd, struct st_KeyHold& kh)
			{
				int code;
				int testCond = 0;

				code = glfwGetKey(pWnd, kh.nKey);
				if (GLFW_PRESS == code)
				{
					if (code != kh.nLastCode)
					{
						testCond = 1;
					}
				}

				kh.nLastCode = code;
				return testCond;
			}

		} KeyHold;
	}
}

