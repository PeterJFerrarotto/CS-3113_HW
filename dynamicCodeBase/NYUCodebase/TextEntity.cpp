#include "TextEntity.h"
#include "Entity.h"
#include "Texture.h"
#include "ShaderProgram.h"
using namespace std;

TextEntity::TextEntity(){

}

TextEntity::TextEntity(const std::string& text, GLuint textSheet, float size, float spacing, float startingX, float startingY) : textSheet(textSheet)
{
	this->text = text;
	this->size = size;
	this->spacing = spacing;
	position.x = startingX;
	position.y = startingY;
	startingPosition.x = startingX;
	startingPosition.y = startingY;
}


TextEntity::~TextEntity()
{
}

void TextEntity::setDisplayText(const string& text){
	this->text = text;
}

void TextEntity::draw(ShaderProgram* program){
	Matrix modelMatrix;
	modelMatrix.identity();
	modelMatrix.Translate(startingPosition.x, startingPosition.y, 0.0);
	float texture_size = 1.0 / 16.0f; std::vector<float> vertexData; std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y,
			texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size,
		});
	}
	program->setModelMatrix(modelMatrix);
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, textSheet);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

const std::string& TextEntity::getDisplayText(){
	return text;
}

void TextEntity::setTextSheet(GLuint textSheet){
	this->textSheet = textSheet;
}