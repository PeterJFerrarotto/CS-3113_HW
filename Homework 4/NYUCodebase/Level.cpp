#include "Level.h"
#include "ShaderProgram.h"
#include "Tile.h"
using namespace std;
using namespace rapidxml;

Level::Level()
{
}

Level::~Level()
{
}

void Level::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / tileWidth); 
	*gridY = (int)(-worldY / tileHeight);
}

unsigned Level::getLayerCount(){
	return vertexData.size();
}

bool Level::readHeader(const char* levelName, std::ifstream &stream) {
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
	return mapWidth != -1 && mapHeight != -1;
}

bool Level::readLayerData(std::ifstream &stream) {
	unsigned layer = levelData.size();
	levelData[layer] = new unsigned char*[mapHeight];
	for (int i = 0; i < mapHeight; ++i){
		levelData[layer][i] = new unsigned char[mapWidth];
	}
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
						levelData[layer][y][x] = val;
					}
					else {
						levelData[layer][y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

void Level::fillVertexArrays(){
	for (size_t i = 0; i < levelData.size(); i++){
		for (int y = 0; y < mapHeight; y++) {
			for (int x = 0; x < mapWidth; x++) {
				if ((int)(levelData[i][y][x]) != 0){
					int levelIndex = (int)levelData[i][y][x]-1;
					float u = (float)(((int)levelData[i][y][x] - 1) % spriteCountX) / (float)spriteCountX;
					float v = (float)(((int)levelData[i][y][x] - 1) / spriteCountX) / (float)spriteCountY;
					float spriteWidth = 1.0 / (float)spriteCountX;
					float spriteHeight = 1.0 / (float)spriteCountY;
					//tileSize = 1.6;
					vertexData[i].insert(vertexData[i].end(), {
						tileSize * x, -tileSize * y, tileSize * x, (-tileSize * y) - tileSize, (tileSize * x) + tileSize, (-tileSize * y) - tileSize,
						tileSize * x, -tileSize * y, (tileSize * x) + tileSize, (-tileSize * y) - tileSize,
						(tileSize * x) + tileSize, -tileSize * y
					});
					texCoordData[i].insert(texCoordData[i].end(), { u, v,
						u, v + (spriteHeight),
						u + spriteWidth, v + (spriteHeight), u, v,
						u + spriteWidth, v + (spriteHeight), u + spriteWidth, v
					});
				}
			}
		}
	}
}

void Level::draw(ShaderProgram* program, unsigned layer){
	if (layer >= levelData.size())
		return;
	Matrix matrix;
	glBindTexture(GL_TEXTURE_2D, tileSet);
	program->setModelMatrix(matrix);

	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData[layer].data());

	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData[layer].data());

	glDrawArrays(GL_TRIANGLES, 0, vertexData[layer].size()/2);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


float Level::getTileWidth(){
	return tileWidth;
}

float Level::getTileHeight(){
	return tileHeight;
}

const std::string& Level::getLevelID(){
	return levelID;
}

void Level::setLevelID(const std::string& levelID){
	this->levelID = levelID;
}

void Level::setTileSet(GLuint tileSet){
	this->tileSet = tileSet;
}

void Level::setTileSize(float tileSize){
	this->tileSize = tileSize;
}

Tile* Level::getTile(unsigned layer, int gridX, int gridY){
	unsigned char tileIndex = levelData[layer][gridY][gridX];
	if (collisionData.find(tileIndex) == collisionData.end() || tileIndex == 0){
		return nullptr;
	}
	return collisionData[tileIndex - 1];
}

float Level::getTileSize(){
	return tileSize;
}

float Level::getMapHeight(){
	return mapHeight * tileSize;
}

float Level::getMapLength(){
	return mapWidth * tileSize;
}

void Level::fillCollisionData(xml_node<>* tileNode){
	if (tileNode == nullptr){
		return;
	}
	static const char* collideCheck = "canCollide";
	static const char* trueCheck = "true";
	static const char* typeCheck = "tileType";
	unsigned tileIndex = stoi(tileNode->first_attribute("id")->value());
	float x = 0, y = 0;
	float width = 0, height = 0;
	bool canCollide = false;
	TILE_TYPE tileType = SOLID;
	xml_node<>* propertyNode = tileNode->first_node("properties");
	if (propertyNode == nullptr){
		//If properties added using Tiled's "Edit Tile Collision" tool, they will be contained within an "object" tag
		propertyNode = tileNode->first_node()->first_node("properties");
		if (propertyNode == nullptr){
			throw "Properties not found!";
		}
	}
	propertyNode = propertyNode->first_node("property");
	do{
		if (propertyNode == nullptr){
			break;
		}
		if (strcmp(collideCheck, propertyNode->first_attribute("name")->value()) == 0){
			canCollide = strcmp(trueCheck, propertyNode->first_attribute("value")->value()) == 0;
		}
		else if (strcmp(typeCheck, propertyNode->first_attribute("name")->value()) == 0){
			tileType = static_cast<TILE_TYPE>(stoi(propertyNode->first_attribute("value")->value()));
		}
		propertyNode = propertyNode->next_sibling("property");
	} while (propertyNode != nullptr);

	xml_node<>* objectNode = tileNode->first_node();
	if (objectNode->first_node("object") != nullptr){
		objectNode = objectNode->first_node("object");
		x = stof(objectNode->first_attribute("x")->value());
		y = stof(objectNode->first_attribute("y")->value());
		width = stof(objectNode->first_attribute("width")->value());
		height = stof(objectNode->first_attribute("height")->value());
	}
	if (collisionData.find(tileIndex) != collisionData.end()){
		throw "Repeated tile index!";
	}
	collisionData[tileIndex] = new Tile();
	collisionData[tileIndex]->height = height;
	collisionData[tileIndex]->width = width;
	collisionData[tileIndex]->x = x;
	collisionData[tileIndex]->y = y;
	collisionData[tileIndex]->canCollide = canCollide;
	collisionData[tileIndex]->tileType = tileType;
	fillCollisionData(tileNode->next_sibling());
}

void Level::fillSpriteSheetData(xml_node<>* tileNode){
	xml_node<>* imageNode = tileNode->first_node("image");
	if (imageNode != nullptr){
		sheetHeight = stoi(imageNode->first_attribute("height")->value());
		sheetWidth = stoi(imageNode->first_attribute("width")->value());
		spriteCountX = sheetWidth / tileWidth;
		spriteCountY = sheetHeight / tileHeight;
	}
	if (tileNode->first_attribute("id") == nullptr){
		tileNode = tileNode->first_node("tile");
	}
	fillCollisionData(tileNode);
	fillVertexArrays();
}