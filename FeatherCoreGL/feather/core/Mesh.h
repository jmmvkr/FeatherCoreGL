#pragma once


#include "EBuffer.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace feather
{
	namespace core
	{

		typedef struct st_VertexStride {
			glm::vec3 pos;
			glm::vec3 norm;
			glm::vec2 texcoord;
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
				if (pFile)
				{
					return;
				}

				int i;
				auto len = GetCurrentDirectoryA((DWORD)szMax, buf);
				char* ptr = &buf[len];

				auto iEnd = sprintf_s(buf, sizeof(buf), "", ptr, relPath);
				if (iEnd >= 0)
				{
					buf[iEnd] = '\0';
				}
				fopen_s(pFile, buf, mode);
			}

			static st_Mesh load(const char* path)
			{
				st_Mesh st = { 0 };
				auto len = strlen(path);

				const char* fOpenMode = "rb";
				char pthVb[1 + MAX_CHAR];
				char pthIb[1 + MAX_CHAR];

				FILE* fVb = NULL;
				FILE* fIb = NULL;
				if (len >= 4)
				{
					// ensure extension (.vb or .ib) before loading
					const char* ptr = &path[len - 3];
					if ((0 != strcmp(ptr, ".vb")) && (0 != strcmp(ptr, ".ib")))
					{
						return st;
					}

					_snprintf_s(pthVb, sizeof(pthVb), "%s", path);
					_snprintf_s(pthIb, sizeof(pthIb), "%s", path);
					pthVb[len - 2] = 'v';
					pthIb[len - 2] = 'i';

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