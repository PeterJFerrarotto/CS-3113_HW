#ifndef TEXT_ENTITY_H
#define TEXT_ENTITY_H
#include "CompositeEntity.h"
#include <string>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

class ShaderProgram;
class TextEntity :
	public CompositeEntity
{
protected:
	GLuint textSheet;
	float size;
	float spacing;
	float startingX;
	float startingY;
public:
	TextEntity(const std::string& text, GLuint textSheet, float size, float spacing, float startingX, float startingY);
	~TextEntity();

	void setDisplayText(const std::string& text);

	void draw(ShaderProgram* program);

	const std::string& getDisplayText();
};

#endif
