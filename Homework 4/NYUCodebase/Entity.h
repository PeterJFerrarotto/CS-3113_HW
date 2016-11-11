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

	Vector3 size;
	Vector3 scale;
	Vector3 baseSize;

	float rotation;
	float rotationalVelocity;

	bool canCollide;
	bool doRender;
	bool isAnimated;
	bool doMirror;
	BOUNDING_TYPE boundingType;
	std::unordered_map<unsigned, Animation*> animations;
	ANIMATION_TYPE currentAnimation;
	std::string entityID;

	Matrix modelMatrix;
	Matrix offsetModelMatrix;
	void transformMatrix();

	std::vector<GLfloat> objectVertices;
	std::vector<GLfloat> textureCoordinates;
public:
	Entity();
	Entity(const std::string& entityID, Texture* texture);
	~Entity();

	const std::string& getEntityID();
	Vector3 getPosition();
	Vector3 getVelocity();
	Vector3 getAcceleration();
	Vector3 getSize();
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
	bool hasAnimation(ANIMATION_TYPE animationType);
	bool animationComplete(ANIMATION_TYPE animationType);

	void setEntityID(const std::string& entityID);
	void setTexture(Texture* texture);
	void setPosition(float x, float y, float z = 0.0f);
	void setVelocity(float x, float y, float z = 0.0f);
	void setAcceleration(float x, float y, float z = 0.0f);
	void setSize(float x, float y, float z = 1.0f);
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
	void draw(ShaderProgram* program, Matrix offset, float elapsed, float fps);

	void blink();
	void blinkAll();

	void updateBounding(float scaleX, float scaleY, float scaleZ);

	void startAnimation(ANIMATION_TYPE animation);

	void runAnimation(float elapsed, float fps);

	virtual void deepCopy(Entity* toCopy);
};

#endif