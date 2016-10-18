#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <vector>
#include <unordered_map>

#define GAME_CEILING 2.0
#define GAME_WALL 3.55
#define PLAYER_TOP_SPEED 2.5
#define PLAYER_ACCELERATION 5.0
#define TIME_STEP

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

	void collisionCheck(float elapsed);
	void deleteFlagged();
	void handleInput(const Uint8* input);

	void render(ShaderProgram* program);
public:
	GameEngine(Texture* projectileTexture);
	~GameEngine();

	void addCollisionEvent(CollisionListener* collisionEvent);
	void addGameEntity(CompositeEntity* entity);
	void updateEntities(float elapsed, const Uint8* input, ShaderProgram* program);

	bool getGameOver();
};

#endif