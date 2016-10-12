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
#include "CollisionEventListener.h"
#include "SpriteEntity.h"
#include "LogicEvent.h"
#include <time.h>
#include "TextCharacter.h"
using namespace std;


#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GAME_STATE { TITLE_SCREEN, GAME_BEGIN, GAME_PLAY, GAME_END, GAME_QUIT };

#define GAME_CEILING 1.0
#define GAME_WALL 3.55
#define GRAVITY 9.8
#define HI 5.0
#define LO 1.0
#define HI_R 359

SDL_Window* displayWindow;

unordered_map<int, vector<Entity*>> gameEntities;
vector<float> textVertices = { -5, -5, 5, -5, 5, 5, -5, -5, 5, 5, -5, 5 };

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


void drawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing, float startingX, float startingY) {
	Matrix modelMatrix;
	modelMatrix.identity();
	modelMatrix.Translate(startingX, startingY, 0.0);
	float texture_size = 1.0 / 16.0f; std::vector<float> vertexData; std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y,
			texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size,
		});
	}
	program->setModelMatrix(modelMatrix);
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void draw(ShaderProgram* program){
	for (size_t i = 0; i < ENTITY_TYPE_COUNT; i++){
		for (vector<Entity*>::const_iterator itr = gameEntities[i].begin(); itr != gameEntities[i].end(); itr++){
			if ((*itr)->getRender())
				(*itr)->draw(program);
		}
	}
}



void tickTextCharacters(int type, float elapsed){
	for (vector<Entity*>::iterator eval = gameEntities[type].begin(); eval != gameEntities[type].end(); eval++){
		if ((*eval)->getState() == DESTROYED){
			eval = gameEntities[type].erase(eval);
			if (eval == gameEntities[type].end()){
				break;
			}
		}
		float posX = (*eval)->getPosition().x;
		float posY = (*eval)->getPosition().y;

		float velX = (*eval)->getVelocity().x;
		float velY = (*eval)->getVelocity().y;

		float rotation = (*eval)->getObjectRotation();
		float velRot = (*eval)->getObjectVelRot();

		posY += velY * elapsed;
		posX += velX * elapsed;
		rotation += velRot * elapsed;
		if (rotation >= 2 * M_PI){
			rotation -= 2 * M_PI;
		}

		if (velY > -HI){
			velY -= GRAVITY * elapsed;
		}

		if (velY * (*eval)->getVelocity().y < 0){
			if (velX == 0){
				velX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.25));
				if (rotation >= M_PI){
					velX = -velX;
				}
			}
			if (velRot == 0){
				if (rotation < M_PI){
					velRot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/M_PI));
				}
				else{
					velRot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/M_PI));
				}
			}
		}
		if (velY < -HI){
			velY = -HI;
		}
		(*eval)->incrementTimeAlive(elapsed);
		if ((*eval)->getTimeAlive() >= 5.0){
			(*eval)->setState(DESTROYED);
		}

		if (abs(posY) + (*eval)->getBounding().y >= GAME_CEILING){
			if (posY <= -GAME_CEILING){
				posY = -GAME_CEILING;
				if (velY < 0){
					velY = -velY * 0.75;
				}
				if (velY <= 0.7){
					velY = 0;
					velX = 0;
					velRot = 0;
				}
			}
			posY += velY * elapsed;
			velRot = 0;
		}

		(*eval)->setPosition(posX, posY);
		(*eval)->setVelocity(velX, velY);

		(*eval)->setRotation(rotation);
		(*eval)->setVelRot(velRot);
	}
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

	GLuint textTextureID = loadTexture(RESOURCE_FOLDER"Assets/font2.png");

	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Matrix projectionMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -1.0f, 1.0f, -1.0f, 1.0f);

	TickEvent evalCharacters;
	evalCharacters.setEventToDo(&tickTextCharacters);
	evalCharacters.setTickingEntities(TEXT);

	EventListener<TickEvent> tickListener;
	tickListener.addEvent(evalCharacters);
	//Game Loop:
	SDL_Event event;
	int gameState = TITLE_SCREEN;
	bool done = false;
	bool pressed = false;
	int titleSelection = 0;
	float dA = 0.0;
	float dB = 0.0;
	float dC = 0.0;
	float dD = 0.0;
	float dE = 0.0;
	float dF = 0.0;
	float dG = 0.0;
	float dH = 0.0;
	float dI = 0.0;
	float dJ = 0.0;
	float dK = 0.0;
	float dL = 0.0;
	float dM = 0.0;
	float dN = 0.0;
	float dO = 0.0;
	float dP = 0.0;
	float dQ = 0.0;
	float dR = 0.0;
	float dS = 0.0;
	float dT = 0.0;
	float dU = 0.0;
	float dV = 0.0;
	float dW = 0.0;
	float dX = 0.0;
	float dY = 0.0;
	float dZ = 0.0;
	float d1 = 0.0;
	float d2 = 0.0;
	float d3 = 0.0;
	float d4 = 0.0;
	float d5 = 0.0;
	float d6 = 0.0;
	float d7 = 0.0;
	float d8 = 0.0;
	float d9 = 0.0;
	float d0 = 0.0;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		int toCapital = 0;
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		glClear(GL_COLOR_BUFFER_BIT);
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		string toCreate;

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		if (state[SDL_SCANCODE_RSHIFT] || state[SDL_SCANCODE_LSHIFT]){
			toCapital = 32;
		}
		if (state[SDL_SCANCODE_A]){
			if (dA <= 0.0){
				toCreate += (char)('a' - toCapital);
				dA = 0.2;
			}
		}

		if (state[SDL_SCANCODE_B]){
			if (dB <= 0.0){
				toCreate += (char)('b' - toCapital);
				dB = 0.2;
			}
		}

		if (state[SDL_SCANCODE_C]){
			if (dC <= 0.0){
				toCreate += (char)('c' - toCapital);
				dC = 0.2;
			}
		}

		if (state[SDL_SCANCODE_D]){
			if (dD <= 0.0){
				toCreate += (char)('d' - toCapital);
				dD = 0.2;
			}
		}

		if (state[SDL_SCANCODE_E]){
			if (dE <= 0.0){
				toCreate += (char)('e' - toCapital);
				dE = 0.2;
			}
		}

		if (state[SDL_SCANCODE_F]){
			if (dF <= 0.0){
				toCreate += (char)('f' - toCapital);
				dF = 0.2;
			}
		}

		if (state[SDL_SCANCODE_G]){
			if (dG <= 0.0){
				toCreate += (char)('g' - toCapital);
				dG = 0.2;
			}
		}

		if (state[SDL_SCANCODE_H]){
			if (dH <= 0.0){
				toCreate += (char)('h' - toCapital);
				dH = 0.2;
			}
		}

		if (state[SDL_SCANCODE_I]){
			if (dI <= 0.0){
				toCreate += (char)('i' - toCapital);
				dI = 0.2;
			}
		}

		if (state[SDL_SCANCODE_J]){
			if (dJ <= 0.0){
				toCreate += (char)('j' - toCapital);
				dJ = 0.2;
			}
		}

		if (state[SDL_SCANCODE_K]){
			if (dK <= 0.0){
				toCreate += (char)('k' - toCapital);
				dK = 0.2;
			}
		}

		if (state[SDL_SCANCODE_L]){
			if (dL <= 0.0){
				toCreate += (char)('l' - toCapital);
				dL = 0.2;
			}
		}

		if (state[SDL_SCANCODE_M]){
			if (dM <= 0.0){
				toCreate += (char)('m' - toCapital);
				dM = 0.2;
			}
		}

		if (state[SDL_SCANCODE_N]){
			if (dN <= 0.0){
				toCreate += (char)('n' - toCapital);
				dN = 0.2;
			}
		}

		if (state[SDL_SCANCODE_O]){
			if (dO <= 0.0){
				toCreate += (char)('o' - toCapital);
				dO = 0.2;
			}
		}

		if (state[SDL_SCANCODE_P]){
			if (dP <= 0.0){
				toCreate += (char)('p' - toCapital);
				dP = 0.2;
			}
		}

		if (state[SDL_SCANCODE_Q]){
			if (dQ <= 0.0){
				toCreate += (char)('q' - toCapital);
				dQ = 0.2;
			}
		}

		if (state[SDL_SCANCODE_R]){
			if (dR <= 0.0){
				toCreate += (char)('r' - toCapital);
				dR = 0.2;
			}
		}

		if (state[SDL_SCANCODE_S]){
			if (dS <= 0.0){
				toCreate += (char)('s' - toCapital);
				dS = 0.2;
			}
		}

		if (state[SDL_SCANCODE_T]){
			if (dT <= 0.0){
				toCreate += (char)('t' - toCapital);
				dT = 0.2;
			}
		}

		if (state[SDL_SCANCODE_U]){
			if (dU <= 0.0){
				toCreate += char('u' - toCapital);
				dU = 0.2;
			}
		}

		if (state[SDL_SCANCODE_V]){
			if (dV <= 0.0){
				toCreate += char('v' - toCapital);
				dV = 0.2;
			}
		}

		if (state[SDL_SCANCODE_W]){
			if (dW <= 0.0){
				toCreate += (char)('w' - toCapital);
				dW = 0.2;
			}
		}

		if (state[SDL_SCANCODE_X]){
			if (dX <= 0.0){
				toCreate += (char)('x' - toCapital);
				dX = 0.2;
			}
		}

		if (state[SDL_SCANCODE_Y]){
			if (dY <= 0.0){
				toCreate += (char)('y' - toCapital);
				dY = 0.2;
			}
		}

		if (state[SDL_SCANCODE_Z]){
			if (dZ <= 0.0){
				toCreate += (char)('z' - toCapital);
				dZ = 0.2;
			}
		}

		if (state[SDL_SCANCODE_1]){
			if (d1 <= 0.0){
				toCreate += (char)('1');
				d1 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_2]){
			if (d2 <= 0.0){
				toCreate += (char)('2');
				d2 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_3]){
			if (d3 <= 0.0){
				toCreate += (char)('3');
				d3 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_4]){
			if (d4 <= 0.0){
				toCreate += (char)('4');
				d4 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_5]){
			if (d5 <= 0.0){
				toCreate += (char)('5');
				d5 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_6]){
			if (d6 = 0.0){
				toCreate += (char)('6');
				d6 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_7]){
			if (d7 <= 0.0){
				toCreate += (char)('7');
				d7 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_8]){
			if (d8 <= 0.0){
				toCreate += (char)('8');
				d8 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_9]){
			if (d9 <= 0.0){
				toCreate += (char)('9');
				d9 = 0.2;
			}
		}

		if (state[SDL_SCANCODE_0]){
			if (d0 <= 0.0){
				toCreate += (char)('0');
				d0 = 0.2;
			}
		}

		dA -= elapsed;
		dB -= elapsed;
		dC -= elapsed;
		dD -= elapsed;
		dE -= elapsed;
		dF -= elapsed;
		dG -= elapsed;
		dH -= elapsed;
		dI -= elapsed;
		dJ -= elapsed;
		dK -= elapsed;
		dL -= elapsed;
		dM -= elapsed;
		dN -= elapsed;
		dO -= elapsed;
		dP -= elapsed;
		dQ -= elapsed;
		dR -= elapsed;
		dS -= elapsed;
		dT -= elapsed;
		dU -= elapsed;
		dV -= elapsed;
		dW -= elapsed;
		dX -= elapsed;
		dY -= elapsed;
		dZ -= elapsed;
		d1 -= elapsed;
		d2 -= elapsed;
		d3 -= elapsed;
		d4 -= elapsed;
		d5 -= elapsed;
		d6 -= elapsed;
		d7 -= elapsed;
		d8 -= elapsed;
		d9 -= elapsed;
		d0 -= elapsed;

		for (char i : toCreate){
			float posY = -0.9;
			float posX;
			switch (i){
			case '1':
			case '!':
				posX = -3.2;
				break;

			case 'q':
			case 'Q':
				posX = -3.1;
				break;

			case 'a':
			case 'A':
				posX = -3.0;
				break;

			case 'z':
			case 'Z':
				posX = -2.5;
				break;

			case '2':
			case '@':
				posX = -2.45;
				break;

			case 'w':
			case 'W':
				posX = -2.4;
				break;

			case 's':
			case 'S':
				posX = -2.3;
				break;

			case 'x':
			case 'X':
				posX = -1.8;
				break;

			case '3':
			case '#':
				posX = -1.75;
				break;
				
			case 'e':
			case 'E':
				posX = -1.7;
				break;

			case 'd':
			case 'D':
				posX = -1.6;
				break;

			case 'c':
			case 'C':
				posX = -1.1;
				break;

			case '4':
			case '$':
				posX = -1.05;
				break;

			case 'r':
			case 'R':
				posX = -1.0;
				break;

			case 'f':
			case 'F':
				posX = -0.9;
				break;

			case 'v':
			case 'V':
				posX = -0.4;
				break;

			case '5':
			case '%':
				posX = -0.35;
				break;

			case 't':
			case 'T':
				posX = -0.3;
				break;

			case 'g':
			case 'G':
				posX = -0.2;
				break;

			case 'b':
			case 'B':
				posX = 0.3;
				break;

			case '6':
			case '^':
				posX = 0.35;
				break;

			case 'y':
			case 'Y':
				posX = 0.4;
				break;

			case 'h':
			case 'H':
				posX = 0.5;
				break;

			case 'n':
			case 'N':
				posX = 1.0;
				break;
				
			case '7':
			case '&':
				posX = 1.05;
				break;

			case 'u':
			case 'U':
				posX = 1.1;
				break;

			case 'j':
			case 'J':
				posX = 1.2;
				break;

			case 'm':
			case 'M':
				posX = 1.7;
				break;

			case '8':
			case '*':
				posX = 1.75;
				break;

			case 'i':
			case 'I':
				posX = 1.8;
				break;
				
			case 'k':
			case 'K':
				posX = 1.9;
				break;

			case '9':
			case '(':
				posX = 1.95;
				break;

			case 'o':
			case 'O':
				posX = 2.4;
				break;

			case 'l':
			case 'L':
				posX = 2.5;
				break;

			case '0':
			case ')':
				posX = 2.55;
				break;

			case 'p':
			case 'P':
				posX = 3.0;
				break;

			default:
				posX = 0;
				break;
			}
			Entity* character = new TextCharacter(i, textTextureID, 0.2);
			character->setPosition(posX, posY);
			srand(time(NULL));
			float velY = LO + static_cast <float> (rand())/(static_cast <float> (RAND_MAX/HI));
			float rotation = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / HI_R))) * M_PI/180;
			float velX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.25));
			if (rotation >= M_PI){
				velX = -velX;
			}
			character->setVelocity(velX, velY);
			character->setRotation(rotation);
			character->setVelRot(0);
			character->setBounding(0.25, 0.5, 0);
			character->setRender(true);
			gameEntities[TEXT].push_back(character);
		}
		tickListener.listen(elapsed);
		draw(&program);
		SDL_GL_SwapWindow(displayWindow);
	}
	return 0;
}