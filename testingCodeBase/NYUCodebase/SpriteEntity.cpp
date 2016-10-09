#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "SpriteEntity.h"
#include "ShaderProgram.h"


SpriteEntity::SpriteEntity()
{
}


SpriteEntity::~SpriteEntity()
{
}

void SpriteEntity::draw(ShaderProgram* program){
	glBindTexture(GL_TEXTURE_2D, textureID);

	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;

	textureCoordinates = { u, v + spriteHeight, u + spriteWidth, v, u, v,
		u + spriteWidth, v, u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight };

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, objectVertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void SpriteEntity::setIndex(int index){
	this->index = index;
}

void SpriteEntity::setSpriteCount(int spriteCountX, int spriteCountY){
	this->spriteCountX = spriteCountX;
	this->spriteCountY = spriteCountY;
}