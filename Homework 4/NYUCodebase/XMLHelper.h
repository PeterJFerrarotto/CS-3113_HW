#ifndef XML_HELPER_H
#define XML_HELPER_H
#include "GameEngine.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "dirent.h"
#include "CompositeEntity.h"
#include "Entity.h"
#include "Texture.h"
#include "Animation.h"
#include "Level.h"
#include <string>
#include <unordered_map>
#include <sstream>
using namespace std;
using namespace rapidxml;

//#define _CRT_SECURE_NO_WARNINGS
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

static unordered_map<unsigned, CompositeEntity*> entityTypes;
static unordered_map<GLuint, pair<int, int>> loadedTextureSizes;

static const char* txtExt = ".txt";
static const char* xmlExt = ".xml";
static const char* tsxExt = ".tsx";
static const char* pngExt = ".png";
static const char* separator = "/";

inline GLuint loadTexture(const char* imagePath){
	static unordered_map<string, GLuint> loadedTextures;
	if (loadedTextures.find(imagePath) == loadedTextures.end()){
		SDL_Surface *surface = IMG_Load(imagePath);
		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		GLenum externalFormat = GL_RGBA;
		if (surface->format->BytesPerPixel == 3) {
			externalFormat = GL_RGB;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, externalFormat, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		char* x = new char[360];
		strcpy(x, imagePath);
		loadedTextures[x] = textureID;
		loadedTextureSizes[textureID] = { surface->w, surface->h };
		return textureID;
	}
	return loadedTextures[imagePath];
}


inline xml_document<> * loadXMLFile(const char* filePath){
	file<>* xmlFile = new file<>(filePath);
	xml_document<>* doc = new xml_document<>;
	doc->parse<0>(xmlFile->data());
	return doc;
}

inline Texture* enrichTextureInformation(xml_node<>* textureNode){
	if (textureNode == nullptr){
		return nullptr;
	}
	static unordered_map<string, pair<GLuint, xml_document<>*>> loadedFiles;
	static unordered_map<string, GLuint> loadedFilesWithoutXML;
	string directory = RESOURCE_FOLDER;
	GLuint textureID;
	xml_node<>* detailsNode;
	TEXTURE_TYPE textureType = static_cast<TEXTURE_TYPE>(stoi(textureNode->first_attribute("type")->value()));
	if (textureType == IMAGE){
		detailsNode = textureNode->first_node("Image");
		directory = directory + detailsNode->first_attribute("Directory")->value();
		string imageName = detailsNode->first_attribute("imageName")->value();
		string imagePath = directory + imageName;
		int layer = stoi(textureNode->first_attribute("layer")->value());

		if (loadedFilesWithoutXML.find(imagePath) == loadedFilesWithoutXML.end()){
			textureID = loadTexture(imagePath.c_str());
			loadedFilesWithoutXML[imagePath] = textureID;
		}
		else{
			textureID = loadedFilesWithoutXML[imagePath];
		}
		Texture* tex = new Texture(textureID, layer);
		return tex;
	}
	else if (textureType == EVEN_SPRITESHEET){
		detailsNode = textureNode->first_node("EvenSpritesheet");
		directory = directory + detailsNode->first_attribute("Directory")->value();
		string sheetName = detailsNode->first_attribute("sheetName")->value();
		string sheetPath = directory + sheetName;
		int layer = stoi(textureNode->first_attribute("layer")->value());
		unsigned index = stoi(detailsNode->first_attribute("index")->value());
		unsigned spriteCountX = stoi(detailsNode->first_attribute("spriteCountX")->value());
		unsigned spriteCountY = stoi(detailsNode->first_attribute("spriteCountY")->value());
		if (loadedFilesWithoutXML.find(sheetPath) == loadedFilesWithoutXML.end()){
			textureID = loadTexture(sheetPath.c_str());
			loadedFilesWithoutXML[sheetPath] = textureID;
		}
		else{
			textureID = loadedFilesWithoutXML[sheetPath];
		}
		unsigned sheetWidth = loadedTextureSizes[textureID].first;
		unsigned sheetHeight = loadedTextureSizes[textureID].second;
		Texture* tex = new Texture(textureID, spriteCountX, spriteCountY, sheetWidth, sheetHeight, index, layer);
		return tex;
	}
	else if (textureType == UNEVEN_SPRITESHEET){
		detailsNode = textureNode->first_node("UnevenSpritesheet");
		string sheetName = detailsNode->first_attribute("sheetName")->value();
		string atlasName = detailsNode->first_attribute("atlasName")->value();
		directory = directory + detailsNode->first_attribute("Directory")->value();
		string key = directory + atlasName + sheetName;
		int layer = stoi(textureNode->first_attribute("layer")->value());
		xml_document<>* xmlDoc;
		if (loadedFiles.find(key) == loadedFiles.end()){
			string sheetPath = directory + sheetName;
			string atlasPath = directory + atlasName;
			textureID = loadTexture(sheetPath.c_str());
			xmlDoc = loadXMLFile(atlasPath.c_str());
			loadedFiles[key] = make_pair(textureID, xmlDoc);
		}
		else{
			textureID = loadedFiles[key].first;
			xmlDoc = loadedFiles[key].second;
		}
		string spriteName = detailsNode->first_attribute("spriteName")->value();
		float size = stof(detailsNode->first_attribute("size")->value());

		Texture* tex = new Texture(textureID, xmlDoc, spriteName, size, layer);
		return tex;
	}
	else{
		throw "Uknown texture type in XML file!";
	}
}

inline Animation* enrichAnimationInformation(xml_node<>* animationNode){
	bool loop = false;
	ANIMATION_TYPE animationType = ANIMATION_IDLE;
	unsigned startingIndex = 0, endingIndex = 0;
	if (animationNode->first_attribute("loop") != nullptr){
		loop = stoi(animationNode->first_attribute("loop")->value()) == 1;
	}

	if (animationNode->first_attribute("type") != nullptr){
		animationType = static_cast<ANIMATION_TYPE>(stoi(animationNode->first_attribute("type")->value()));
	}

	if (animationNode->first_attribute("startingIndex") != nullptr){
		startingIndex = stoi(animationNode->first_attribute("startingIndex")->value());
	}

	if (animationNode->first_attribute("endingIndex") != nullptr){
		endingIndex = stoi(animationNode->first_attribute("endingIndex")->value());
	}

	Animation* anim = new Animation(animationType, startingIndex, endingIndex);
	anim->setDoLoop(loop);
	Texture* tex;
	if (animationNode->first_node("Texture") != nullptr){
		tex = enrichTextureInformation(animationNode->first_node("Texture"));
		anim->setTexture(tex);
	}
	return anim;
}

inline Entity* enrichEntityInformation(xml_node<>* entityNode){
	if (entityNode == nullptr){
		return nullptr;
	}
	Texture* tex = nullptr;
	if (entityNode->first_node("Texture") != nullptr){
		tex = enrichTextureInformation(entityNode->first_node("Texture"));
	}
	string entityID = entityNode->first_node("EntityID")->value();
	Entity* entity = new Entity(entityID, tex);
	entity->setIsAnimated(false);
	if (entityNode->first_node("Animations") != nullptr){
		entity->setIsAnimated(true);
		xml_node<>* animationNode = entityNode->first_node("Animations")->first_node("Animation");
		do{
			Animation* animation = enrichAnimationInformation(animationNode);
			entity->addAnimation(animation);
			if (animationNode->first_attribute("Starting") != nullptr){
				entity->startAnimation(animation->getAnimationType());
			}
			animationNode = animationNode->next_sibling();
		} while (animationNode != nullptr);
	}
	xml_node<>* detailsNode = entityNode->first_node("Details");
	if (detailsNode == nullptr){
		throw "Empty subEntity tag!";
	}

	float posX = 0, posY = 0, posZ = 0;
	float velX = 0, velY = 0, velZ = 0;
	float accX = 0, accY = 0, accZ = 0;
	float scaleX = 1, scaleY = 1, scaleZ = 1;
	float rotation = 0;
	float rotationalVelocity = 0;
	bool canCollide = false;
	bool doRender = true;
	bool doMirror = false;
	BOUNDING_TYPE boundingType = SQUARE;

	xml_node<>* currentDetail = detailsNode->first_node("Position");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			posX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			posY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			posZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Velocity");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			velX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			velY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			velZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Acceleration");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			accX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			accY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			accZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Scale");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			scaleX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			scaleY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			scaleZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Rotation");
	if (currentDetail != nullptr){
		rotation = (M_PI / 180) * stof(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("rotationalVelocity");
	if (currentDetail != nullptr){
		rotationalVelocity = (M_PI / 180) * stof(currentDetail->value());
	}
	else{
		rotationalVelocity = 0;
	}

	currentDetail = detailsNode->first_node("canCollide");
	if (currentDetail != nullptr){
		canCollide = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("doRender");
	if (currentDetail != nullptr){
		doRender = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("boundingType");
	if (currentDetail != nullptr){
		boundingType = static_cast<BOUNDING_TYPE>(stoi(currentDetail->value()));
	}

	currentDetail = detailsNode->first_node("doMirror");
	if (currentDetail != nullptr){
		doMirror = stoi(currentDetail->value()) == 1;
	}

	entity->setPosition(posX, posY, posZ);
	entity->setVelocity(velX, velY, velZ);
	entity->setAcceleration(accX, accY, accZ);
	entity->setScale(scaleX, scaleY, scaleZ);
	entity->setRotation(rotation);
	entity->setRotationalVelocity(rotationalVelocity);
	entity->setCanCollide(canCollide);
	entity->setDoRender(doRender);
	entity->setBoundingType(boundingType);
	entity->setDoMirror(doMirror);

	if (entityNode->first_node("Sibling") != nullptr){
		entity->setSibling(enrichEntityInformation(entityNode->first_node("Sibling")));
	}

	if (entityNode->first_node("Child") != nullptr){
		entity->setChild(enrichEntityInformation(entityNode->first_node("Child")));
	}

	return entity;
}

inline CompositeEntity* enrichXMLData(xml_node<>* rootNode){
	ENTITY_TYPE entityType = static_cast<ENTITY_TYPE>(stoi(rootNode->first_node("Type")->value()));
	CompositeEntity* tmp;
	tmp = new CompositeEntity();
	
	float posX = 0, posY = 0, posZ = 0;
	float velX = 0, velY = 0, velZ = 0;
	float accX = 0, accY = 0, accZ = 0;
	float scaleX = 1, scaleY = 1, scaleZ = 1;
	float rotation = 0;
	float rotationalVelocity = 0;
	float firingDelay = 0;
	bool canCollide = false, isInvincible = false, isActive = true, falls = false, isStatic = false;
	float topSpeed = 1.0;
	float textSize = 1.0, textSpacing = 1.0;
	unsigned layer = 0;
	float jumpSpeed = 0;
	string displayText = "";
	string textSheetDirectory;
	GLuint textSheet = 1;
	BOUNDING_TYPE boundingType = SQUARE;
	COLLISION_BEHAVIOR collisionBehavior = NOTHING;
	TILE_COLLISION_BEHAVIOR tileCollisionBehavior = T_STOP;
	BOUNDARY_BEHAVIOR boundaryBehavior = BOUND_STOP;
	tmp->setEntityType(entityType);
	if (entityType != WARP_ENTITY){
		xml_node<>* detailsNode = rootNode->first_node("Details");
		if (detailsNode == nullptr){
			throw "Error: Empty Entity tag!!";
		}
		xml_node<>* currentDetail = detailsNode->first_node("firingDelay");
		if (currentDetail != nullptr){
			firingDelay = stof(currentDetail->value());
		}

		currentDetail = detailsNode->first_node("tileCollisionBehavior");
		if (currentDetail != nullptr){
			tileCollisionBehavior = static_cast<TILE_COLLISION_BEHAVIOR>(stoi(currentDetail->value()));
		}

		currentDetail = detailsNode->first_node("Position");
		if (currentDetail != nullptr){
			if (currentDetail->first_attribute("x") != nullptr){
				posX = stof(currentDetail->first_attribute("x")->value());
			}
			if (currentDetail->first_attribute("y") != nullptr){
				posY = stof(currentDetail->first_attribute("y")->value());
			}
			if (currentDetail->first_attribute("z") != nullptr){
				posZ = stof(currentDetail->first_attribute("z")->value());
			}
		}

		currentDetail = detailsNode->first_node("Velocity");
		if (currentDetail != nullptr){
			if (currentDetail->first_attribute("x") != nullptr){
				velX = stof(currentDetail->first_attribute("x")->value());
			}
			if (currentDetail->first_attribute("y") != nullptr){
				velY = stof(currentDetail->first_attribute("y")->value());
			}
			if (currentDetail->first_attribute("z") != nullptr){
				velZ = stof(currentDetail->first_attribute("z")->value());
			}
		}

		currentDetail = detailsNode->first_node("Acceleration");
		if (currentDetail != nullptr){
			if (currentDetail->first_attribute("x") != nullptr){
				accX = stof(currentDetail->first_attribute("x")->value());
			}
			if (currentDetail->first_attribute("y") != nullptr){
				accY = stof(currentDetail->first_attribute("y")->value());
			}
			if (currentDetail->first_attribute("z") != nullptr){
				accZ = stof(currentDetail->first_attribute("z")->value());
			}
		}

		currentDetail = detailsNode->first_node("Scale");
		if (currentDetail != nullptr){
			if (currentDetail->first_attribute("x") != nullptr){
				scaleX = stof(currentDetail->first_attribute("x")->value());
			}
			if (currentDetail->first_attribute("y") != nullptr){
				scaleY = stof(currentDetail->first_attribute("y")->value());
			}
			if (currentDetail->first_attribute("z") != nullptr){
				scaleZ = stof(currentDetail->first_attribute("z")->value());
			}
		}

		currentDetail = detailsNode->first_node("topSpeed");
		if (currentDetail != nullptr){
			topSpeed = stof(currentDetail->value());
		}

		currentDetail = detailsNode->first_node("jumpSpeed");
		if (currentDetail != nullptr){
			jumpSpeed = stof(currentDetail->value());
		}

		currentDetail = detailsNode->first_node("Rotation");
		if (currentDetail != nullptr){
			rotation = (M_PI / 180) * stof(currentDetail->value());
		}

		currentDetail = detailsNode->first_node("rotationalVelocity");
		if (currentDetail != nullptr){
			rotationalVelocity = (M_PI / 180) * stof(currentDetail->value());
		}
		else{
			rotationalVelocity = 0;
		}

		currentDetail = detailsNode->first_node("canCollide");
		if (currentDetail != nullptr){
			canCollide = stoi(currentDetail->value()) == 1;
		}

		currentDetail = detailsNode->first_node("isActive");
		if (currentDetail != nullptr){
			isActive = stoi(currentDetail->value()) == 1;
		}

		currentDetail = detailsNode->first_node("falls");
		if (currentDetail != nullptr){
			falls = stoi(currentDetail->value()) == 1;
		}

		currentDetail = detailsNode->first_node("isInvincible");
		if (currentDetail != nullptr){
			isInvincible = stoi(currentDetail->value()) == 1;
		}

		currentDetail = detailsNode->first_node("CollisionBehavior");
		if (currentDetail != nullptr){
			collisionBehavior = static_cast<COLLISION_BEHAVIOR>(stoi(currentDetail->value()));
		}

		currentDetail = detailsNode->first_node("BoundaryBehavior");
		if (currentDetail != nullptr){
			boundaryBehavior = static_cast<BOUNDARY_BEHAVIOR>(stoi(currentDetail->value()));
		}

		currentDetail = detailsNode->first_node("BoundingType");
		if (currentDetail != nullptr){
			boundingType = static_cast<BOUNDING_TYPE>(stoi(currentDetail->value()));
		}

		currentDetail = detailsNode->first_node("DisplayText");
		if (currentDetail != nullptr){
			displayText = currentDetail->value();

			textSize = stof(currentDetail->first_attribute("size")->value());
			textSpacing = stof(currentDetail->first_attribute("spacing")->value());
		}

		currentDetail = detailsNode->first_node("TextSheet");
		if (currentDetail != nullptr){
			textSheetDirectory = RESOURCE_FOLDER;
			textSheetDirectory = textSheetDirectory + currentDetail->first_attribute("sheetPath")->value() + currentDetail->first_attribute("sheetName")->value();
			textSheet = loadTexture(textSheetDirectory.c_str());
		}

		currentDetail = detailsNode->first_node("isStatic");
		if (currentDetail != nullptr){
			isStatic = stoi(currentDetail->value()) == 1;
		}

		currentDetail = detailsNode->first_node("Layer");
		if (currentDetail != nullptr){
			layer = stoi(currentDetail->value());
		}
	}
	else{
		canCollide = true;
		falls = false;
		isStatic = true;
		boundingType = SQUARE;
	}

	tmp->setStartingPosition(posX, posY, posZ);
	tmp->setStartingVelocity(velX, velY, velZ);
	tmp->setTopSpeed(topSpeed);
	tmp->setStartingScale(scaleX, scaleY, scaleZ);
	tmp->setStartingAcceleration(accX, accY, accZ);
	tmp->setStartingRotation(rotation);
	tmp->setStartingRotationalVelocity(rotationalVelocity);
	tmp->setFiringDelay(firingDelay);
	tmp->setCanCollide(canCollide);
	tmp->setIsActive(isActive);
	tmp->setFalls(falls);
	tmp->setisInvincible(isInvincible);
	tmp->setCollisionBehavior(collisionBehavior);
	tmp->setBoundaryBehavior(boundaryBehavior);
	tmp->setBoundingType(boundingType);
	tmp->setDisplayText(displayText);
	tmp->setSize(textSize);
	tmp->setSpacing(textSpacing);
	tmp->setTextSheet(textSheet);
	tmp->setTileCollisionBehavior(tileCollisionBehavior);
	tmp->setJumpSpeed(jumpSpeed);
	tmp->setIsStatic(isStatic);
	if (rootNode->first_node("subEntities") != nullptr){
		tmp->setEntities(enrichEntityInformation(rootNode->first_node("subEntities")->first_node("First")));
	}
	tmp->setLayer(layer);
	tmp->updateBounding();

	xml_node<>* projectileTextureNode = rootNode->first_node("Projectile");
	if (projectileTextureNode != nullptr){
		CompositeEntity* compProjectile = new CompositeEntity();
		Entity* projectile = new Entity();
		projectile->setEntityID("Projectile");
		if (entityType == ACTOR_PLAYER){
			compProjectile->setEntityType(PLAYER_PROJECTILE);
		}
		else{
			compProjectile->setEntityType(ENEMY_PROJECTILE);
		}
		projectile->setPosition(0, 0, 0);
		projectile->setVelocity(0, 0, 0);
		projectile->setAcceleration(0, 0, 0);
		projectile->setScale(1, 1, 1);
		projectile->setBoundingType(SQUARE);
		projectile->setRotation(0);
		projectile->setCanCollide(true);
		projectile->setDoRender(true);

		compProjectile->setCanCollide(true);
		compProjectile->setFalls(false);
		compProjectile->setBoundingType(SQUARE);
		compProjectile->setEntityType(PLAYER_PROJECTILE);
		compProjectile->setCollisionBehavior(DESTROY);
		compProjectile->setBoundaryBehavior(BOUND_DESTROY);
		compProjectile->setScale(16, 16);

		if (projectileTextureNode->first_node("Texture") != nullptr){
			projectile->setIsAnimated(false);
			Texture* projectileTexture = enrichTextureInformation(projectileTextureNode->first_node("Texture"));
			projectile->setTexture(projectileTexture);
		}
		else if (projectileTextureNode->first_node("Animations") != nullptr && projectileTextureNode->first_node("Animations")->first_node("Animation") != nullptr){
			projectile->setIsAnimated(true);
			xml_node<>* animationNode = projectileTextureNode->first_node("Animations")->first_node("Animation");
			do{
				Animation* animation = enrichAnimationInformation(animationNode);
				projectile->addAnimation(animation);
				if (animationNode->first_attribute("Starting") != nullptr){
					projectile->startAnimation(animation->getAnimationType());
				}
				animationNode = animationNode->next_sibling();
			} while (animationNode != nullptr);
		}
		else{
			delete projectile;
			projectile = nullptr;
			throw "Error: No animation or texture for projectile!!";
		}
		compProjectile->setEntities(projectile);
		compProjectile->setProjectile(nullptr);
		tmp->setProjectile(compProjectile);
	}
	else{
		tmp->setProjectile(nullptr);
	}
	return tmp;
}

inline void loadXMLData(GameEngine& engine){
	DIR *dirp;
	struct dirent *dp;
	char fileName[360];
	char fileDirec[360] = RESOURCE_FOLDER"Assets/XML/";
	if ((dirp = opendir("./Assets/XML")) == NULL){
		throw "Could not find current directory!";
	}
	while ((dp = readdir(dirp)) != NULL){
		strcpy_s(fileName, dp->d_name);
		if (fileName[0] != '.' && fileName[1] != '.'){
			strcpy_s(fileDirec, RESOURCE_FOLDER"Assets/XML/");
			strcat_s(fileDirec, fileName);
			xml_document<>* doc = loadXMLFile(fileDirec);
			xml_node<>* entityNode = doc->first_node()->first_node();
			do{
				CompositeEntity* toAdd = enrichXMLData(entityNode);
				if (entityTypes.find(toAdd->getEntityType()) != entityTypes.end()){
					throw "Entity type already exists!";
				}
				entityTypes[toAdd->getEntityType()] = toAdd;
				entityNode = entityNode->next_sibling();
			} while (entityNode != nullptr);
		}
	}
}

inline void readEntityData(std::ifstream &stream, Level* level, GameEngine& engine) {
	string line;
	ENTITY_TYPE type;
	CompositeEntity* entity = new CompositeEntity();
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
			if (entityTypes.find(type) == entityTypes.end()){
				throw "Entity type undefined!";
			}
			entity->deepCopy(entityTypes[type]);
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			float placeX = (atoi(xPosition.c_str()) + 0.5)*level->getTileSize();
			float placeY = (atoi(yPosition.c_str()) - 0.5)*-(level->getTileSize());
			entity->setStartingPosition(placeX, placeY);
		}
		else if (key == "text"){
			string text = value;
			entity->setDisplayText(text);
		}
		else if (key == "size"){
			float size = stof(value);
			entity->setSize(size);
		}
		else if (key == "spacing"){
			float spacing = stof(value);
			entity->setSpacing(spacing);
		}
		else if (key == "isStatic"){
			bool isStatic = value == "true";
			entity->setIsStatic(isStatic);
		}
		else if (key == "renderLayer"){
			unsigned layer = stoi(value);
			entity->setLayer(layer);
		}
		else if (key == "warpDestination"){
			string destination = value;
			entity->setWarpDestination(destination);
		}
	}
	if (type == ACTOR_PLAYER){
		level->setPlayerEntity(entity);
	}
	engine.addGameEntity(level->getLevelID(), entity);
}

inline Level* loadLevel(const char* levelFile, const char* tileData, const char* levelName, const char* spriteSheetLocation, GameEngine& engine){
	Level* level = new Level();
	ifstream infile(levelFile);
	string line;
	level->setLevelID(levelName);
	//level->setTileSize(16);
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!level->readHeader(levelFile, infile)) {
				return nullptr;
			}
		}
		else if (line == "[layer]") {
			level->readLayerData(infile);
		}
		else if (line == "[ObjectsLayer]") {
			readEntityData(infile, level, engine);
		}
	}
	xml_document<>* doc = loadXMLFile(tileData);
	xml_node<>* node = doc->first_node();
	GLuint sheet = loadTexture(spriteSheetLocation);
	level->setTileSet(sheet);
	level->fillSpriteSheetData(node);
	return level;
}

inline void loadLevelData(GameEngine& engine){
	char spritesFolder[] = RESOURCE_FOLDER"Assets/Levels/Spritesheets/";
	char levelsFolder[] = RESOURCE_FOLDER"Assets/Levels/";
	char txtFile[360];
	char xmlFile[360];
	char spriteSheetName[360];
	char levelName[360];
	char spriteSheetDirec[360];
	xml_document<>* levelIndex = loadXMLFile(RESOURCE_FOLDER"Assets/Levels/level_index.xml");
	if (!levelIndex){
		throw "Level index not found!";
	}
	xml_node<>* levelNode = levelIndex->first_node();
	if (levelNode == nullptr){
		throw "Level index file empty!";
	}
	levelNode = levelNode->first_node("Level");
	if (levelNode == nullptr){
		throw "No level data!";
	}
	do{
		strcpy_s(spriteSheetDirec, spritesFolder);
		if (levelNode->first_attribute("id") == nullptr){
			throw "ID field missing for level!";
		}
		strcpy_s(levelName, levelNode->first_attribute("id")->value());
		strcpy_s(txtFile, levelsFolder);
		strcat_s(txtFile, levelName);
		strcat_s(txtFile, separator);
		strcat_s(txtFile, levelName);
		strcat_s(txtFile, txtExt);
		if (levelNode->first_attribute("spriteSheet") == nullptr){
			throw "Tile sheet name missing for level!";
		}
		strcpy_s(spriteSheetName, levelNode->first_attribute("spriteSheet")->value());
		strcpy_s(spriteSheetDirec, spritesFolder);
		strcat_s(spriteSheetDirec, spriteSheetName);

		strcpy_s(xmlFile, spritesFolder);
		strcat_s(xmlFile, spriteSheetName);
		strcat_s(xmlFile, tsxExt);

		strcat_s(spriteSheetDirec, pngExt);
		Level* level = loadLevel(txtFile, xmlFile, levelName, spriteSheetDirec, engine);
		engine.addLevel(level);
		if (levelNode->first_attribute("startingLevel") != nullptr){
			engine.setLevel(levelName);
		}
		levelNode = levelNode->next_sibling();
	} while (levelNode != nullptr);
}

#endif