#ifndef ENTITY_H
#define ENTITY_H
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "Vector3.h"
#include "Matrix.h"
#include <vector>
#include <string>

enum BOUNDING_TYPE{ SQUARE, CIRCLE };

class Texture;
class ShaderProgram;
class Entity
{
protected:
	Texture* texture;
	Entity* next;

	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;

	Vector3 bounding;
	Vector3 scale;
	Vector3 baseSize;

	float rotation;
	float rotationalVelocity;

	bool canCollide;
	bool doRender;

	Matrix modelMatrix;
	void transformMatrix(Vector3 positionOffset, Vector3 scaleOffset, float rotationOffset);

	std::vector<GLfloat> objectVertices;
	std::vector<GLfloat> textureCoordinates;
	std::string entityID;

	BOUNDING_TYPE boundingType;

public:
	Entity();
	Entity(const std::string& entityID, Texture* texture);
	~Entity();

	const std::string& getEntityID();
	Vector3 getPosition();
	Vector3 getVelocity();
	Vector3 getAcceleration();
	Vector3 getBounding();
	Vector3 getScale();
	float getRotation();
	float getRotationalVelocity();
	Entity* getNext();
	BOUNDING_TYPE getBoundingType();
	bool getCanCollide();
	bool getDoRender();

	void setEntityID(const std::string& entityID);
	void setPosition(float x, float y, float z = 0.0f);
	void setVelocity(float x, float y, float z = 0.0f);
	void setAcceleration(float x, float y, float z = 0.0f);
	void setBounding(float x, float y, float z = 1.0f);
	void setScale(float x, float y, float z = 1.0f);
	void setRotation(float rotation);
	void setRotationalVelocity(float rotationalVelocity);
	void setNext(Entity* next);
	void setObjectVertices(const std::vector<GLfloat>& objectVertices);
	void setBoundingType(BOUNDING_TYPE boundingType);
	void setCanCollide(bool canCollide);
	void setDoRender(bool doRender);

	void freeMemory();
	void move(float elapsed);
	void addEntity(Entity* toAdd);
	void draw(ShaderProgram* program, Vector3 positionOffset, Vector3 scaleOffset, float rotationOffset);

	void blink();
	void blinkAll();

	void updateBounding();
};

#endif