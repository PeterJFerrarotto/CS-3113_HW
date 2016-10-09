#ifndef TEXTURED_ENTITY_H
#define TEXTURED_ENTITY_H
#include "Entity.h"

class ShaderProgram;
class TexturedEntity :
	public Entity
{
protected:
	GLuint textureID;
	std::vector<float> textureCoordinates;
public:
	TexturedEntity();
	~TexturedEntity();

	virtual GLuint getTextureID();

	virtual void draw(ShaderProgram* program);
	virtual void setTextureID(GLuint textureID);
	virtual void setTextureVertices(const std::vector<float>& textureCoordinates);
	virtual const std::vector<float>& getTextureVertices();

};

#endif