#include "Entity.h"
using namespace std;

Entity::Entity()
{
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

void Entity::transformMatrix(){
	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);
	modelMatrix.Rotate(rotation);
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