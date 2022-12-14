#pragma once

#include "feather/core/Shader.h"
#include "feather/core/Mesh.h"
#include "feather/core/GpuMesh.h"
#include "feather/core/RenderList.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace feather::core;


class Scene
{
private:
	GlProgram p;
	Mesh mesh;
	glm::vec4 tr;
	float posEye;
	float aspectRatio;
	int opt;
	BOOL bWireMode;
	BOOL bRotate;
	float rollYaw;
	RenderList actors;
	std::map<float, int> sorted;

public:
	fpKeyApi getKey;
	fpKeyApi testKey;
	RenderContext* pContext;

public:
	void init(void);
	void update(void);
	void render(void);
	void onWindowResized(int w, int h);

// Uniforms
private:
	typedef struct st_ShaderUniform {
		GLuint pv;
		GLuint model;
		GLuint bWireMode;
		GLuint opt;
		GLuint viewDirection;
		GLuint texScale;
	} ShaderUniform;
	ShaderUniform addr;
	static void initUniforms(GLint prog, ShaderUniform& addr);

private:
	glm::mat4 calcView(glm::mat4* pView, glm::mat4 projection);
	void switchWireMode(BOOL bWireMode, GlProgram& p);
	void renderGpuMesh(RenderItem& item, ShaderUniform& addr);
};
