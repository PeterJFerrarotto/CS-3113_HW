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

Vector3 Entity::getSize(){
	return size;
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

bool Entity::animationComplete(ANIMATION_TYPE animationType){
	if (animations.find(animationType) == animations.end()){
		return false;
	}
	if (currentAnimation == animationType){
		return (animations[animationType]->animationIsDone());
	}
	return false;
}

bool Entity::hasAnimation(ANIMATION_TYPE animationType){
	return (animations.find(animationType) != animations.end());
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

void Entity::setSize(float x, float y, float z){
	this->size.x = x;
	this->size.y = y;
	this->size.z = z;
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

void Entity::setTexture(Texture* texture){
	this->texture = texture;
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
	modelMatrix.Scale(scale.x, scale.y, scale.z);
	modelMatrix.Rotate(rotation);
	modelMatrix.Translate(position.x, position.y, position.z);
	if (doMirror){
		modelMatrix.Pitch(M_PI);
	}
}

void Entity::draw(ShaderProgram* program, Matrix offset, float elapsed, float fps){
	static vector<float> color;
	transformMatrix();
	offsetModelMatrix = modelMatrix * offset;
	if (sibling != nullptr){
		if (sibling->texture->getTextureLayer() < texture->getTextureLayer()){
			sibling->draw(program, offset, elapsed, fps);
		}
	}
	if (doRender){
		if (isAnimated){
			runAnimation(elapsed, fps);
		}
		if (texture->getTextureType() == UNEVEN_SPRITESHEET){
			objectVertices = texture->getObjectCoordinates();
		}
		else if (texture->getTextureType() == EVEN_SPRITESHEET || texture->getTextureType() == IMAGE){
			objectVertices = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		}
		textureCoordinates = texture->getTextureCoordinates();
		glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
		program->setModelMatrix(offsetModelMatrix);

		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, objectVertices.data());

		glEnableVertexAttribArray(program->texCoordAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());

		if (color.size() == 0){
			for (size_t i = 0; i < 6; i++){
				color.insert(color.end(), { 1.0, 1.0, 1.0, 1.0 });
			}
		}


		GLuint colorAttribute = glGetAttribLocation(program->programID, "color");
		glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, color.data());
		glEnableVertexAttribArray(colorAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
		glDisableVertexAttribArray(colorAttribute);
	}

	if (sibling != nullptr && sibling->texture->getTextureLayer() >= texture->getTextureLayer()){
		sibling->draw(program, offset, elapsed, fps);
	}
	if (child != nullptr && child->texture->getTextureLayer() >= texture->getTextureLayer()){
		child->draw(program, offsetModelMatrix, elapsed, fps);
	}
}


void Entity::updateBounding(float scaleX, float scaleY, float scaleZ){
	float tmpScaleX = scale.x * scaleX;
	float tmpScaleY = scale.y * scaleY;
	float tmpScaleZ = scale.z * scaleZ;
	if (texture != nullptr){
		switch (texture->getTextureType()){
		case IMAGE:
			size.x = tmpScaleX;
			size.y = tmpScaleY;
			break;
		case EVEN_SPRITESHEET:
			baseSize.x = texture->getTileSize();
			baseSize.y = texture->getTileSize();
			baseSize.z = 0;
			size.x = baseSize.x;
			size.y = baseSize.y;
			size.z = 0;
			break;
		case UNEVEN_SPRITESHEET:
			baseSize.x = (texture->getSpriteWidth() / texture->getSheetWidth()) / (texture->getSpriteHeight() / texture->getSheetHeight()) * texture->getSpriteSize();
			baseSize.y = baseSize.x;
			size.x = (baseSize.x)*tmpScaleX;
			size.y = (baseSize.y)*tmpScaleY;
			size.z = 0;
			break;
		default:
			throw "ERROR: UNKNOWN TEXTURE TYPE!";
			break;
		}
	}
}

bool Entity::getIsAnimated(){
	return isAnimated;
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

void Entity::addAnimation(Animation* animation){
	if (animations.find(animation->getAnimationType()) != animations.end()){
		throw "Animation already exists for this entity!";
	}
	animations[animation->getAnimationType()] = animation;
}

void Entity::setIsAnimated(bool isAnimated){
	this->isAnimated = isAnimated;
}

void Entity::startAnimation(ANIMATION_TYPE animation){
	if (isAnimated){
		currentAnimation = animation;
		if (animations.find(animation) != animations.end()){
			if (animations[animation] != nullptr){
				animations[animation]->restartAnimation();
				texture = animations[animation]->getTexture();
			}
		}
	}
	if (sibling != nullptr){
		sibling->startAnimation(animation);
	}
	if (child != nullptr){
		child->startAnimation(animation);
	}
}

void Entity::runAnimation(float elapsed, float fps){
	if (isAnimated){
		if (animations[currentAnimation] != nullptr){
			animations[currentAnimation]->runAnimation(elapsed, fps);
			texture = animations[currentAnimation]->getTexture();
		}
	}
	if (sibling != nullptr){
		sibling->runAnimation(elapsed, fps);
	}
	if (child != nullptr){
		child->runAnimation(elapsed, fps);
	}
}

void Entity::deepCopy(Entity* toCopy){
	if (toCopy->texture != nullptr){
		if (toCopy->texture->getTextureType() == UNEVEN_SPRITESHEET){
			texture = new Texture();
			texture->deepCopy(toCopy->texture);
		}
		else{
			this->texture = toCopy->texture;
		}
	}
	//if (toCopy->animations.size() != 0){
	//	for (size_t i = 0; i < toCopy->animations.size(); i++){
	//		if (toCopy->animations[i] != nullptr){
	//			animations[i] = new Animation(toCopy->animations[i]);
	//		}
	//	}
	//}
	else{
		this->texture = toCopy->texture;
	}

	this->position = toCopy->position;
	this->velocity = toCopy->velocity;
	this->acceleration = toCopy->acceleration;

	this->size = toCopy->size;
	this->scale = toCopy->scale;
	this->baseSize = toCopy->baseSize;

	this->rotation = toCopy->rotation;
	this->rotationalVelocity = toCopy->rotationalVelocity;

	this->canCollide = toCopy->canCollide;
	this->doRender = toCopy->doRender;
	this->isAnimated = toCopy->isAnimated;
	this->doMirror = toCopy->doMirror;
	this->boundingType = toCopy->boundingType;
	this->entityID = toCopy->entityID;
	if (isAnimated){
		for (unordered_map<unsigned, Animation*>::iterator itr = toCopy->animations.begin(); itr != toCopy->animations.end(); itr++){
			if (itr->second != nullptr){
				this->animations[itr->second->getAnimationType()] = new Animation(itr->second);
			}
		}
		this->currentAnimation = toCopy->currentAnimation;
		startAnimation(this->currentAnimation);
	}
	if (toCopy->sibling != nullptr){
		this->sibling = new Entity();
		this->sibling->deepCopy(toCopy->sibling);
	}
	if (toCopy->child != nullptr){
		this->child = new Entity();
		this->child->deepCopy(toCopy->child);
	}
}


void Entity::setAllDoRender(bool doRender){
	this->doRender = doRender;
	if (child != nullptr){
		child->setAllDoRender(doRender);
	}
	if (sibling != nullptr){
		sibling->setAllDoRender(doRender);
	}
}