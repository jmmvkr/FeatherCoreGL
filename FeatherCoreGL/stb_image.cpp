
// apply stb_image implementation here
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char* loadByStbImage(const char* fname, int* pw, int* ph, int* pBpp)
{
	int w = 0;
	int h = 0;
	int comp = 0;
	unsigned char* buf = NULL;
	int curComp = 0;

	curComp = 4;
	buf = stbi_load(fname, &w, &h, &comp, 4);
	if (buf && (comp < 4))
	{
		free(buf);
		curComp = 3;
		buf = stbi_load(fname, &w, &h, &comp, 3);
	}

	(*pw) = w;
	(*ph) = h;
	(*pBpp) = curComp;

	return buf;
}
