#pragma once

#include <vector>
#include <map>
#include "GpuMesh.h"


using std::vector;
using std::map;
using glm::vec3;
using glm::vec2;


namespace feather
{
	namespace core
	{

		typedef struct st_RenderItem {
			GpuMesh mesh;
			vec3 position;
			vec3 scale;
			vec2 texScale;
		} RenderItem;

		typedef struct st_RenderAnchor {
			BOOL bFound;
			int resourceIndex;
			void* ptrToRenderList;
			void add(vec3 pos, float scale = 1.0f, float texScale = 1.0f);
		} RenderAnchor;

		typedef struct st_RenderList {
			map<std::string, int> mapResource;
			vector<GpuMesh> meshList;
			vector<RenderItem> itemList;

			void addResource(std::string name, GpuMesh gm)
			{
				int idxNext = meshList.size();
				mapResource.insert(std::pair<std::string, int>(name, idxNext));
				meshList.push_back(gm);
			}

			RenderAnchor addAndFindResource(std::string name, GpuMesh gm)
			{
				addResource(name, gm);
				return findResource(name);
			}

			RenderAnchor findResource(std::string name)
			{
				RenderAnchor ret = { 0 };
				auto it = mapResource.find(name);
				if (it != mapResource.end())
				{
					ret.resourceIndex = (*it).second;
					ret.bFound = TRUE;
				}
				else
				{
					ret.resourceIndex = -1;
				}
				ret.ptrToRenderList = (void*)this;
				return ret;
			}

			void addItem(int idx, vec3 pos, float scale, float texScale)
			{
				RenderItem it = { 0 };

				int len = meshList.size();
				if (0 <= idx && idx < len)
				{
					auto gm = meshList[idx];

					it.mesh = gm;
					it.position = pos;
					it.scale = vec3(scale);
					it.texScale = vec2(texScale);
					itemList.push_back(it);
				}
			}
		} RenderList;

	}
}

