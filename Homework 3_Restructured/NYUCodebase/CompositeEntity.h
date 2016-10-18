#ifndef COMPOSITE_ENTITY_H
#define COMPOSITE_ENTITY_H

#include "Vector3.h"
enum ENTITY_TYPE {ICON_ENTITY, PLAYER_PROJECTILE, ENEMY_PROJECTILE, TEXT_ENTITY, ACTOR_ENEMY, ACTOR_PLAYER, ENTITY_TYPE_SIZE};
enum OBJECT_TYPE {STATIC, DYNAMIC};
enum STATE {MOVING, ACCELERATING, STATIONARY, BOUNCING, BOUNCED, STATE_COUNT};
enum COLLISION_BEHAVIOR {BOUNCE, STOP, DESTROY, DEACTIVATE, NOTHING};
enum BOUNDARY_BEHAVIOR {BOUND_BOUNCE, BOUND_TURN, BOUND_TURN_AND_DOWN, BOUND_STOP, BOUND_DESTROY, BOUND_DEACTIVATE, BOUND_NOTHING};

class Entity;
class ShaderProgram;
class Texture;
class CompositeEntity
{
protected:
	Entity* first;

	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 totalBounding;
	Vector3 scale;

	int boundingType;

	float rotation;
	float rotationalVelocity;
	float topSpeed;

	int type;
	unsigned collisionBehavior;
	unsigned boundaryBehavior;
	unsigned objecType;
	unsigned state;
	
	bool isActive;
	bool doDelete;
	bool collideLeft, collideRight, collideTop, collideBottom;
	bool boundaryLeft, boundaryRight, boundaryTop, boundaryBottom;
	bool canCollide;
	bool subEntitiesColliding(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat);

	void updateBounding();
	void bounce(float elapsed, CompositeEntity* bouncingOffOf);
	void destroy();
	void stop();
	void deActivate();

	void boundaryStop(float gameWall, float gameCeiling);
	void boundaryTurn(float gameWall, float gameCeiling);
	void boundaryTurnAndDown(float gameWall, float gameCeiling);
public:
	CompositeEntity();
	CompositeEntity(Entity* first);
	~CompositeEntity();

	Entity* getEntities();
	Vector3 getPosition();
	Vector3 getVelocity();
	Vector3 getAcceleration();
	Vector3 getTotalBounding();
	Vector3 getScale();
	int getBoundingType();
	float getRotation();
	float getRotationalVelocity();
	int getEntityType();
	unsigned getCollisionBehavior();
	unsigned getBoundaryBehavior();
	unsigned getState();
	unsigned getObjectType();
	bool getIsActive();
	bool getDoDelete();
	bool getCanCollide();

	void setEntities(Entity* first);
	void setPosition(float x, float y, float z = 0.0);
	void setVelocity(float x, float y, float z = 0.0);
	void setAcceleration(float x, float y, float z = 0.0);
	void setScale(float x, float y, float z = 1.0);
	void setBoundingType(int boundingType);
	void setRotation(float rotation);
	void setRotationalVelocity(float rotation);
	void setEntityType(int type);
	void setCollisionBehavior(unsigned behavior);
	void setBoundaryBehavior(unsigned behavior);
	void setState(unsigned state);
	void setBoundaryType(unsigned type);
	void setIsActive(bool isActive);
	void setCanCollide(bool canCollide);

	void addEntity(Entity* toAdd);
	bool isColliding(CompositeEntity* collidingCheck);
	bool atScreenBoundary(float gameWall, float gameCeiling);
	void move(float elapsed);
	void draw(ShaderProgram* program);
	void collide(float elapsed, CompositeEntity* bouncingOffOf);
	void boundaryAction(float gameWall, float gameCeiling);


	virtual CompositeEntity* fire(Texture* projectileTexture);

	virtual void addToTimeSinceFiring(float elapsed);

	virtual CompositeEntity* logic(CompositeEntity* player, Texture* projectileTexture, CompositeEntity* lastProjectile = nullptr);
};

#endif