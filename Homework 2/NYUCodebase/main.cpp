#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "matrix.h"
#include "ShaderProgram.h"
#include <math.h>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define BALL_BASE_SPEED 1.2
#define PADDLE_BASE_SPEED 1

SDL_Window* displayWindow;
//Define the variables containing the positional, binding, and velocity information for player 1 paddle:
float p1YPos, p1XPos, p1YPosStart, p1XPosStart, p1XBounding, p1YBounding, p1YVel, p1XVel, p1Speed;
//Define the variables containing the positional, binding, and velocity information for player 2 paddle:
float p2YPos, p2XPos, p2YPosStart, p2XPosStart, p2XBounding, p2YBounding, p2YVel, p2XVel, p2Speed;
//Define the varialbes containing the positional, binding, and velocity information for the ball:
float ballYPos, ballXPos, ballYPosStart, ballXPosStart, ballXBounding, ballYBounding, ballYVel, ballXVel, ballSpeed;

int bounces;

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

void checkBallColision(float paddleXPos, float paddleYPos, float paddleXBounding, float paddleYBounding, float paddleSpeed, float elapsed){
	if (!(ballYPos + ballYBounding < paddleYPos - paddleYBounding || ballYPos - ballYBounding > paddleYPos + paddleYBounding || ballXPos + ballXBounding < paddleXPos - paddleXBounding || ballXPos - ballXBounding > paddleXPos + paddleXBounding)){
		ballXPos -= ballXVel * elapsed;
		ballXVel = -ballXVel;
		ballXPos += ballXVel * elapsed;
		if (ballYPos > paddleYPos){
			if (ballYPos < paddleYPos + paddleYBounding - 0.25){
				ballYVel = ballYVel;
			}
			else{
				ballYVel = ballSpeed;
			}
		}
		else if (ballYPos < paddleYPos){
			if (ballYPos > paddleYPos - paddleYBounding + 0.25){
				ballYVel = ballYVel;
			}
			else{
				ballYVel = -ballSpeed;
			}
		}
	}
	if (paddleSpeed < 5){
		paddleSpeed += 0.05;
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("PONGILONG THE DING DONG DON", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
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

	GLuint ballTexture = loadTexture(RESOURCE_FOLDER"Assets/ballGrey.png");
	GLuint paddleTexture = loadTexture(RESOURCE_FOLDER"Assets/buttonDefault.png");

	Matrix player1ModelMatrix;
	Matrix player2ModelMatrix;
	Matrix backgroundMatrix1;
	Matrix backgroundMatrix2;
	Matrix backgroundMatrix3;
	Matrix ballModelMatrix;
	Matrix projectionMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	//Set the starting values for paddles and ball:
	p1YPosStart = 0;
	p1XPosStart = 3;
	p1YPos = p1YPosStart;
	p1XPos = p1XPosStart;
	p1XBounding = 0.125;
	p1YBounding = 0.5;
	p1YVel = 0;
	p1XVel = 0;
	p1Speed = 1;
	player1ModelMatrix.Translate(p1XPos, p1YPos, 0.0);

	p2YPosStart = 0;
	p2XPosStart = -3;
	p2YPos = p2YPosStart;
	p2XPos = p2XPosStart;
	p2XBounding = 0.125;
	p2YBounding = 0.5;
	p2YVel = 0;
	p2XVel = 0;
	p2Speed = 1;
	player2ModelMatrix.Translate(p2XPos, p2YPos, 0.0);

	ballYPosStart = 0;
	ballXPosStart = 0;
	ballYPos = ballYPosStart;
	ballXPos = ballXPosStart;
	ballXBounding = 0.125;
	ballYBounding = 0.125;
	ballYVel = 0;
	ballXVel = 1.3;
	ballSpeed = 1.3;
	ballModelMatrix.Translate(ballXPos, ballYPos, 0.0);

	backgroundMatrix1.identity();

	backgroundMatrix2.Translate(0, 1.5, 0.0);
	
	backgroundMatrix3.Translate(0, -1.5, 0.0);

	//Set the vertices data:
	float paddleVertices[] = { -0.125, -0.5, 0.125, -.5, 0.125, .5, -0.125, -.5, 0.125, .5, -0.125, .5 };
	float paddleTextureVertices[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	float ballVertices[] = { -0.125, -0.125, 0.125, -.125, 0.125, .125, -0.125, -.125, 0.125, .125, -0.125, .125 };
	float ballTextureVertices[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
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
		const Uint8 keyUp = state[SDL_SCANCODE_UP];
		const Uint8 keyDown = state[SDL_SCANCODE_DOWN];

		//Input sensing for Player 1 Paddle...
		if (keyUp && !keyDown){
			if (p1YPos + p1YBounding < 1.77){
				p1YVel = p1Speed;
			}
			else{
				p1YVel = 0;
			}
		}
		else if (keyDown && !keyUp){
			if (p1YPos - p1YBounding > -1.77){
				p1YVel = -p1Speed;
			}
			else{
				p1YVel = 0;
			}
		}
		else{
			p1YVel = 0;
		}

		//Logic for player 2 paddle...
		if (ballYPos < p2YPos){
			if (p2YPos - p2YBounding > -1.77){
				p2YVel = -p2Speed;
			}
			else{
				p2YVel = 0;
			}
		}
		else if (ballYPos > p2YPos){
			if (p2YPos + p2YBounding < 1.77){
				p2YVel = p2Speed;
			}
			else{
				p2YVel = 0;
			}
		}
		else{
			p2YVel = 0;
		}

		//Change positions of objects...
		p1XPos += p1XVel * elapsed;
		p1YPos += p1YVel * elapsed;
		player1ModelMatrix.identity();
		player1ModelMatrix.Translate(p1XPos, p1YPos, 0.0);

		p2XPos += p2XVel * elapsed;
		p2YPos += p2YVel * elapsed;
		player2ModelMatrix.identity();
		player2ModelMatrix.Translate(p2XPos, p2YPos, 0.0);

		ballXPos += ballXVel * elapsed;

		//Check for ball collision with paddles...
		checkBallColision(p1XPos, p1YPos, p1XBounding, p1YBounding, p1Speed, elapsed);
		checkBallColision(p2XPos, p2YPos, p2XBounding, p2YBounding, p2Speed, elapsed);

		ballYPos += ballYVel * elapsed;
		if (abs(ballYPos) >= 1.77){
			ballYPos -= ballYVel * elapsed;
			if (ballSpeed < 10){
				ballSpeed += 0.5;
			}
			if (ballYVel > 0){
				ballYVel = -ballSpeed;
			}
			else{
				ballYVel = ballSpeed;
			}

			if (ballXVel > 0){
				ballXVel = ballSpeed;
			}
			else{
				ballXVel = -ballSpeed;
			}
			ballYPos += ballYVel * elapsed;
			bounces++;
			if (bounces % 10 == 0){
				if (p1Speed < 5){
					p1Speed += 0.1;
				}
				if (p2Speed < 5){
					p2Speed += 0.1;
				}
			}
		}

		if (ballXPos >= p1XPos + p1XBounding){
			ballYPos = ballYPosStart;
			ballXPos = ballXPosStart;
			ballSpeed = BALL_BASE_SPEED;
			ballXVel = -ballSpeed;
			ballYVel = 0;
			p1Speed = PADDLE_BASE_SPEED;
			if (p2Speed < 5){
				p2Speed += 0.1;
			}
			bounces = 0;
		}
		else if (ballXPos <= p2XPos - p2XBounding){

			ballYPos = ballYPosStart;
			ballXPos = ballXPosStart;
			ballSpeed = BALL_BASE_SPEED;
			ballXVel = ballSpeed;
			ballYVel = 0;
			if (p1Speed < 5){
				p1Speed += 0.1;
			}
			p2Speed = PADDLE_BASE_SPEED;
			bounces = 0;
		}
		ballModelMatrix.identity();
		ballModelMatrix.Translate(ballXPos, ballYPos, 0.0);

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		//Draw background elements...
		glBindTexture(GL_TEXTURE_2D, paddleTexture);
		program.setModelMatrix(backgroundMatrix1);

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, paddleTextureVertices);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(backgroundMatrix2);

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, paddleTextureVertices);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(backgroundMatrix3);

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, paddleTextureVertices);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//Draw player 1 paddle
		program.setModelMatrix(player1ModelMatrix);
		
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, paddleTextureVertices);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//Draw player 2 paddle
		program.setModelMatrix(player2ModelMatrix);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, paddleTextureVertices);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//Draw ball
		glBindTexture(GL_TEXTURE_2D, ballTexture);
		program.setModelMatrix(ballModelMatrix);

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, ballTextureVertices);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
