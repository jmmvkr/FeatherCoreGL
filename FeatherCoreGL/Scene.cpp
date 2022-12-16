#include "Scene.h"

using feather::core::GlProgram;
using feather::core::Shader;
using feather::core::Mesh;
using feather::core::GpuMesh;
using feather::core::GpuUtil;

#define ALPHA_OBJECT    (1)


void st_RenderAnchor::add(vec3 pos, float scale, float texScale)
{
	RenderList* ptr = (RenderList*)ptrToRenderList;
	ptr->addItem(resourceIndex, pos, scale, texScale);
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
	g.flgBlendFunc = ALPHA_OBJECT;

	auto a = actors.addAndFindResource("ball", g);
	a.add(glm::vec3(0.0f, 0.0f, 0.0f));
	a.add(glm::vec3(3.0f, 0.0f, 0.0f));

	mesh = Mesh::load("_media/mesh/plane.vx");
	auto pg = GpuUtil::loadMesh(mesh);
	pg.tex[0] = GpuUtil::loadTexture(0, "_media/tex/tiles.png");
	pg.tex[1] = pg.tex[0];
	auto plane = actors.addAndFindResource("plane", pg);
	plane.add(glm::vec3(0.0f, -1.5f, 0.0f), 4.0f, 4.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	bRotate = TRUE;
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
	addr.texScale = glGetUniformLocation(prog, "texScale");
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
	if (testKey(GLFW_KEY_R))
	{
		bRotate = !bRotate;
	}

	if (pContext->bPause){
		return;
	}
	if (bRotate)
	{
		rollYaw = vwrap(rollYaw + 1.5f, 0.0f, 360.0f);
	}
}

void Scene::render(void)
{
	glm::mat4 proj = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	proj = glm::perspective(glm::radians<float>(45.0f), aspectRatio, 0.02f, 100.0f);
	posEye = 6.0f;
	auto pvMatrix = calcView(&view, proj);

	glUseProgram(p.prog);
	glMatrixMode(GL_PROJECTION);

	auto viewDirection = (pvMatrix) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	glUniform3fv(addr.viewDirection, 1, PTR_V(viewDirection));


	// clean up resource for 2nd pass
	sorted.clear();
	auto eye4 = (pvMatrix) * glm::vec4(0.0f, 0.0f, posEye, 1.0f);
	vec3 eye(eye4.x, eye4.y, eye4.z);

	// 1st pass, opaque objects
	for (int i = 0, len = actors.itemList.size(); i < len; i++)
	{
		auto item = actors.itemList[i];
		if (ALPHA_OBJECT != item.mesh.flgBlendFunc)
		{
			renderGpuMesh(item, addr);
			continue;
		}

		float distance = glm::length(eye - item.position);
		sorted[distance] = i;
	}

	// 2nd pass, alpha objects
	auto lst = actors.itemList;
	for (std::map<float, int>::iterator it = sorted.begin(); it != sorted.end(); ++it)
	{
		auto item = lst[(*it).second];
		renderGpuMesh(item, addr);
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

glm::mat4 Scene::calcView(glm::mat4* pView, glm::mat4 projection)
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
	return pv;
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

void Scene::renderGpuMesh(RenderItem& item, Scene::ShaderUniform& addr)
{
	glm::mat4 model = glm::mat4(1.0f);
	GpuMesh& g = item.mesh;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g.tex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g.tex[1]);

	model = glm::translate(glm::mat4(1.0f), item.position);
	model = glm::scale(model, item.scale);
	glUniformMatrix4fv(addr.model, 1, GL_FALSE, PTR_M(model));
	glUniform2f(addr.texScale, item.texScale.x, item.texScale.y);

	glBindVertexArray(g.vao);
	glDrawElements(GL_TRIANGLES, g.lenIndexBuffer, GL_UNSIGNED_SHORT, 0);
}
