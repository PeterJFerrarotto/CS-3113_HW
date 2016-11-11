#ifndef XML_HELPER_H
#define XML_HELPER_H
#include "GameEngine.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "dirent.h"
#include "CompositeEntity.h"
#include "Entity.h"
#include "Texture.h"
#include "EnemyShip.h"
#include "Animation.h"
#include <string>
#include <unordered_map>
using namespace std;
using namespace rapidxml;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

inline GLuint loadTexture(const char* imagePath){
	static unordered_map<const char*, GLuint> loadedTextures;
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		loadedTextures[imagePath] = textureID;
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
		Texture* tex = new Texture(textureID, spriteCountX, spriteCountY, index, layer);
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
	if (entityType == ACTOR_ENEMY){
		tmp = new EnemyShip();
	}
	else if (entityType == TITLE_TEXT_ENTITY || entityType == GAME_TEXT_ENTITY || entityType == POINTS_INDICATOR){
		tmp = new TextEntity();
	}
	else{
		tmp = new CompositeEntity();
	}
	string entityID = rootNode->first_node("EntityID")->value();
	float posX = 0, posY = 0, posZ = 0;
	float velX = 0, velY = 0, velZ = 0;
	float accX = 0, accY = 0, accZ = 0;
	float scaleX = 1, scaleY = 1, scaleZ = 1;
	float rotation = 0;
	float rotationalVelocity = 0;
	float firingDelay = 0;
	bool canCollide = false, isInvincible = false, isActive = true, falls = false;
	float topSpeed = 1.0;
	float textSize = 1.0, textSpacing = 1.0;
	unsigned layer = 0;
	string displayText = "";
	string textSheetDirectory;
	GLuint textSheet = 1;
	BOUNDING_TYPE boundingType = SQUARE;
	COLLISION_BEHAVIOR collisionBehavior = NOTHING;
	BOUNDARY_BEHAVIOR boundaryBehavior = BOUND_STOP;
	tmp->setEntityType(entityType);
	tmp->setEntityID(entityID);

	xml_node<>* detailsNode = rootNode->first_node("Details");
	if (detailsNode == nullptr){
		throw "Error: Empty Entity tag!!";
	}
	xml_node<>* currentDetail = detailsNode->first_node("firingDelay");
	if (currentDetail != nullptr){
		firingDelay = stof(currentDetail->value());
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

	currentDetail = detailsNode->first_node("Position_Offset");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("left") != nullptr){
			posX = -GAME_WALL + stof(currentDetail->first_attribute("left")->value());
		}
		else if (currentDetail->first_attribute("right") != nullptr){
			posX = GAME_WALL - stof(currentDetail->first_attribute("right")->value());
		}

		if (currentDetail->first_attribute("top") != nullptr){
			posY = GAME_CEILING - stof(currentDetail->first_attribute("top")->value());
		}
		else if (currentDetail->first_attribute("bottom") != nullptr){
			posY = -(GAME_CEILING - stof(currentDetail->first_attribute("bottom")->value()));
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

	currentDetail = detailsNode->first_node("Rotation");
	if (currentDetail != nullptr){
		rotation = (M_PI/180) * stof(currentDetail->value());
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

	currentDetail = detailsNode->first_node("Layer");
	if (currentDetail != nullptr){
		layer = stoi(currentDetail->value());
	}

	tmp->setStartingPosition(posX, posY, posZ);
	tmp->setStartingVelocity(velX, velY, velZ);
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
	if (rootNode->first_node("subEntities") != nullptr){
		tmp->setEntities(enrichEntityInformation(rootNode->first_node("subEntities")->first_node("First")));
	}
	tmp->setLayer(layer);
	tmp->updateBounding();

	xml_node<>* projectileTextureNode = rootNode->first_node("Projectile");
	if (projectileTextureNode != nullptr){
		Texture* projectileTexture = enrichTextureInformation(projectileTextureNode->first_node("Texture"));
		tmp->setProjectileTexture(projectileTexture);
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
			if (doc->first_node()->first_attribute("isBackGroundFile") != nullptr){
				xml_node<>* textureNode = doc->first_node()->first_node();
				do{
					Texture* backGroundTexture = enrichTextureInformation(textureNode);
					unsigned levelID = stoi(textureNode->first_attribute("levelID")->value());
					engine.addBackGroundTexture(levelID, backGroundTexture);
					textureNode = textureNode->next_sibling();
				} while (textureNode != nullptr);
			}
			else{
				xml_node<>* entityNode = doc->first_node()->first_node();
				do{
					CompositeEntity* toAdd = enrichXMLData(entityNode);
					engine.addGameEntity(toAdd);
					entityNode = entityNode->next_sibling();
				} while (entityNode != nullptr);
			}
		}
	}
}

#endif