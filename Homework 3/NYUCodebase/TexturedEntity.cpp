#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "TexturedEntity.h"
#include "ShaderProgram.h"
TexturedEntity::TexturedEntity()
{
	textureCoordinates = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
}


TexturedEntity::~TexturedEntity()
{
}

void TexturedEntity::draw(ShaderProgram* program){
	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);
	modelMatrix.Rotate(rotation);
	modelMatrix.Roll(roll);
	modelMatrix.Yaw(yaw);
	glBindTexture(GL_TEXTURE_2D, textureID);
	program->setModelMatrix(modelMatrix);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, objectVertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void TexturedEntity::setTextureID(GLuint textureID){
	this->textureID = textureID;
}

void TexturedEntity::setTextureVertices(const std::vector<float>& textureCoordinates){
	this->textureCoordinates = textureCoordinates;
}

GLuint TexturedEntity::getTextureID(){
	return textureID;
}

const std::vector<float>& TexturedEntity::getTextureVertices(){
	return textureCoordinates;
}

void TexturedEntity::setDefaultTextureVertices(){
	textureCoordinates = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
}