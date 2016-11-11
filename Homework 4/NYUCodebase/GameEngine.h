#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "CompositeEntity.h"
#include "EnumClassHash.h"

#define PLAYER_TOP_SPEED 2.5
#define PLAYER_ACCELERATION 30
//60 fps : 1.0f/60.0f
#define FIXED_TIMESTEP 1/30.0f
#define MAX_TIMESTEP 6
#define BLINK_TIMING 0.25f
#define MAX_BLINK_COUNT 7
#define GRAVITY -50
#define STATIC_INDEX 10
#define FRAMES_PER_SECOND 6.0f

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GAME_STATE { TITLE_SCREEN, GAME_BEGIN, GAME_PLAY, GAME_END, GAME_QUIT };

class CollisionListener;
class CompositeEntity;
class ShaderProgram;
class Texture;
class Level;

class GameEngine final
{
private:
	std::vector<CollisionListener*> collisionEvents;
	std::unordered_map<std::string, std::unordered_map<unsigned, std::vector<CompositeEntity*>>> gameEntities;
	std::unordered_map<std::string, std::unordered_map<unsigned, std::vector<CompositeEntity*>>> gameEntitiesRenderingOrder;
	std::unordered_map<std::string, Level*> levels;
	CompositeEntity* playerEntity;
	std::unordered_map<std::string, Texture*> backGroundTextures;

	bool gameOver;

	int points;

	float blinkTime;
	float gameWall, gameCeiling;

	unsigned titleSelection;
	unsigned gameState;
	unsigned lives;
	unsigned enemyCount;
	unsigned blinkCount;
	unsigned mapWidth, mapHeight;

	Matrix viewMatrix;
	Matrix uIMatrix;

	std::string currentLevel;

	std::vector<GLfloat> backGroundVertices;

	//Checks collisions between dynamic entities
	void collisionCheck(float elapsed);
	//Checks collisions between dynamic entities and static entities
	void checkTileCollisions();
	void deleteFlagged();
	void handleInput(const Uint8* input, SDL_Event input2);

	void render(ShaderProgram* program, float elapsed, float fps);

	void moveAll(unsigned entityType, float x, float y, float z = 0);
	void turnAll(unsigned entityType, bool turnX, bool turnY, bool turnZ = false);

	void changeState(unsigned newState);

	void changeTitleSelection();

	void update(float elapsed, ShaderProgram* program);

	float getTilePenetrationLeft(float posX, float sizeX, int tileX);
	float getTilePenetrationRight(float posX, float sizeX, int tileX);
	float getTilePenetrationUp(float posY, float sizeY, int tileY);
	float getTilePenetrationDown(float posY, float sizeY, int tileY);

	bool tileCollisionUp(float posY, float sizeYPos, float sizeYNeg, int tileY);
	bool tileCollisionDown(float posY, float sizeYNeg, float sizeYPos, int tileY);
	bool tileCollisionLeft(float posX, float sizeXNeg, float sizeXPos, int tileX);
	bool tileCollisionRight(float posX, float sizeXPos, float sizeXNeg, int tileX);

	float checkBottomTileCollisions(CompositeEntity* entity);
	float checkTopTileCollisions(CompositeEntity* entity);
	float checkLeftTileCollisions(CompositeEntity* entity);
	float checkRightTileCollisions(CompositeEntity* entity);

	void resolveYCollisions(CompositeEntity* entity);
	void resolveXCollisions(CompositeEntity* entity);
public:
	GameEngine();
	~GameEngine();

	void addCollisionEvent(CollisionListener* collisionEvent);
	void addGameEntity(const std::string& level, CompositeEntity* entity);
	void run(float elapsed, const Uint8* input, SDL_Event input2, ShaderProgram* program);
	void addBackGroundTexture(const std::string& levelID, Texture* backGroundTexture);
	void setLevel(const std::string& levelID);
	void addLevel(Level* level);

	void start();

	bool getGameOver();
	unsigned getPoints();

};

#endif