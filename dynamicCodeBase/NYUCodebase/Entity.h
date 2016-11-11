#ifndef ENTITY_H
#define ENTITY_H
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "Vector3.h"
#include "Matrix.h"
#include <vector>
#include "Animation.h"
#include <unordered_map>
#include <string>

enum BOUNDING_TYPE{ SQUARE, CIRCLE };

class Texture;
class ShaderProgram;
class Entity
{
protected:
	Texture* texture;
	Entity* child;
	Entity* sibling;

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
	bool isAnimated;
	bool doMirror;

	Matrix modelMatrix;
	Matrix offsetModelMatrix;
	void transformMatrix();

	std::vector<GLfloat> objectVertices;
	std::vector<GLfloat> textureCoordinates;
	std::string entityID;
	std::unordered_map<unsigned, Animation*> animations;
	ANIMATION_TYPE currentAnimation;

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
	Entity* getChild();
	Entity* getSibling();
	BOUNDING_TYPE getBoundingType();
	bool getCanCollide();
	bool getDoRender();
	bool getIsAnimated();
	bool getDoMirror();

	void setEntityID(const std::string& entityID);
	void setPosition(float x, float y, float z = 0.0f);
	void setVelocity(float x, float y, float z = 0.0f);
	void setAcceleration(float x, float y, float z = 0.0f);
	void setBounding(float x, float y, float z = 1.0f);
	void setScale(float x, float y, float z = 1.0f);
	void setRotation(float rotation);
	void setRotationalVelocity(float rotationalVelocity);
	void setChild(Entity* next);
	void setSibling(Entity* sibling);
	void setObjectVertices(const std::vector<GLfloat>& objectVertices);
	void setBoundingType(BOUNDING_TYPE boundingType);
	void setCanCollide(bool canCollide);
	void setDoRender(bool doRender);
	void setDoMirror(bool doMirror);
	void setIsAnimated(bool isAnimated);
	void addAnimation(Animation* animation);

	void freeMemory();
	void move(float elapsed);
	void addSibling(Entity* siblingToAdd);
	void draw(ShaderProgram* program, Matrix offset);

	void blink();
	void blinkAll();

	void updateBounding(float scaleX, float scaleY, float scaleZ);

	void startAnimation(ANIMATION_TYPE animation);

	void runAnimation(float elapsed, float fps);
};

#endif