#ifndef COMPOSITE_ENTITY_H
#define COMPOSITE_ENTITY_H
#include <string>
#include "Vector3.h"
#include "Entity.h"

enum ENTITY_TYPE { ICON_ENTITY, LIFE_ICON_ENTITY, PLAYER_PROJECTILE, ENEMY_PROJECTILE, TITLE_TEXT_ENTITY, GAME_TEXT_ENTITY, POINTS_INDICATOR, ACTOR_ENEMY, ACTOR_PLAYER, ACTOR_ENEMY_SAUCER, STATIC_ENTITY, ENTITY_TYPE_SIZE };
enum STATE {MOVING, ACCELERATING, STATIONARY, BOUNCING, BOUNCED, STATE_COUNT};
enum COLLISION_BEHAVIOR {BOUNCE, STOP, DESTROY, DEACTIVATE, NOTHING};
enum BOUNDARY_BEHAVIOR {BOUND_BOUNCE, BOUND_TURN, BOUND_STOP, BOUND_DESTROY, BOUND_DEACTIVATE, BOUND_NOTHING};
enum DIRECTION {UP, DOWN, LEFT, RIGHT, X, Y};

class ShaderProgram;
class Texture;
class CompositeEntity
{
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
	float getRotation();
	float getRotationalVelocity();
	BOUNDING_TYPE getBoundingType();
	ENTITY_TYPE getEntityType();
	COLLISION_BEHAVIOR getCollisionBehavior();
	BOUNDARY_BEHAVIOR getBoundaryBehavior();
	STATE getState();
	bool getIsActive();
	bool getDoDelete();
	bool getCanCollide();
	bool getisInvincible();
	const std::string& getEntityID();

	void setEntities(Entity* first);
	void setPosition(float x, float y, float z = 0.0);
	void setStartingPosition(float x, float y, float z = 0.0);
	void setVelocity(float x, float y, float z = 0.0);
	void setStartingVelocity(float x, float y, float z = 0.0);
	void setAcceleration(float x, float y, float z = 0.0);
	void setStartingAcceleration(float x, float y, float z = 0.0);
	void setScale(float x, float y, float z = 1.0);
	void setStartingScale(float x, float y, float z = 1.0);
	void setRotation(float rotation);
	void setRotationalVelocity(float rotation);
	void setBoundingType(BOUNDING_TYPE boundingType);
	void setEntityType(ENTITY_TYPE type);
	void setCollisionBehavior(COLLISION_BEHAVIOR behavior);
	void setBoundaryBehavior(BOUNDARY_BEHAVIOR behavior);
	void setState(STATE state);
	void setIsActive(bool isActive);
	void setCanCollide(bool canCollide);
	void setisInvincible(bool isInvincible);
	void setEntityID(const std::string& entityID);

	void updateBounding();
	void reset();
	void addEntity(Entity* toAdd);
	bool isColliding(CompositeEntity* collidingCheck);
	bool atScreenBoundary(float gameWall, float gameCeiling);
	void move(float elapsed, float gravity = 0.0f, float frictionX = 0.0f, float frictionY = 0.0f);
	virtual void draw(ShaderProgram* program);
	void collide(float elapsed, CompositeEntity* bouncingOffOf);
	void collideWithStatic(float penetration, DIRECTION direction);
	void boundaryAction(float gameWall, float gameCeiling);
	void blink();


	virtual CompositeEntity* fire(Texture* projectileTexture);

	virtual void addToTimeSinceFiring(float elapsed);

	virtual CompositeEntity* logic(CompositeEntity* player, Texture* projectileTexture, CompositeEntity* lastProjectile = nullptr);

	virtual void setDisplayText(const std::string& text);

	virtual void generateText();

	virtual const std::string& getDisplayText();
protected:
	Entity* first;

	Vector3 position;
	Vector3 startingPosition;
	Vector3 velocity;
	Vector3 startingVelocity;
	Vector3 acceleration;
	Vector3 startingAcceleration;
	Vector3 totalBounding;
	Vector3 scale;
	Vector3 startingScale;

	float rotation;
	float rotationalVelocity;
	float topSpeed;

	ENTITY_TYPE type;
	BOUNDING_TYPE boundingType;
	COLLISION_BEHAVIOR collisionBehavior;
	BOUNDARY_BEHAVIOR boundaryBehavior;
	STATE state;

	bool isActive;
	bool doDelete;
	bool collideLeft, collideRight, collideTop, collideBottom;
	bool staticCollideLeft, staticCollideRight, staticCollideTop, staticCollideBottom;
	bool boundaryLeft, boundaryRight, boundaryTop, boundaryBottom;
	bool canCollide;
	bool subEntitiesColliding(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat);
	bool isInvincible;

	void bounce(float elapsed, CompositeEntity* bouncingOffOf);
	virtual void destroy();
	void stop();
	void deActivate();
	void resetFlags();

	std::string entityID;
	std::string text;

	float lerp(float v0, float v1, float t);
	void boundaryStop(float gameWall, float gameCeiling);
	void boundaryTurn(float gameWall, float gameCeiling);
	void boundaryTurnAndDown(float gameWall, float gameCeiling);
};

#endif