#include "Entity.h"
#include "Texture.h"
#include "ShaderProgram.h"
using namespace std;

Entity::Entity()
{
	child = nullptr;
	sibling = nullptr;
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
	child = nullptr;
	sibling = nullptr;
}

void Entity::freeMemory(){
	if (child != nullptr){
		child->freeMemory();
		delete child;
		child = nullptr;
	}
	if (sibling != nullptr){
		sibling->freeMemory();
		delete sibling;
		sibling = nullptr;
	}
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

Entity* Entity::getChild(){
	return child;
}

Entity* Entity::getSibling(){
	return sibling;
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

bool Entity::getDoMirror(){
	return doMirror;
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

void Entity::setChild(Entity* child){
	this->child = child;
}

void Entity::setSibling(Entity* sibling){
	this->sibling = sibling;
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

void Entity::setDoMirror(bool doMirror){
	this->doMirror = doMirror;
}


void Entity::move(float elapsed){
	position.x += velocity.x * elapsed;
	position.y += velocity.y * elapsed;
	position.z += velocity.z * elapsed;

	velocity.x += acceleration.x * elapsed;
	velocity.y += acceleration.y * elapsed;
	velocity.z += acceleration.z * elapsed;

	rotation += rotationalVelocity * elapsed;
	if (child != nullptr){
		child->move(elapsed);
	}
	if (sibling != nullptr){
		sibling->move(elapsed);
	}
}

void Entity::addSibling(Entity* sibling){
	if (this->sibling != nullptr){
		this->sibling->addSibling(sibling);
	}
	else{
		this->sibling = sibling;
	}
}

void Entity::blink(){
	doRender = !doRender;
}

void Entity::transformMatrix(){
	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);
	modelMatrix.Scale(scale.x, scale.y, scale.z);
	modelMatrix.Rotate(rotation);
	if (doMirror){
		modelMatrix.Pitch(M_PI);
	}
}

void Entity::draw(ShaderProgram* program, Matrix offset){
	transformMatrix();
	offsetModelMatrix = modelMatrix * offset;
	if (sibling != nullptr){
		if (sibling->texture->getTextureLayer() < texture->getTextureLayer()){
			sibling->draw(program, offset);
		}
	}
	if (doRender){
		if (texture->getTextureType() == UNEVEN_SPRITESHEET){
			objectVertices = texture->getObjectCoordinates();
		}
		textureCoordinates = texture->getTextureCoordinates();
		glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
		program->setModelMatrix(offsetModelMatrix);

		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, objectVertices.data());

		glEnableVertexAttribArray(program->texCoordAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}

	if (sibling != nullptr && sibling->texture->getTextureLayer() >= texture->getTextureLayer()){
		sibling->draw(program, offset);
	}
	if (child != nullptr && child->texture->getTextureLayer() >= texture->getTextureLayer()){
		child->draw(program, offsetModelMatrix);
	}
}


void Entity::updateBounding(float scaleX, float scaleY, float scaleZ){
	float tmpScaleX = scale.x * scaleX;
	float tmpScaleY = scale.y * scaleY;
	float tmpScaleZ = scale.z * scaleZ;
	if (texture != nullptr){
		switch (texture->getTextureType()){
		case IMAGE:
			bounding.x = tmpScaleX/2;
			bounding.y = tmpScaleY/2;
			break;
		case EVEN_SPRITESHEET:
			baseSize.x = texture->getSpriteWidth();
			baseSize.y = texture->getSpriteHeight();
			baseSize.z = 0;
			bounding.x = (baseSize.x / 2) * tmpScaleX;
			bounding.y = (baseSize.y / 2) * tmpScaleY;
			bounding.z = 0;
			break;
		case UNEVEN_SPRITESHEET:
			baseSize.x = (texture->getSpriteWidth() / texture->getSheetWidth()) / (texture->getSpriteHeight() / texture->getSheetHeight()) * texture->getSpriteSize();
			baseSize.y = baseSize.x;
			bounding.x = (baseSize.x / 2)*tmpScaleX;
			bounding.y = (baseSize.y / 2)*tmpScaleY;
			bounding.z = 0;
			break;
		default:
			throw "ERROR: UNKNOWN TEXTURE TYPE!";
			break;
		}
	}
}

void Entity::blinkAll(){
	if (sibling != nullptr){
		sibling->blinkAll();
	}
	if (child != nullptr){
		child->blinkAll();
	}
	blink();
}