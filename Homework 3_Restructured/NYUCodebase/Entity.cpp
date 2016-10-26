#include "Entity.h"
#include "Texture.h"
#include "ShaderProgram.h"
using namespace std;

Entity::Entity()
{
	next = nullptr;
	position.x = 0;
	position.y = 0;
	position.z = 0;

	velocity.x = 0;
	velocity.y = 0;
	velocity.z = 0;
}

Entity::~Entity(){
	freeMemory();
}

Entity::Entity(const std::string& entityID, Texture* texture){
	this->entityID = entityID;
	this->texture = texture;
	next = nullptr;
}

void Entity::freeMemory(){
	if (next != nullptr){
		next->freeMemory();
		delete next;
		next = nullptr;
	}

	delete texture;
	texture = nullptr;
}

const string& Entity::getEntityID(){
	return entityID;
}

Vector3 Entity::getPosition(){
	return position;
}

Vector3 Entity::getVelocity(){
	return velocity;
}

Vector3 Entity::getAcceleration(){
	return acceleration;
}

Vector3 Entity::getBounding(){
	return bounding;
}

Vector3 Entity::getScale(){
	return scale;
}

float Entity::getRotation(){
	return rotation;
}

float Entity::getRotationalVelocity(){
	return rotationalVelocity;
}

Entity* Entity::getNext(){
	return next;
}

BOUNDING_TYPE Entity::getBoundingType(){
	return boundingType;
}

bool Entity::getCanCollide(){
	return canCollide;
}

bool Entity::getDoRender(){
	return doRender;
}


void Entity::setEntityID(const std::string& entityID){
	this->entityID = entityID;
}

void Entity::setPosition(float x, float y, float z){
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}

void Entity::setVelocity(float x, float y, float z){
	this->velocity.x = x;
	this->velocity.y = y;
	this->velocity.z = z;
}

void Entity::setAcceleration(float x, float y, float z){
	this->acceleration.x = x;
	this->acceleration.y = y;
	this->acceleration.z = z;
}

void Entity::setBounding(float x, float y, float z){
	this->bounding.x = x;
	this->bounding.y = y;
	this->bounding.z = z;
}

void Entity::setRotation(float rotation){
	this->rotation = rotation;
}

void Entity::setRotationalVelocity(float rotationalVelocity){
	this->rotationalVelocity = rotationalVelocity;
}

void Entity::setScale(float x, float y, float z){
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}

void Entity::setNext(Entity* next){
	this->next = next;
}

void Entity::setObjectVertices(const vector<GLfloat>& objectVertices){
	this->objectVertices = objectVertices;
}

void Entity::setBoundingType(BOUNDING_TYPE boundingType){
	this->boundingType = boundingType;
}

void Entity::setCanCollide(bool canCollide){
	this->canCollide = canCollide;
}

void Entity::setDoRender(bool doRender){
	this->doRender = doRender;
}


void Entity::move(float elapsed){
	position.x += velocity.x * elapsed;
	position.y += velocity.y * elapsed;
	position.z += velocity.z * elapsed;

	velocity.x += acceleration.x * elapsed;
	velocity.y += acceleration.y * elapsed;
	velocity.z += acceleration.z * elapsed;

	rotation += rotationalVelocity * elapsed;
	if (next != nullptr){
		next->move(elapsed);
	}
}

void Entity::addEntity(Entity* toAdd){
	if (next == nullptr){
		next = toAdd;
	}
	else{
		next->addEntity(toAdd);
	}
}

void Entity::blink(){
	doRender = !doRender;
}

void Entity::transformMatrix(Vector3 positionOffset, Vector3 scaleOffset, float rotationOffset){
	float posX = position.x + positionOffset.x;
	float posY = position.y + positionOffset.y;
	float posZ = position.z + positionOffset.z;

	float scaleX = scale.x * scaleOffset.x;
	float scaleY = scale.y * scaleOffset.y;
	float scaleZ = scale.z * scaleOffset.z;

	float toRotate = rotation + rotationOffset;

	modelMatrix.identity();
	modelMatrix.Translate(posX, posY, posZ);
	modelMatrix.Scale(scaleX, scaleY, scaleZ);
	modelMatrix.Rotate(toRotate);
}

void Entity::draw(ShaderProgram* program, Vector3 positionOffset, Vector3 scaleOffset, float rotationOffset){
	if (next != nullptr){
		if (next->texture->getTextureLayer() > texture->getTextureLayer() || texture->getTextureLayer() == 0){
			next->draw(program, positionOffset, scaleOffset, rotationOffset);
		}
	}
	if (doRender){
		transformMatrix(positionOffset, scaleOffset, rotationOffset);
		if (texture->getTextureType() == TEXT || texture->getTextureType() == UNEVEN_SPRITESHEET){
			objectVertices = texture->getObjectCoordinates();
		}
		textureCoordinates = texture->getTextureCoordinates();
		glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
		program->setModelMatrix(modelMatrix);

		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, objectVertices.data());

		glEnableVertexAttribArray(program->texCoordAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}

	if (next != nullptr){
		next->draw(program, positionOffset, scaleOffset, rotationOffset);
	}
}


void Entity::updateBounding(){
	if (texture != nullptr){
		switch (texture->getTextureType()){
		case IMAGE:
			bounding.x = scale.x/2;
			bounding.y = scale.y/2;
			break;
		case EVEN_SPRITESHEET:
			baseSize.x = texture->getSpriteWidth();
			baseSize.y = texture->getSpriteHeight();
			baseSize.z = 0;
			bounding.x = (baseSize.x / 2) * scale.x;
			bounding.y = (baseSize.y / 2) * scale.y;
			bounding.z = 0;
			break;
		case UNEVEN_SPRITESHEET:
			baseSize.x = (texture->getSpriteWidth() / texture->getSheetWidth()) / (texture->getSpriteHeight() / texture->getSheetHeight()) * texture->getSpriteSize();
			baseSize.y = baseSize.x;
			bounding.x = (baseSize.x / 2)*scale.x;
			bounding.y = (baseSize.y / 2)*scale.y;
			bounding.z = 0;
			break;
		case TEXT:
			bounding.x = 0;
			bounding.y = 0;
			bounding.z = 0;
			break;
		default:
			throw "ERROR: UNKNOWN TEXTURE TYPE!";
			break;
		}
	}
}

void Entity::blinkAll(){
	if (next != nullptr){
		next->blinkAll();
	}
	blink();
}