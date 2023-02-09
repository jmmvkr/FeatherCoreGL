
// definition for a Character in a font
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	glm::ivec2   Advance;    // Offset to advance to next glyph
};

// load glyph of a given size
// 
// deps follows
// #include <ft2build.h>
// #include FT_FREETYPE_H  
//
int prepareFonts()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "media/consola.ttf", 0, &face))
	//if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);
	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		glm::vec2 ivSize;
		glm::vec2 ivPos;
		glm::vec2 ivAdvance;
		FT_GlyphSlot glyph = face->glyph;
		ivSize.x = glyph->bitmap.width;
		ivSize.y = glyph->bitmap.rows;
		ivPos.x = glyph->bitmap_left;
		ivPos.y = glyph->bitmap_top;
		ivAdvance.x = glyph->advance.x;
		ivAdvance.y = glyph->advance.y;
		auto iAdvance = glyph->advance.x;
		auto buffer = glyph->bitmap.buffer;

		Character chr = {
			0, ivSize, ivPos, ivAdvance
		};
	}

	std::cout << "Font was loaded" << NL;
}
