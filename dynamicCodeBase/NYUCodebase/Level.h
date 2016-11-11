#ifndef LEVEL_H
#define LEVEL_H
#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include "rapidxml.hpp"
#include "CompositeEntity.h"
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

class Level
{
protected:
	unsigned char** levelData;
	unsigned mapHeight, mapWidth;
	bool readHeader(std::ifstream& stream);
	bool readLayerData(std::ifstream& stream);
	bool readEntityData(std::ifstream& stream, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntities, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntitiesRenderingOrder);
	void loadLevel(const char* levelFile, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntities, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntitiesRenderingOrder);
	void placeEntity(ENTITY_TYPE type, float placeX, float placeY, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntitiesRenderingOrder);

	GLuint tileSet;
	unsigned spriteCountX, spriteCountY;
	float spriteWidth, spriteHeight;
	float tileWidth, tileHeight;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	std::vector<bool> collisionData;

	void fillCollisionData(rapidxml::xml_node<>* tileNode);

	void fillVertexArrays();
public:
	Level();
	Level(const char* stream, rapidxml::xml_document<>* doc, GLuint tileSet, unsigned spriteCountX, unsigned spriteCountY, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntitiesRenderingOrder);
	~Level();

	void draw(ShaderProgram* program);

	bool getTileDoesCollide(unsigned tileIndex);
};

#endif