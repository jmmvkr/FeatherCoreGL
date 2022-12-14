#pragma once


#include "CoreWindow.h"



namespace feather
{
	namespace core
	{

		typedef struct st_Info {
			char msg[512];
			int iStart;
			void append(const wchar_t* wstr)
			{
				int i;
				int iLast = sizeof(msg) - 1;
				int wOff = 0;
				for (i = iStart; i < iLast; i++, wOff++)
				{
					int c = wstr[wOff];
					if (c <= 0 || c >= 128) { break; }
					msg[i] = (char)c;
				}
				msg[i] = '\0';
				iStart = i;
			}
			void assign(const wchar_t* wstr)
			{
				iStart = 0;
				append(wstr);
			}
		} Info;

		typedef struct st_GlShader {
			Info path;
			GLint id;
		} GlShader;

		typedef struct st_GlProgram {
			BOOL bLinked;
			GlShader vShader;
			GlShader fShader;
			GLint prog;
		} GlProgram;


		class Shader
		{

		public:

			static void readSource(const wchar_t* path, std::string& stdString)
			{
				// [reference] https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
				std::ifstream txtFile(path);
				std::stringstream buffer;

				buffer << txtFile.rdbuf();
				auto str = buffer.str();
				stdString.append(str);
			}


			static void printGlShaderLog(int maxLength, std::vector<GLchar>& infoLog)
			{
				int i = 0;
				if ('0' == infoLog[0])
				{
					i++;
				}
				LOG() << "   ";
				for (; i < maxLength; i++)
				{
					GLchar chr = infoLog[i];
					if (!chr) break;
					LOG() << chr;
				}
				LOG() << NL;
			}

			static void printShaderError(GLuint shader)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				printGlShaderLog(maxLength, infoLog);
			}

			static void printProgramError(GLuint prog)
			{
				GLint maxLength = 0;
				glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(prog, maxLength, &maxLength, &infoLog[0]);
			}

			static GLuint compileShader(GLenum tp, const wchar_t* path)
			{
				Info nm = {};
				std::string source;
				readSource(path, source);

				const GLchar* cstr = source.c_str();
				GLint lengths[] = { (GLint)strlen(cstr) };

				nm.append(path);
				GLuint shader = glCreateShader(tp);
				if (NO_SHADER == shader)
				{
					LOG() << NL << " # [Shader-Creation-Failed] " << nm.msg << NL;
					return NO_SHADER;
				}

				glShaderSource(shader, GLsizei(1), &cstr, lengths);
				glCompileShader(shader);

				GLint bCompiled = 0;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &bCompiled);
				if (GL_FALSE == bCompiled)
				{
					LOG() << NL << " # [Shader-Error] " << nm.msg << NL;
					printShaderError(shader);
					glDeleteShader(shader);
					return NO_SHADER;
				}
				LOG() << " # [Shader-Done]  < " << shader << " > " << nm.msg << NL;
				return shader;
			}

			static GlProgram createProgram(const wchar_t* vsPath, const wchar_t* fsPath)
			{
				GlProgram p = {};
				GLint bLinked = 0;
				GLint prog = 0;

				p.bLinked = FALSE;
				p.vShader.path.assign(vsPath);
				p.fShader.path.assign(fsPath);

				auto vs = compileShader(GL_VERTEX_SHADER, vsPath);
				auto fs = compileShader(GL_FRAGMENT_SHADER, fsPath);
				if (vs && fs)
				{
					prog = glCreateProgram();
					glAttachShader(prog, vs);
					glAttachShader(prog, fs);
					glLinkProgram(prog);

					glGetProgramiv(prog, GL_LINK_STATUS, &bLinked);
					if (GL_FALSE == bLinked)
					{
						printProgramError(prog);

						// release program
						glDeleteProgram(prog);
						// release shader
						glDeleteShader(vs);
						glDeleteShader(fs);
					}
					else
					{
						p.vShader.id = vs;
						p.fShader.id = fs;
						p.prog = prog;
						p.bLinked = TRUE;

						// Always detach shaders after a successful link.
						// https://www.khronos.org/opengl/wiki/Shader_Compilation
					}
				}
				return p;
			}

		};
	}
}