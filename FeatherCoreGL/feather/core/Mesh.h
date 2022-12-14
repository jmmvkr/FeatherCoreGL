#pragma once


#include "RawTypes.h"
#include "EBuffer.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#ifdef _WIN64
#include <windows.h>
#endif


#define GPU_UINT    u32
#define GPU_SINT    i32

#define PTR_V(x)  &x[0]
#define PTR_M(x)  &x[0][0]


namespace feather
{
	namespace core
	{

		typedef struct st_VertexStride {
			glm::vec3 pos;
			glm::vec2 texcoord;
			glm::vec3 norm;
		} VertexStride;

		typedef struct st_Mesh {
			int bLoaded;
			int resourceIndex;
			EBuffer<VertexStride> vb;
			EBuffer<glm::uint16> ib;

			static const int MAX_CHAR = 255;

			static void tryOpen(FILE** pFile, const char* relPath, const char* mode)
			{
				char buf[1 + MAX_CHAR] = { 0 };
				auto szMax = sizeof(buf);

				fopen_s(pFile, relPath, mode);
				if (*pFile)
				{
					return;
				}

#ifdef _WIN64
				auto len = GetCurrentDirectoryA((DWORD)szMax, buf);
				char* ptr = &buf[len];

				if ((1 + len) < sizeof(buf))
				{
					ptr[0] = '\\';
					ptr[1] = '\0';
				}

				strcat_s(buf, sizeof(buf), relPath);
				fopen_s(pFile, buf, mode);
#endif
			}

			static st_Mesh load(const char* pthVb, const char* pthIb)
			{
				st_Mesh st = { 0 };
				const char* fOpenMode = "rb";
				FILE* fVb = NULL;
				FILE* fIb = NULL;

				tryOpen(&fVb, pthVb, fOpenMode);
				tryOpen(&fIb, pthIb, fOpenMode);

				EBuffer<char> bufVb;
				EBuffer<char> bufIb;
				int bLoaded = 0;
				if (fVb && fIb)
				{
					bufVb = fullRead(fVb);
					bufIb = fullRead(fIb);

					auto convertedVb = EBuffer<VertexStride>::fromBuffer(bufVb, sizeof(VertexStride));
					auto convertedIb = EBuffer<u16>::fromBuffer(bufIb, sizeof(u16));

					if (convertedVb.data && convertedIb.data)
					{
						bLoaded = TRUE;
					}

					if (bLoaded)
					{
						st.vb = convertedVb;
						st.ib = convertedIb;
						st.bLoaded = TRUE;
					}
					else
					{
						convertedVb.freeBuffer();
						convertedIb.freeBuffer();
					}
				}

				if (fVb)
				{
					fclose(fVb);
				}
				if (fIb)
				{
					fclose(fIb);
				}
				return st;
			}


			static st_Mesh load(const char* path)
			{
				st_Mesh st = { 0 };
				auto len = strlen(path);

				char pthVb[1 + MAX_CHAR];
				char pthIb[1 + MAX_CHAR];

				if (len >= 4)
				{
					// ensure extension (.vx or .ix) before loading
					const char* ptr = &path[len - 3];
					if ((0 != strcmp(ptr, ".vx")) && (0 != strcmp(ptr, ".ix")))
					{
						return st;
					}

					_snprintf_s(pthVb, sizeof(pthVb), "%s", path);
					_snprintf_s(pthIb, sizeof(pthIb), "%s", path);
					pthVb[len - 2] = 'v';
					pthIb[len - 2] = 'i';

					st = load((const char*)pthVb, (const char*)pthIb);
				}

				return st;
			}

			static EBuffer<char> fullRead(FILE* fp)
			{
				EBuffer<char> raw = { 0 };
				auto len = fileLength(fp);

				char* buf = (char*)malloc(len);
				if (buf)
				{
					fread_s(buf, len, len, 1, fp);
					raw.len = len;
					raw.bufferSize = len;
					raw.data = buf;
					return raw;
				}

				return raw;
			}

			static long fileLength(FILE* fp)
			{
				long sz = 0;
				if (fp)
				{
					fseek(fp, 0L, SEEK_END);
					sz = ftell(fp);
					fseek(fp, 0L, SEEK_SET);
				}
				return sz;
			}


		} Mesh;
	}
}