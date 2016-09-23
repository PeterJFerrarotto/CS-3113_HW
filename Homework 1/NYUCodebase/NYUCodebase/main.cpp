#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <math.h>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define WALKING_SPEED 0.001
#define RUNNING_SPEED 0.005
#define TURN_SPEED 0.05

SDL_Window* displayWindow;

GLuint LoadTexture(const char* imagePath) {


	SDL_Surface *surface = IMG_Load(imagePath);
	GLuint textureID;	
	glPixelStorei(GL_PACK_ROW_LENGTH, surface->w);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->w);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->pitch / surface->format->BytesPerPixel);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);

	//glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
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
	float playerPosX = 0.0f;
	float playerSpeedX = 0.0f;
	float playerPosY = 0.0f;
	float playerSpeedY = 0.0f;
	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint texture1 = LoadTexture("assets/backdrop.png");
	GLuint texture2 = LoadTexture("assets/F5S4.png");
	GLuint texture3 = LoadTexture("assets/eyeball.png");

	//Yes, I used gravel, a space ship, and an eyeball. I didn't know what three sprites to use, so I just chose the first three random words
	//that came to mind. Which were gravel, spaceship, and eyeball. Don't ask me why.

	//Some people say the best ideas for games come from random ideas.

	//This seems more like the idea for a nightmare.

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix modelMatrixBackground;
	Matrix modelMatrixEyeball;
	Matrix viewMatrix;

	modelMatrixEyeball.setPosition(1.0, -2.0, 0.0);

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float moveSpeed;
	glUseProgram(program.programID);



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
		const Uint8 upKey = state[SDL_SCANCODE_UP];
		const Uint8 downKey = state[SDL_SCANCODE_DOWN];
		const Uint8 leftKey = state[SDL_SCANCODE_LEFT];
		const Uint8 rightKey = state[SDL_SCANCODE_RIGHT];
		const Uint8 keyA = state[SDL_SCANCODE_A];
		const Uint8 keyD = state[SDL_SCANCODE_D];
		const Uint8 keyW = state[SDL_SCANCODE_W];
		const Uint8 keyS = state[SDL_SCANCODE_S];

		if (state[SDL_SCANCODE_RETURN]) {
			modelMatrix.identity();
			playerPosY = 0.0f;
			playerPosX = 0.0f;
			playerSpeedY = 0.0f;
			playerSpeedX = 0.0f;
		}
		else if (state[SDL_SCANCODE_ESCAPE]){
			done = true;
		}

		if (!state[SDL_SCANCODE_RSHIFT] && !state[SDL_SCANCODE_LSHIFT]){
			moveSpeed = WALKING_SPEED;
		}
		else{
			moveSpeed = RUNNING_SPEED;
		}

		if (leftKey && !rightKey){
			if (playerSpeedX > -1){
				if (playerSpeedX < 0){
					playerSpeedX = 0;
				}
				playerSpeedX += elapsed * TURN_SPEED;
			}
			playerPosX += playerSpeedX * M_PI/180;
		}
		else if (rightKey && !leftKey){
			if (playerSpeedX < 1){
				if (playerSpeedX > 0){
					playerSpeedX = 0;
				}
				playerSpeedX -= elapsed * TURN_SPEED;
			}
			playerPosX += playerSpeedX * M_PI/180;
		}
		else if (!rightKey && !leftKey){
			if (playerSpeedX > 0 && playerSpeedX >= 0.04 * elapsed){
				playerSpeedX -= elapsed * 0.04;
				playerPosX += playerSpeedX * M_PI/180;
			}
			else if (playerSpeedX < 0 && playerSpeedX <= -0.04 * elapsed){
				playerSpeedX += elapsed * 0.04;
				playerPosX += playerSpeedX * M_PI/180;
			}
			else
			{
				playerSpeedX = 0;
			}
		}
		
		if (upKey && !downKey){
			if (playerSpeedY < 5){
				playerSpeedY += elapsed * moveSpeed;
			}
			playerPosY += playerSpeedY;
		}
		else if (downKey && !upKey){
			if (playerSpeedY > -5){
				playerSpeedY -= elapsed * moveSpeed;
			}
			playerPosY += playerSpeedY;
		}
		else if (!downKey && !upKey){
			if (playerSpeedY > 0 && playerSpeedY >= 0.001 * elapsed){
				playerSpeedY -= 0.001 * elapsed;
				playerPosY += playerSpeedY;
			}
			else if (playerSpeedY < 0 && playerSpeedY <= -0.001 * elapsed){
				playerSpeedY += 0.001 * elapsed;
				playerPosY += playerSpeedY;
			}
			else{
				playerSpeedY = 0;
			}
		}

		if (keyA && !keyD){
			modelMatrix.Rotate(25 * (3.14 / 180) * elapsed);
		}
		else if (keyD && !keyA){
			modelMatrix.Rotate(-25 * (3.14 / 180) * elapsed);
		}

		modelMatrix.Translate(0.0f, playerSpeedY, 0.0);
		modelMatrix.Rotate(playerSpeedX * M_PI / 180);

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		program.setModelMatrix(modelMatrixBackground);

		glBindTexture(GL_TEXTURE_2D, texture1);

		float backGroundVertices[] = { -5, -3, 5, -3, 5, 3, -5, -3, 5, 3, -5, 3 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, backGroundVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float backGroundTexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, backGroundTexCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(modelMatrix);

		glBindTexture(GL_TEXTURE_2D, texture2);

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(modelMatrixEyeball);

		glBindTexture(GL_TEXTURE_2D, texture3);

		float eyeBallVertices[] = { -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, eyeBallVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float eyeBallTexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, eyeBallTexCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
