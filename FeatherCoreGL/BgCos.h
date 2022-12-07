#pragma once

#include "CoreWindow.h"
#include <glm/glm.hpp>

using namespace glm;


typedef struct st_BgCos {

	float phi;
	vec3 color;

	static float frame2Phi(int nCurFrame, int nMaxFrame, float pi_x2)
	{
		return (((float)nCurFrame) / nMaxFrame) * pi_x2;
	}

	static vec3 fromRgb(float r, float g, float b)
	{
		vec3 color(r, g, b);
		return color;
	}

	void render()
	{
		float w = 0.5f * (1.0f + std::cos(phi));

		glClearColor(w * color.r, w * color.g, w * color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

} BgCos;
