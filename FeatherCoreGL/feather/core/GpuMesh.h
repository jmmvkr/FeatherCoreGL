#pragma once


#include "Mesh.h"
#include "CoreWindow.h"


using feather::core::Mesh;
extern unsigned char* loadByStbImage(const char* fname, int* pw, int* ph, int* pBpp);



namespace feather
{
	namespace core
	{

		typedef struct st_GpuMesh {
			GPU_UINT vao;
			GPU_UINT vbo;
			GPU_UINT ebo;
			GPU_SINT lenIndexBuffer;
			GPU_SINT offIndexBuffer;
			GPU_UINT tex[2];
			int resourceIndex;
			int flgBlendFunc;
		} GpuMesh;


#define MAX_CELL    (10)
		typedef struct st_ECell
		{
			GLuint index;
			GLint count;
			void* offset;
		} ECell;

		typedef struct st_EHelper
		{
			GLsizei stride;
			GLint count;
			GLint curOffset;
			ECell cells[MAX_CELL];

			void addCell(int idx, size_t szUnit, int unitCount)
			{
				char* arrNil = NULL;
				ECell newCell = { 0 };
				newCell.index = idx;
				newCell.count = unitCount;
				newCell.offset = &arrNil[curOffset];

				if (idx != count)
				{
					throw new std::exception("invalid cell sequence");
				}
				cells[count] = newCell;

				auto szBlock = (GLsizei)(unitCount * szUnit);
				stride += szBlock;
				curOffset += szBlock;
				count++;
			}

			void apply()
			{
				int it;
				for (it = 0; it < count; it++)
				{
					auto c = cells[it];
					glEnableVertexAttribArray(c.index);
					glVertexAttribPointer(c.index, c.count, GL_FLOAT, GL_FALSE, stride, (const void*)c.offset);
				}
			}
		} EHelper;


		class GpuUtil
		{

		public:
			static GpuMesh loadMesh(Mesh m, int ibOffset, int ibCount)
			{
				GpuMesh gm = { 0 };

				int szTotal = m.vb.bufferSize;
				const u16* indices = &(m.ib.data[0]);
				const float* vertices = (float*)(m.vb.data);

				GLuint VAO = {};
				GLuint VBO = {};
				GLuint EBO = {};

				EHelper eh = { 0 };
				auto ibUnit = (m.ib.bufferSize / m.ib.len);
				int lenIb = m.ib.len;
				if (vertices)
				{
					if (ibCount < 0)
					{
						ibOffset = 0;
					}
					else
					{
						lenIb = ibCount;
					}

					glGenVertexArrays(1, &VAO);
					glGenBuffers(1, &VBO);
					glGenBuffers(1, &EBO);

					// VAO
					glBindVertexArray(VAO);
					// VBO
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glBufferData(GL_ARRAY_BUFFER, szTotal, vertices, GL_STATIC_DRAW);
					// EBO
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibUnit * lenIb, &indices[ibOffset], GL_STATIC_DRAW);

					// calculate offsets for VAO
					eh.addCell(0, sizeof(float), 3);
					eh.addCell(1, sizeof(float), 2);
					eh.addCell(2, sizeof(float), 3);
					eh.apply();

					// output GpuMesh information
					gm.vao = VAO;
					gm.vbo = VBO;
					gm.ebo = EBO;
					gm.lenIndexBuffer = lenIb;
					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					gm.resourceIndex = m.resourceIndex;
				}

				return gm;
			}

			static GpuMesh loadMesh(Mesh m)
			{
				return loadMesh(m, 0, -1);
			}

			static GLuint loadTexture(void* pixelData, int width, int height, GLenum idxTexture, int bpp, fpVoid fpTexParam)
			{
				GLuint texture = 0;
				glGenTextures(1, &texture);
				if (texture)
				{
					glActiveTexture(GL_TEXTURE0 + idxTexture);
					glBindTexture(GL_TEXTURE_2D, texture);
					glEnable(GL_TEXTURE_2D);

					fpTexParam();

					GLint level = 0;
					GLint border = 0;
					int eInternalFormat = GL_RGB;
					int eFormat = GL_RGB;
					if (pixelData)
					{
						if (4 == bpp)
						{
							eInternalFormat = GL_RGBA8;
							eFormat = GL_RGBA;
						}
						glTexImage2D(GL_TEXTURE_2D, level, eInternalFormat, width, height, border, eFormat, GL_UNSIGNED_BYTE, pixelData);
						glGenerateMipmap(GL_TEXTURE_2D);
					}
				}
				glBindTexture(GL_TEXTURE_2D, 0);

				if (!texture)
				{
					throw new std::exception("texture error");
				}
				return texture;
			}

			static GLuint loadTexture(int idxTexture, const char* path)
			{
				return loadTexture(idxTexture, path, GpuUtil::texLinearRepeat);
			}

			static GLuint loadTexture(int idxTexture, const char* path, fpVoid fpTexParam)
			{
				GLuint texGpuId = 0;
				int bpp = 0;
				int w = 0;
				int h = 0;
				u8* buf = NULL;

				fpVoid fp = texLinearClamp;

				buf = ::loadByStbImage(path, &w, &h, &bpp);
				if (buf)
				{
					texGpuId = loadTexture(buf, w, h, idxTexture, bpp, fpTexParam);
					free(buf);
				}
				return texGpuId;
			}

			static GLuint createShadowFrameBuffer(int w, int h, GLuint* texOut)
			{
				BOOL bSuccess = FALSE;
				GLuint depthMapFBO = 0;
				GLuint depthMap = 0;

				(*texOut) = 0;
				glGenFramebuffers(1, &depthMapFBO);
				if (depthMapFBO)
				{
					depthMap = createShadowTexture(w, h);
					if (!depthMap)
					{
						goto cleanup;
					}

					glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);

					if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
					{
						bSuccess = TRUE;
					}
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}

			cleanup:
				if (bSuccess)
				{
					(*texOut) = depthMap;
					return depthMapFBO;
				}
				else
				{
					if (depthMapFBO)
					{
						glDeleteFramebuffers(1, &depthMapFBO);
					}
					if (depthMap)
					{
						glDeleteTextures(1, &depthMap);
					}
					return 0;
				}
			}

			static GLuint createShadowTexture(int w, int h)
			{
				GLuint depthMap;
				glGenTextures(1, &depthMap);

				if (depthMap)
				{
					glBindTexture(GL_TEXTURE_2D, depthMap);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

					texDepthMap();
				}
				return depthMap;
			}

			static void texLinearClamp()
			{
				// texture wraping
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // wrap: GL_CLAMP or GL_REPEAT
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

				// texture filtering
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			
			static void texLinearRepeat()
			{
				// texture wraping
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap: GL_CLAMP or GL_REPEAT
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				// texture filtering
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			static void texDepthMap()
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		};
	}
}

