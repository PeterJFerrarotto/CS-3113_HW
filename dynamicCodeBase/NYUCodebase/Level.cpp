#include "Level.h"
#include "ShaderProgram.h"
using namespace std;

Level::Level()
{
}

Level::Level(const char* stream, rapidxml::xml_document<>* doc, GLuint tileSet, unsigned spriteCountX, unsigned spriteCountY, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntitiesRenderingOrder) : tileSet(tileSet), spriteCountX(spriteCountX), spriteCountY(spriteCountY){
	fillCollisionData(doc->first_node()->first_node("Tile"));
	loadLevel(stream, gameEntities, gameEntitiesRenderingOrder);
}

Level::~Level()
{
}


bool Level::readHeader(std::ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") {
			break;
		}
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
		else if (key == "tilewidth"){
			tileWidth = atoi(value.c_str());
		}
		else if (key == "tileheight"){
			tileHeight = atoi(value.c_str());
		}
	}
	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else { // allocate our map data
		levelData = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool Level::readLayerData(std::ifstream &stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") {
			break;
		}
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						levelData[y][x] = val;
					}
					else {
						levelData[y][x] = 0;
					}
				}
			}
		}
	} return true;
}

bool Level::readEntityData(std::ifstream &stream, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntities, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntitiesRenderingOrder) {
	string line;
	ENTITY_TYPE type;
	while (getline(stream, line)) {
		if (line == "") { 
			break; 
		}
		istringstream sStream(line); 
		string key, value; 
		getline(sStream, key, '='); 
		getline(sStream, value);
		if (key == "type") {
			type = static_cast<ENTITY_TYPE>(stoi(value));
		}
		else if (key == "location") {
			istringstream lineStream(value); 
			string xPosition, yPosition; 
			getline(lineStream, xPosition, ','); 
			getline(lineStream, yPosition, ',');
			float placeX = atoi(xPosition.c_str())*tileWidth; 
			float placeY = atoi(yPosition.c_str())*-tileHeight;
		}
	} 
	return true;
}

void Level::loadLevel(const char* levelFile, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntities, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntitiesRenderingOrder){
	ifstream infile(levelFile);
	string line;
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				return;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[ObjectsLayer]") {
			readEntityData(infile, gameEntities, gameEntitiesRenderingOrder);
		}
	}
}

void Level::fillVertexArrays(){
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapHeight; x++) {
			float u = (float)(((int)levelData[y][x]) % spriteCountX) / (float)spriteCountX; 
			float v = (float)(((int)levelData[y][x]) / spriteCountX) / (float)spriteCountY;
			spriteWidth = 1.0f / (float)spriteCountX; 
			spriteHeight = 1.0f / (float)spriteCountY;
			vertexData.insert(vertexData.end(), {
				tileWidth * x, -tileHeight * y, tileWidth * x, (-tileHeight * y) - tileHeight, (tileWidth * x) + tileWidth, (-tileHeight * y) - tileHeight,
				tileWidth * x, -tileHeight * y, (tileWidth * x) + tileWidth, (-tileHeight * y) - tileHeight, (tileWidth * x) + tileWidth, -tileHeight * y
			});
			texCoordData.insert(texCoordData.end(), { u, v,
				u, v + (spriteHeight),
				u + spriteWidth, v + (spriteHeight), u, v,
				u + spriteWidth, v + (spriteHeight), u + spriteWidth, v
			});
		}
	}
}

void Level::draw(ShaderProgram* program){
	Matrix matrix;
	glBindTexture(GL_TEXTURE_2D, tileSet);
	program->setModelMatrix(matrix);

	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());

	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());

	glDrawArrays(GL_TRIANGLES, 0, vertexData.size()/6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}