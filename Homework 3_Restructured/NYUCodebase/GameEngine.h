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

#define GAME_CEILING 2.0
#define GAME_WALL 3.55
#define PLAYER_TOP_SPEED 2.5
#define PLAYER_ACCELERATION 5.0
//60 fps : 1.0f/60.0f
#define FIXED_TIMESTEP 1.0f/60.0f
#define MAX_TIMESTEP 6
#define BLINK_TIMING 0.25f
#define MAX_BLINK_COUNT 7
#define GRAVITY -9.8
#define STATIC_INDEX 10

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
class GameEngine final
{
private:
	std::vector<CollisionListener*> collisionEvents;
	std::unordered_map<unsigned, std::vector<CompositeEntity*>> gameEntities;
	CompositeEntity* playerEntity;

	Texture* projectileTexture;

	bool gameOver;

	int points;

	float blinkTime;

	unsigned titleSelection;
	unsigned gameState;
	unsigned lives;
	unsigned enemyCount;
	unsigned blinkCount;

	//Checks collisions between dynamic entities
	void collisionCheck(float elapsed);
	//Checks collisions between dynamic entities and static entities
	void checkStaticCollisions();
	void deleteFlagged();
	void handleInput(const Uint8* input, SDL_Event input2);

	void render(ShaderProgram* program);

	void moveAll(unsigned entityType, float x, float y, float z = 0);
	void turnAll(unsigned entityType, bool turnX, bool turnY, bool turnZ = false);

	void changeState(unsigned newState);

	void changeTitleSelection();

	void update(float elapsed, ShaderProgram* program);
public:
	GameEngine(Texture* projectileTexture);
	~GameEngine();

	void addCollisionEvent(CollisionListener* collisionEvent);
	void addGameEntity(CompositeEntity* entity);
	void updateEntities(float elapsed, const Uint8* input, SDL_Event input2, ShaderProgram* program);

	void start();

	bool getGameOver();
	unsigned getPoints();
};

#endif