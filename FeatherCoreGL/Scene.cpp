#include "Scene.h"

using feather::core::GlProgram;
using feather::core::Shader;
using feather::core::Mesh;
using feather::core::GpuMesh;
using feather::core::GpuUtil;


void Scene::init(void)
{
	p = Shader::createProgram(L"_shader/demo.vs", L"_shader/demo.fs");
	std::cout << " - shader linked? " << boolStr(p.bLinked) << NL;

	//mesh = Mesh::load("_media/mesh/ball.vx");
	mesh = Mesh::load("_media/mesh/ball-90.vx");
	std::cout << " - mesh loaded? " << boolStr(mesh.bLoaded) << NL;

	gm = GpuUtil::loadMesh(mesh);

	gm.tex[0] = GpuUtil::loadTexture(0, "_media/tex/ball.png");
	gm.tex[1] = gm.tex[0];

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switchWireMode(bWireMode, p);
}

void Scene::update(void)
{
	BOOL bShift = getKey(GLFW_KEY_LEFT_SHIFT) || getKey(GLFW_KEY_RIGHT_SHIFT);

	if (testKey(GLFW_KEY_Z))
	{
		if (bShift)
		{
			opt = (1 + opt) % 2;
			glUniform1i(glGetUniformLocation(p.prog, "opt"), opt);
			bWireMode = FALSE;
		}
		else
		{
			bWireMode = !bWireMode;
		}
		switchWireMode(bWireMode, p);
	}
}

void Scene::render(void)
{
	glm::mat4 proj = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	proj = glm::perspective(glm::radians<float>(45.0f), aspectRatio, 0.02f, 100.0f);
	posEye = 6.0f;
	calcView(&view, proj);

	glUseProgram(p.prog);
	glMatrixMode(GL_PROJECTION);

	glUniformMatrix4fv(glGetUniformLocation(p.prog, "model"), 1, GL_FALSE, PTR_M(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gm.tex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gm.tex[1]);

	glBindVertexArray(gm.vao);
	glDrawElements(GL_TRIANGLES, gm.lenIndexBuffer, GL_UNSIGNED_SHORT, 0);

}

void Scene::onWindowResized(int w, int h)
{
	aspectRatio = ((float)w / (float)h);
}

void Scene::calcView(glm::mat4* pView, glm::mat4 projection)
{
	glm::mat4 view(1.0f);
	glm::mat4 pv;
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	float fx = tr.x;
	float fy = tr.y;
	float fz = tr.z;
	view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, posEye),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	pv = projection * view;
	glUniformMatrix4fv(glGetUniformLocation(p.prog, "pv"), 1, GL_FALSE, PTR_M(pv));
	if (pView)
	{
		(*pView) = view;
	}
}

void Scene::switchWireMode(BOOL bWireMode, GlProgram& p)
{
	if (bWireMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	glUniform1i(glGetUniformLocation(p.prog, "bWireMode"), bWireMode);
}
