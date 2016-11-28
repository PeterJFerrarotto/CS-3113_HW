#include "Animation.h"
#include "Texture.h"

Animation::Animation(Animation* toCopy){
	deepCopy(toCopy);
}

Animation::Animation(ANIMATION_TYPE animationType, unsigned startingIndex, unsigned endingIndex) : animationType(animationType), startingIndex(startingIndex), endingIndex(endingIndex), currentIndex(startingIndex)
{
}

Animation::~Animation()
{
}

bool Animation::animationIsDone(){
	return !loop && currentIndex == endingIndex;
}

void Animation::updateTexture(){
	currentTexture->setIndex(currentIndex);
}

ANIMATION_TYPE Animation::getAnimationType(){
	return animationType;
}

Texture* Animation::getTexture(){
	return currentTexture;
}

bool Animation::getDoLoop(){
	return loop;
}

void Animation::setTexture(Texture* texture){
	currentTexture = texture;
}

void Animation::setDoLoop(bool loop){
	this->loop = loop;
}

void Animation::runAnimation(float elapsed, float fps){
	animationElapsed += elapsed;
	if (animationElapsed > 1.0 / fps) {
		currentIndex++;
		if (currentIndex > endingIndex){
			currentIndex = loop ? startingIndex : endingIndex;
			timesRun++;
		}
		updateTexture();
		animationElapsed = 0;
	}
}

void Animation::restartAnimation(){
	currentIndex = startingIndex;
	timesRun = 0;
	updateTexture();
}

void Animation::deepCopy(Animation* toCopy){
	toCopy->restartAnimation();
	this->currentTexture = new Texture();
	this->currentTexture->deepCopy(toCopy->currentTexture);
	this->animationType = toCopy->animationType;
	this->currentIndex = toCopy->currentIndex;
	this->endingIndex = toCopy->endingIndex;
	this->loop = toCopy->loop;
	this->startingIndex = toCopy->startingIndex;
	this->timesRun = 0;
}