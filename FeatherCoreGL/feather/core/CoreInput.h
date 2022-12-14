#pragma once

#include <GLFW/glfw3.h>


namespace feather
{
	namespace core
	{
		/**
		   A tool to check holding status of key on keyboard, in order to
		   detect key-down event correctly.
		  
		   [Usage 001]
           KeyHold arrKeyHold[1 + GLFW_KEY_LAST] = { 0 };
		   
		   int apiTestKey(int keyCode)
		   {
		       // optinal to call initKeyArray()
		       return arrKeyHold[keyCode].test(glfwGetKey(g_wnd.pWnd, keyCode));
		   }

		   [Usage 002]
		   KeyHold arrKeyHold[1 + GLFW_KEY_LAST] = { 0 };
		   
		   int apiTestKey(int keyCode)
		   {
		       // required to call initKeyArray()
		       return KeyHold::testInput(g_wnd.pWnd, arrKeyHold[keyCode]);
		   }
		   
		   void init(void)
		   {
		       // ...
		       KeyHold::initKeyArray(arrKeyHold); // initialize nKey in arrKeyHold
		       scene.getKey = apiGetKey;
		       scene.testKey = apiTestKey;
		       // ...
		   }
		 
		 */
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

			static inline void initKeyArray(struct st_KeyHold* pArr)
			{
				int i;
				for (i = 0; i <= GLFW_KEY_LAST; i++)
				{
					pArr[i].nKey = i;
				}
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

