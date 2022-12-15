#pragma once

#include "feather/core/Shader.h"
#include "feather/core/Mesh.h"
#include "feather/core/GpuMesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace feather::core;


class Scene
{
private:
	GlProgram p;
	Mesh mesh;
	GpuMesh gm;
	glm::vec4 tr;
	float posEye;
	float aspectRatio;
	int opt;
	BOOL bWireMode;

public:
	fpKeyApi getKey;
	fpKeyApi testKey;
	RenderContext* pContext;

public:
	void init(void);
	void update(void);
	void render(void);
	void onWindowResized(int w, int h);

private:
	void calcView(glm::mat4* pView, glm::mat4 projection);
	static void switchWireMode(BOOL bWireMode, GlProgram& p);
};
