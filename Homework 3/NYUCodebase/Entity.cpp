#include "Entity.h"
using namespace std;

Entity::Entity()
{
	modelMatrix = Matrix();
}


Entity::~Entity()
{
}

//-----------------------
//SETTERS:
void Entity::setCanCollide(bool canCollide){
	this->canCollide = canCollide;
}

void Entity::setEntityID(const string& entityID){
	this->entityID = entityID;
}

void Entity::setModelMatrix(const Matrix& modelMatrix){
	this->modelMatrix = modelMatrix;
}

void Entity::setObjectVertices(const vector<float>& objectVertices){
	this->objectVertices = objectVertices;
}

void Entity::setCollisionType(int collisionType){
	this->collisionType = collisionType;
}

void Entity::setPosition(float posX, float posY, float posZ){
	position.x = posX;
	position.y = posY;
	position.z = posZ;
}

void Entity::setVelocity(float velX, float velY, float velZ){
	velocity.x = velX;
	velocity.y = velY;
	velocity.z = velZ;
}

void Entity::setBounding(float boundX, float boundY, float boundZ){
	bounding.x = boundX;
	bounding.y = boundY;
	bounding.z = boundZ;
}

void Entity::setRotation(float rotation){
	this->rotation = rotation;
}

void Entity::setVelRot(float velRot){
	this->velRot = velRot;
}

void Entity::setState(int state){
	this->state = state;
}

void Entity::setBoundaryBehavior(int boundaryBehavior){
	this->boundaryBehavior = boundaryBehavior;
}

void Entity::addCollisionCheck(const std::string& entityID){
	collidesWith.push_back(entityID);
}

void Entity::setEntityType(int entityType){
	this->entityType = entityType;
}

void Entity::setCollisionBehavior(int collisionBehavior){
	this->collisionBehavior = collisionBehavior;
}

void Entity::transformMatrix(){
	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);
	modelMatrix.Rotate(rotation);
}

void Entity::addCollisionCheck(int entityType){
	collidesWithTypes.push_back(entityType);
}

void Entity::resetFiringDelay(float newDelay){
	while (newDelay >= 10){
		newDelay /= 10;
	}
	firingDelay = newDelay;
}

void Entity::decreaseFiringDelay(float elapsed){
	if (firingDelay >= elapsed){
		firingDelay -= elapsed;
	}
	else{
		firingDelay = 0;
	}
}

void Entity::setRoll(float roll){
	this->roll = roll;
}

void Entity::setYaw(float yaw){
	this->yaw = yaw;
}

//-------------------------
//GETTERS:
bool Entity::getCanCollide(){
	return canCollide;
}

string& Entity::getEntityID(){
	return entityID;
}

Matrix Entity::getModelMatrix(){
	return modelMatrix;
}

Vector3 Entity::getPosition(){
	return position;
}

Vector3 Entity::getVelocity(){
	return velocity;
}

Vector3 Entity::getBounding(){
	return bounding;
}


const vector<float>& Entity::getObjectVertices(){
	return objectVertices;
}

float Entity::getObjectRotation(){
	return rotation;
}

float Entity::getObjectVelRot(){
	return velRot;
}

int Entity::getState(){
	return state;
}

int Entity::getCollisionType(){
	return collisionType;
}

int Entity::getBoundaryBehavior(){
	return boundaryBehavior;
}

const std::vector<std::string>& Entity::getCollisionList(){
	return collidesWith;
}

int Entity::getCollisionBehavior(){
	return collisionBehavior;
}

int Entity::getEntityType(){
	return entityType;
}

const std::vector<int>& Entity::getTypeCollisionList(){
	return collidesWithTypes;
}

bool Entity::getCanFire(){
	return firingDelay == 0;
}

float Entity::getRoll(){
	return roll;
}

float Entity::getYaw(){
	return yaw;
}

//-------------------------------
//Pseudo-Pure virtual function calls:

void Entity::setTextureID(GLuint textureID){
	return;
}

void Entity::draw(ShaderProgram* program){
	return;
}

void Entity::setTextureVertices(const std::vector<float>& textureCoordinates){
	return;
}

void Entity::setIndex(int index){
	return;
}

void Entity::setSpriteCount(int spriteCountX, int spriteCountY){
	return;
}

void Entity::setDefaultTextureVertices(){
	return;
}

const std::vector<float>& Entity::getTextureVertices(){
	return{};
}
