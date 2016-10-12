#ifndef ENTITY_H
#define ENTITY_H

#include <SDL_opengl.h>
#include <SDL.h>
#include <string>
#include "Matrix.h"
#include <vector>
#include "Vector3.h"
#include <array>

enum BOUNDING_TYPE{ SQUARE, CIRCLE, BOUNDING_TYPE_COUNT };
enum BOUNDARY_BEHAVIOR{ BOUND_BOUNCE, BOUNDING_BEHAVIOR_COUNT };
enum COLLISION_BEHAVIOR{ COLL_IGNORE };
enum ENTITY_TYPE{ TEXT, ENTITY_TYPE_COUNT};

enum STATE {ACTIVE, DESTROYED};
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
	Vector3 startingPosition;
	Vector3 velocity;
	Vector3 startingVelocity;
	Vector3 bounding;
	float velX, velY, velZ;
	float rotation;
	float velRot;
	int state;
	int collisionBehavior;
	int boundaryBehavior;
	int entityType;
	std::vector<std::string> collidesWith;
	std::vector<int> collidesWithTypes;
	float firingDelay;
	float roll;
	float yaw;
	bool render;
	int index, spriteCountX, spriteCountY;
	float timeAlive;
public:
	Entity();
	~Entity();
	void setCanCollide(bool canCollide);
	void setEntityID(const std::string& entityID);
	void setModelMatrix(const Matrix& modelMatrix);
	void setObjectVertices(const std::vector<float>& objectVertices);
	void setCollisionType(int collisionType);
	void setPosition(float posX, float posY, float posZ = 0.0);
	void setVelocity(float velX, float velY, float velZ = 0.0);
	void setBounding(float boundX, float boundY, float boundZ = 1.0);
	void setRotation(float rotation);
	void setVelRot(float velRot);
	void setState(int state);
	void transformMatrix();
	void setBoundaryBehavior(int boundaryBehavior);
	void addCollisionCheck(const std::string& entityID);
	void addCollisionCheck(int entityID);
	void setCollisionBehavior(int collisionBehavior);
	void setEntityType(int entityType);
	void resetFiringDelay(float newDelay = 1.0);
	void decreaseFiringDelay(float elapsed);
	void setRoll(float roll);
	void setYaw(float yaw);
	void setRender(bool render);
	void setStartingPosition(float x, float y, float z = 0);
	void setStartingVelocity(float x, float y, float z = 0);
	void returnToStart();
	void incrementTimeAlive(float elapsed);

	bool getCanFire();
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
	virtual const std::vector<float>& getTextureVertices();
	int getBoundaryBehavior();
	const std::vector<std::string>& getCollisionList();
	const std::vector<int>& getTypeCollisionList();
	int getEntityType();
	int getCollisionBehavior();
	float getRoll();
	float getYaw();
	bool getRender();
	float getTimeAlive();

	virtual void setTextureID(GLuint textureID);
	virtual void draw(ShaderProgram* program);
	virtual void setTextureVertices(const std::vector<float>& textureCoordinates);
	virtual void setIndex(int index);
	virtual void setSpriteCount(int spriteCountX, int spriteCountY);
	virtual void setDefaultTextureVertices();
};

#endif
