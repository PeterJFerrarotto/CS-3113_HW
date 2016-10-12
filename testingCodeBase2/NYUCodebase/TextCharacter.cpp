#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "TextCharacter.h"
#include "ShaderProgram.h"

TextCharacter::TextCharacter()
{
}

TextCharacter::TextCharacter(char character, GLuint textFont, float size){
	this->character = character;
	textureID = textFont;
	this->size = size;
}


TextCharacter::~TextCharacter()
{
}

void TextCharacter::draw(ShaderProgram* program){

	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);
	modelMatrix.Rotate(rotation);
	float texture_size = 1.0 / 16.0f;
	float texture_x = (float)(((int)character) % 16) / 16.0f;
	float texture_y = (float)(((int)character) / 16) / 16.0f;
	objectVertices = { (-0.5f * size), 0.5f * size, (-0.5f * size), -0.5f * size, (0.5f * size), 0.5f * size, (0.5f * size), -0.5f * size, (0.5f * size), 0.5f * size, +(-0.5f * size), -0.5f * size, };
	textureCoordinates = { texture_x, texture_y,
		texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size,
	};
	glBindTexture(GL_TEXTURE_2D, textureID);
	program->setModelMatrix(modelMatrix);

	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, objectVertices.data());
	
	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}
