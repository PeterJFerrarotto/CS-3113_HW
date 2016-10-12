#ifndef TEXT_CHARACTER_H
#define TEXT_CHARACTER_H

#include "SpriteEntity.h"

class TextCharacter :
	public SpriteEntity
{
protected:
	char character;
	float size;
public:
	TextCharacter();
	TextCharacter(char character, GLuint textFont, float size);
	~TextCharacter();

	void draw(ShaderProgram* program);
};

#endif