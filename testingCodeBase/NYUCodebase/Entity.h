#ifndef ENTITY_H
#define ENTITY_H

#include <SDL_opengl.h>
#include <SDL.h>
#include <string>
#include "Matrix.h"
#include <vector>
#include "Vector3.h"
#include <array>

enum BOUNDING_TYPE{ SQUARE, CIRCLE };
enum BOUNDARY_BEHAVIOR{ STOP, BOUNCE, BOUNCE_PUCK };
class ShaderProgram;

class Entity
{
protected:
	bool canCollide;
	std::string entityID;
	Matrix modelMatrix;
	std::vector<float> objectVertices;
	unsigned collisionType;
	Vector3 position;
	Vector3 velocity;
	Vector3 bounding;
	float velX, velY, velZ;
	float rotation;
	float velRot;
	int state;
	int boundaryBehavior;
public:
	Entity();
	~Entity();
	void setCanCollide(bool canCollide);
	void setEntityID(const std::string& entityID);
	void setModelMatrix(const Matrix& modelMatrix);
	void setObjectVertices(const std::vector<float>& objectVertices);
	void setCollisionType(int collisionType);
	void setPosition(float posX, float posY, float posZ = 1.0);
	void setVelocity(float velX, float velY, float velZ = 0.0);
	void setBounding(float boundX, float boundY, float boundZ = 1.0);
	void setRotation(float rotation);
	void setVelRot(float velRot);
	void addCollisionChecker(const std::string& entityID);
	void setState(int state);
	void transformMatrix();
	void setBoundaryBehavior(int boundaryBehavior);

	bool getCanCollide();
	std::string& getEntityID();
	Matrix getModelMatrix();
	const std::vector<float>& getObjectVertices();
	Vector3 getPosition();
	Vector3 getVelocity();
	Vector3 getBounding();
	float getObjectRotation();
	float getObjectVelRot();
	int getState();
	int getCollisionType();
	virtual GLuint getTextureID() = 0;
	virtual const std::vector<float>& getTextureVertices() = 0;
	int getBoundaryBehavior();

	virtual void setTextureID(GLuint textureID) = 0;
	virtual void draw(ShaderProgram* program) = 0;
	virtual void setTextureVertices(const std::vector<float>& textureCoordinates) = 0;
};

#endif
