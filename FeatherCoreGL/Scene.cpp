#include "Scene.h"

using feather::core::GlProgram;
using feather::core::Shader;
using feather::core::Mesh;
using feather::core::GpuMesh;
using feather::core::GpuUtil;


void st_RenderAnchor::add(vec3 pos)
{
	RenderList* ptr = (RenderList*)ptrToRenderList;
	ptr->addItem(resourceIndex, pos);
}

void Scene::init(void)
{
	p = Shader::createProgram(L"_shader/demo.vs", L"_shader/demo.fs");
	std::cout << " - shader linked? " << boolStr(p.bLinked) << NL;

	//mesh = Mesh::load("_media/mesh/ball.vx");
	mesh = Mesh::load("_media/mesh/ball-90.vx");
	std::cout << " - mesh loaded? " << boolStr(mesh.bLoaded) << NL;

	auto g = GpuUtil::loadMesh(mesh);
	g.tex[0] = GpuUtil::loadTexture(0, "_media/tex/ball.png");
	g.tex[1] = g.tex[0];

	auto a = actors.addAndFindResource("ball", g);
	a.add(glm::vec3(0.0f, 0.0f, 0.0f));
	a.add(glm::vec3(3.0f, 0.0f, 0.0f));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	initUniforms(p.prog, addr);
	switchWireMode(bWireMode, p);
}

void Scene::initUniforms(GLint prog, ShaderUniform& addr)
{
	addr.pv = glGetUniformLocation(prog, "pv");
	addr.model = glGetUniformLocation(prog, "model");
	addr.bWireMode = glGetUniformLocation(prog, "bWireMode");
	addr.opt = glGetUniformLocation(prog, "opt");
	addr.viewDirection = glGetUniformLocation(prog, "viewDirection");
}

static inline float vwrap(float v, float vMin, float vMax)
{
	if (v < vMin)
	{
		return (v + vMax);
	}
	if (v >= vMax)
	{
		return (v - vMax);
	}
	return v;
}

void Scene::update(void)
{
	BOOL bShift = getKey(GLFW_KEY_LEFT_SHIFT) || getKey(GLFW_KEY_RIGHT_SHIFT);

	if (testKey(GLFW_KEY_Z))
	{
		if (bShift)
		{
			opt = (1 + opt) % 3;
			glUniform1i(addr.opt, opt);
			bWireMode = FALSE;
		}
		else
		{
			bWireMode = !bWireMode;
		}
		switchWireMode(bWireMode, p);
	}

	if (pContext->bPause){
		return;
	}
	rollYaw = vwrap(rollYaw + 1.5f, 0.0f, 360.0f);
}

void Scene::render(void)
{
	glm::mat4 proj = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	proj = glm::perspective(glm::radians<float>(45.0f), aspectRatio, 0.02f, 100.0f);
	posEye = 6.0f;
	calcView(&view, proj);

	glUseProgram(p.prog);
	glMatrixMode(GL_PROJECTION);

	auto viewDirection = proj * view * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	glUniform3fv(addr.viewDirection, 1, PTR_V(viewDirection));

	auto lst = actors.itemList;
	for (int i = 0, len = lst.size(); i < len; i++)
	{
		auto item = lst[i];
		auto g = item.mesh;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g.tex[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g.tex[1]);

		model = glm::translate(glm::mat4(1.0f), item.position);
		glUniformMatrix4fv(addr.model, 1, GL_FALSE, PTR_M(model));

		glBindVertexArray(g.vao);
		glDrawElements(GL_TRIANGLES, g.lenIndexBuffer, GL_UNSIGNED_SHORT, 0);
	}
}

void Scene::onWindowResized(int w, int h)
{
	aspectRatio = ((float)w / (float)h);
}

void applyRotation(glm::mat4& mat, float yaw, float pitch)
{
	mat = glm::rotate(mat, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
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
	applyRotation(view, rollYaw, 0.0f);

	pv = projection * view;
	glUniformMatrix4fv(addr.pv, 1, GL_FALSE, PTR_M(pv));
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

	glUniform1i(addr.bWireMode, bWireMode);
}
