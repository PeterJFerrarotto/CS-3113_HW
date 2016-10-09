#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "matrix.h"
#include "ShaderProgram.h"
#include <math.h>
#include "Entity.h"
#include "CollisionEvent.h"
#include "TexturedEntity.h"
#include "Vector3.h"
#include <unordered_map>
#include "EventListener.h"
#include "TickEvent.h"
#include "LogicEvent.h"
using namespace std;

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define BASE_BALL_SPEED 1.2
#define BASE_PADDLE_SPEED 1.0
#define GAME_CEILING 1.77
#define GAME_WALL 3.5

SDL_Window* displayWindow;

float ballSpeed = 1.2;
float paddleSpeed = 1.0;

GLuint loadTexture(const char* imagePath){
	SDL_Surface *surface = IMG_Load(imagePath);
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	GLenum externalFormat = GL_RGBA;
	if (surface->format->BytesPerPixel == 3) {
		externalFormat = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, externalFormat, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

void collideWithPaddle(float elapsed, Entity* ball, Entity* paddle){
	float ballPosY = ball->getPosition().y;
	float ballPosX = ball->getPosition().x;
	float ballVelX = ball->getVelocity().x;
	float ballVelY = ball->getVelocity().y;

	float paddlePosY = paddle->getPosition().y;
	float paddleBoundingY = paddle->getBounding().y;

	ballPosX -= ballVelX * elapsed;
	ballVelX = -ballVelX;
	ballPosX += ballVelX * elapsed;
	if (ballPosY > paddlePosY){
		if (ballPosY < paddlePosY + paddleBoundingY - 0.25){
			ballVelY = ballVelY;
		}
		else{
			ballVelY = ballSpeed;
		}
	}
	else if (ballPosY < paddlePosY){
		if (ballPosY > paddlePosY - paddleBoundingY + 0.25){
			ballVelY = ballVelY;
		}
		else{
			ballVelY = -ballSpeed;
		}
	}
	ball->setPosition(ballPosX, ballPosY);
	ball->setVelocity(ballVelX, ballVelY);
	ball->transformMatrix();
}

void ballWithBall(float elapsed, Entity* ball1, Entity* ball2){
	float ball1XVel = ball1->getVelocity().x;
	float ball1XPos = ball1->getPosition().x;

	float ball2XVel = ball2->getVelocity().x;
	float ball2XPos = ball2->getPosition().x;

	float ball1YVel = ball1->getVelocity().y;
	float ball1YPos = ball1->getPosition().y;

	float ball2YVel = ball2->getVelocity().y;
	float ball2YPos = ball2->getPosition().y;

	ball1->setPosition(ball2XPos, ball2YPos);
	ball1->setVelocity(ball1XVel, ball1YVel);

	ball2->setPosition(ball1XPos, ball1YPos);
	ball2->setPosition(ball2XVel, ball2YVel);
}


//Logic Event:
void paddle2AI(float elapsed, std::unordered_map<string, Entity*> entities){
	Entity* paddle = entities.at("paddle2");
	Entity* ball = entities.at("ball");
	float pPosY = paddle->getPosition().y;
	float pVelY = paddle->getVelocity().y;
	float bPosY = ball->getPosition().y;

	if (pPosY < bPosY){
		pVelY = paddleSpeed;
	}
	else if (pPosY > bPosY){
		pVelY = -paddleSpeed;
	}
	else{
		pVelY = 0;
	}

	paddle->setVelocity(0.0, pVelY);
}

//Tick Event:
void motion(float elapsed, vector<Entity*>& entities){
	for (Entity* entity : entities){
		float posX = entity->getPosition().x;
		float posY = entity->getPosition().y;
		float boundingX = entity->getBounding().x;
		float boundingY = entity->getBounding().y;
		float rotation = entity->getObjectRotation();

		float velX = entity->getVelocity().x;
		float velY = entity->getVelocity().y;
		float rotVel = entity->getObjectVelRot();

		posX += velX * elapsed;
		posY += velY * elapsed;
		rotation += rotVel * elapsed;
		if (entity->getBoundaryBehavior() == BOUNCE_PUCK){
			if (abs(posY) + boundingY >= GAME_CEILING){
				posY -= velY * elapsed;
				velY = -velY;
				posY += velY * elapsed;
			}
			if (abs(posX) + boundingX >= GAME_WALL){
				posX = 0;
			}
		}
		else if (entity->getBoundaryBehavior() == BOUNCE){
			if (abs(posY) + boundingY >= GAME_CEILING){
				posY -= velY * elapsed;
				velY = -velY;
				posY += velY * elapsed;
				if (paddleSpeed < 3)
					paddleSpeed++;
			}
			if (abs(posX) + boundingX >= GAME_WALL){
				posX -= velX * elapsed;
				velX = -velX;
				posX += velX * elapsed;
				if (paddleSpeed < 3)
					paddleSpeed++;
			}
		}
		else if (entity->getBoundaryBehavior() == STOP){
			if (abs(posY) + boundingY >= GAME_CEILING){
				posY -= velY * elapsed;
			}
			if (abs(posX) + boundingX >= GAME_WALL){
				posX -= velX * elapsed;
			}
		}
		entity->setPosition(posX, posY);
		entity->setVelocity(velX, velY);
		entity->setRotation(rotation);

		entity->transformMatrix();
	}
}

void draw(const vector<Entity*> Entities, ShaderProgram* program){
	for (vector<Entity*>::const_iterator itr = Entities.begin(); itr != Entities.end(); itr++){
		(*itr)->draw(program);
	}
}

void handleInput(const Uint8* state, Entity* p1Paddle){
	const Uint8 keyUp = state[SDL_SCANCODE_UP];
	const Uint8 keyDown = state[SDL_SCANCODE_DOWN];
	float velY = p1Paddle->getVelocity().y;
	//Input sensing for Player 1 Paddle...
	if (keyUp && !keyDown){
		velY = paddleSpeed;
	}
	else if (keyDown && !keyUp){
		velY = -paddleSpeed;
	}
	else{
		velY = 0;
	}
	p1Paddle->setVelocity(0.0, velY);
}


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	float lastFrameTicks = 0.0f;

	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vector<Entity*> gameEntities;
	unordered_map<string, Entity*> paddle2AIEntities;

	Entity* ball = new TexturedEntity();
	Entity* ball2 = new TexturedEntity();
	Entity* paddle1 = new TexturedEntity();
	Entity* paddle2 = new TexturedEntity();
	ball->setEntityID("ball");
	ball2->setEntityID("ball2");
	paddle1->setEntityID("paddle1");
	paddle2->setEntityID("paddle2");
	GLuint ballTexture = loadTexture(RESOURCE_FOLDER"Assets/ball.png");
	GLuint paddleTexture = loadTexture(RESOURCE_FOLDER"Assets/paddle.png");

	vector<float> paddleVertices = { -0.125, -0.5, 0.125, -.5, 0.125, .5, -0.125, -.5, 0.125, .5, -0.125, .5 };

	vector<float> ballVertices = { -0.125, -0.125, 0.125, -.125, 0.125, .125, -0.125, -.125, 0.125, .125, -0.125, .125 };

	ball->setTextureID(ballTexture);
	ball2->setTextureID(ballTexture);
	paddle1->setTextureID(paddleTexture);
	paddle2->setTextureID(paddleTexture);

	ball->setObjectVertices(ballVertices);

	ball2->setObjectVertices(ballVertices);

	paddle1->setObjectVertices(paddleVertices);

	paddle2->setObjectVertices(paddleVertices);

	ball->setCollisionType(SQUARE);
	ball2->setCollisionType(SQUARE);
	paddle1->setCollisionType(SQUARE);
	paddle2->setCollisionType(SQUARE);

	ball->setBoundaryBehavior(BOUNCE_PUCK);
	ball2->setBoundaryBehavior(BOUNCE);
	paddle1->setBoundaryBehavior(STOP);
	paddle2->setBoundaryBehavior(STOP);

	Matrix ballMatrix;
	Matrix ball2Matrix;
	Matrix paddle1Matrix;
	Matrix paddle2Matrix;
	ballMatrix.identity();
	ball2Matrix.identity();
	paddle1Matrix.identity();
	paddle2Matrix.identity();

	Matrix projectionMatrix;
	Matrix viewMatrix;

	ball->setModelMatrix(ballMatrix);
	ball2->setModelMatrix(ballMatrix);
	paddle1->setModelMatrix(paddle1Matrix);
	paddle2->setModelMatrix(paddle2Matrix);

	ball->setPosition(0, 0);
	ball2->setPosition(0, 1);
	paddle1->setPosition(3, 0);
	paddle2->setPosition(-3, 0);

	ball->setVelocity(1.2, 0);
	ball2->setVelocity(1.2, 0);
	paddle1->setVelocity(0, 0);
	paddle2->setVelocity(0, 0);

	ball->setBounding(0.125, 0.125);
	ball2->setBounding(0.125, 0.125);
	paddle1->setBounding(0.125, 0.5);
	paddle2->setBounding(0.125, 0.5);

	ball->transformMatrix();
	ball2->transformMatrix();
	paddle1->transformMatrix();
	paddle2->transformMatrix();

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	gameEntities.push_back(ball);
	gameEntities.push_back(ball2);
	gameEntities.push_back(paddle1);
	gameEntities.push_back(paddle2);

	paddle2AIEntities[ball->getEntityID()] = ball;
	paddle2AIEntities[paddle2->getEntityID()] = paddle2;

	CollisionEvent ballWithPaddle1;
	ballWithPaddle1.setEntity1(ball);
	ballWithPaddle1.setEntity2(paddle1);
	ballWithPaddle1.setCollisionEvent(&collideWithPaddle);

	CollisionEvent ballWithPaddle2;
	ballWithPaddle2.setEntity1(ball);
	ballWithPaddle2.setEntity2(paddle2);
	ballWithPaddle2.setCollisionEvent(&collideWithPaddle);

	CollisionEvent ball2WithPaddle1;
	ball2WithPaddle1.setEntity1(ball2);
	ball2WithPaddle1.setEntity2(paddle1);
	ball2WithPaddle1.setCollisionEvent(&collideWithPaddle);

	CollisionEvent ball2WithPaddle2;
	ball2WithPaddle2.setEntity1(ball2);
	ball2WithPaddle2.setEntity2(paddle2);
	ball2WithPaddle2.setCollisionEvent(&collideWithPaddle);
	
	CollisionEvent ball1WithBall2;
	ball1WithBall2.setEntity1(ball);
	ball1WithBall2.setEntity2(ball2);
	ball1WithBall2.setCollisionEvent(&ballWithBall);

	TickEvent motionOfElements;
	motionOfElements.setTickingEntities(gameEntities);
	motionOfElements.setEventToDo(&motion);

	LogicEvent paddle2Logic;
	paddle2Logic.setEntities(paddle2AIEntities);
	paddle2Logic.setLogic(&paddle2AI);

	EventListener<CollisionEvent> collisionListener;
	EventListener<TickEvent> tickListener;
	EventListener<LogicEvent> logicListener;

	collisionListener.addEvent(ballWithPaddle1);
	collisionListener.addEvent(ballWithPaddle2);
	collisionListener.addEvent(ball2WithPaddle1);
	collisionListener.addEvent(ball2WithPaddle2);
	//collisionListener.addEvent(ball1WithBall2);

	tickListener.addEvent(motionOfElements);

	logicListener.addEvent(paddle2Logic);

	
	//Game Loop:
	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		glClear(GL_COLOR_BUFFER_BIT);
		const Uint8 *state = SDL_GetKeyboardState(NULL);

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		handleInput(state, paddle1);
		logicListener.listen(elapsed);
		tickListener.listen(elapsed);
		collisionListener.listen(elapsed);

		draw(gameEntities, &program);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
