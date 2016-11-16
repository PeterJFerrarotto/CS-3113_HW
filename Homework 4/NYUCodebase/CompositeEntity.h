#ifndef COMPOSITE_ENTITY_H
#define COMPOSITE_ENTITY_H
#include <string>
#include "Vector3.h"
#include "Entity.h"

enum ENTITY_TYPE { ICON_ENTITY, LIFE_ICON_ENTITY, PLAYER_PROJECTILE, ENEMY_PROJECTILE, TITLE_TEXT_ENTITY, GAME_TEXT_ENTITY, POINTS_INDICATOR, ACTOR_ENEMY, ACTOR_PLAYER, ACTOR_ENEMY_PATROL_TURN, STATIC_ENTITY, BACKGROUND_ENTITY, ENTITY_COIN, ENTITY_TYPE_SIZE };
enum STATE {MOVING, ACCELERATING, STATIONARY, BOUNCING, BOUNCED, IDLE, JUMPING, FALLING, DESTROYING, STATE_COUNT};
enum COLLISION_BEHAVIOR { BOUNCE, STOP, DESTROY, DEACTIVATE, NOTHING, BOUNCE_HIGH, COLLISION_BEHAVIOR_SIZE };
enum BOUNDARY_BEHAVIOR {BOUND_BOUNCE, BOUND_TURN, BOUND_STOP, BOUND_DESTROY, BOUND_DEACTIVATE, BOUND_NOTHING};
enum TILE_COLLISION_BEHAVIOR {T_STOP, T_BOUNCE, T_TURN};
enum DIRECTION {UP, DOWN, LEFT, RIGHT, X, Y, DIRECTION_SIZE};

class ShaderProgram;
class Texture;
class CompositeEntity
{
public:
	CompositeEntity();
	CompositeEntity(Entity* first);
	CompositeEntity(Entity* first, float firingDelay);
	CompositeEntity(CompositeEntity* toCopy);
	~CompositeEntity();

	Entity* getEntities();
	Vector3 getPosition();
	Vector3 getVelocity();
	Vector3 getAcceleration();
	//Vector3 getTotalSize();
	Vector3 getSizePositive();
	Vector3 getSizeNegative();
	Vector3 getScale();
	float getRotation();
	float getRotationalVelocity();
	float getTopSpeed();
	float timeSinceFiring;
	BOUNDING_TYPE getBoundingType();
	ENTITY_TYPE getEntityType();
	COLLISION_BEHAVIOR getCollisionBehavior();
	BOUNDARY_BEHAVIOR getBoundaryBehavior();
	TILE_COLLISION_BEHAVIOR getTileCollisionBehavior();
	STATE getState();
	bool getIsActive();
	bool getDoDelete();
	bool getCanCollide();
	bool getisInvincible();
	bool getOnGround();
	bool getAtBottom();
	bool getFalls();
	bool getCollUpFlag();
	bool getCollDownFlag();
	bool getCollLeftFlag();
	bool getCollRightFlag();
	bool getIsStatic();
	unsigned getLayer();

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
	void setFiringDelay(float firingDelay);
	void setRotation(float rotation);
	void setStartingRotation(float startingRotation);
	void setRotationalVelocity(float rotation);
	void setStartingRotationalVelocity(float startingRotationalVelocity);
	void setBoundingType(BOUNDING_TYPE boundingType);
	void setEntityType(ENTITY_TYPE type);
	void setCollisionBehavior(COLLISION_BEHAVIOR behavior);
	void setBoundaryBehavior(BOUNDARY_BEHAVIOR behavior);
	void setTileCollisionBehavior(TILE_COLLISION_BEHAVIOR behavior);
	void setState(STATE state);
	void setIsActive(bool isActive);
	void setCanCollide(bool canCollide);
	void setisInvincible(bool isInvincible);
	void setFalls(bool falls);
	void setEntityID(const std::string& entityID);
	void setSpacing(float spacing);
	void setSize(float size);
	void setTopSpeed(float topSpeed);
	void setLayer(unsigned layer);
	void setOnTileGround(bool onTileGround);
	void setIsStatic(bool isStatic);
	void checkPoint();

	void jump();
	void updateBounding();
	void updateBoundingRecurse(Entity* check, float offsetX = 0, float offsetY = 0, float offsetZ = 0);
	void reset();
	void resetToCheckpoint();
	bool isColliding(CompositeEntity* collidingCheck);
	bool atScreenBoundary(float gameWall, float gameCeiling);
	void move(float elapsed, float gravity = 0.0f, float frictionX = 0.0f, float frictionY = 0.0f);
	virtual void draw(ShaderProgram* program, Matrix matrix, float elapsed, float fps);
	void drawText(ShaderProgram* program, Matrix matrix, float elapsed, float fps);
	void collide(float elapsed, CompositeEntity* bouncingOffOf, COLLISION_BEHAVIOR collisionBehavior = COLLISION_BEHAVIOR_SIZE);
	void collideWithStatic(float penetration, DIRECTION direction);
	void boundaryAction(float gameWall, float gameCeiling);
	void setJumpSpeed(float jumpSpeed);
	void blink();

	void changeAnimation(ANIMATION_TYPE animationType);
	void runAnimation(float elapsed, float fps);

	virtual CompositeEntity* fire();
	void setProjectile(CompositeEntity* projectile);

	virtual void addToTimeSinceFiring(float elapsed);

	virtual CompositeEntity* logic(CompositeEntity* player, CompositeEntity* lastProjectile = nullptr);

	virtual void setDisplayText(const std::string& text);

	virtual const std::string& getDisplayText();

	virtual void setTextSheet(GLuint textSheet);

	virtual void deepCopy(CompositeEntity* toCopy);

	virtual void destroy();

	Matrix& getMatrix();

protected:
	Entity* first;

	Vector3 position;
	Vector3 startingPosition;
	Vector3 velocity;
	Vector3 startingVelocity;
	Vector3 acceleration;
	Vector3 startingAcceleration;
	Vector3 checkPointPosition;
	Vector3 sizePositive;
	Vector3 sizeNegative;
	Vector3 scale;
	Vector3 startingScale;

	Matrix modelMatrix;

	unsigned layer;
	CompositeEntity* projectile;

	GLuint textSheet;
	float rotation;
	float startingRotation;
	float rotationalVelocity;
	float startingRotationalVelocity;
	float topSpeed;
	float firingDelay;
	//For TEXT_ENTITY only:
	float spacing, size;
	float jumpSpeed;

	ENTITY_TYPE type;
	BOUNDING_TYPE boundingType;
	COLLISION_BEHAVIOR collisionBehavior;
	BOUNDARY_BEHAVIOR boundaryBehavior;
	STATE state;
	TILE_COLLISION_BEHAVIOR tileCollisionBehavior;

	bool isActive;
	bool doDelete;
	bool collideLeft, collideRight, collideTop, collideBottom;
	bool staticCollideLeft, staticCollideRight, staticCollideTop, staticCollideBottom;
	bool boundaryLeft, boundaryRight, boundaryTop, boundaryBottom;
	bool canCollide;
	bool falls;
	bool subEntitiesColliding(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat, CompositeEntity* that);
	bool isInvincible;
	bool onTileGround;
	bool doMirror;
	bool isStatic;


	void bounce(float elapsed, CompositeEntity* bouncingOffOf);
	void bounceHigh(float elapsed, CompositeEntity* bouncingOffOf);
	void stop();
	void deActivate();
	void resetFlags();
	void transformMatrix();

	std::string entityID;
	std::string text;

	float lerp(float v0, float v1, float t);
	void boundaryStop(float gameWall, float gameCeiling);
	void boundaryTurn(float gameWall, float gameCeiling);
};

#endif