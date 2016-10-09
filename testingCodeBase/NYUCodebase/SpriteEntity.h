#ifndef SPRITE_ENTITY_H
#define SPRITE_ENTITY_H

#include "TexturedEntity.h"
class SpriteEntity :
	public TexturedEntity
{
protected:
	int index, spriteCountX, spriteCountY;
public:
	SpriteEntity();
	~SpriteEntity();

	void draw(ShaderProgram* program);

	void setIndex(int index);
	void setSpriteCount(int spriteCountX, int spriteCountY);
};

#endif