#include "Animation.h"
#include "Texture.h"

Animation::Animation(ANIMATION_TYPE animationType, unsigned startingIndex, unsigned endingIndex) : animationType(animationType), startingIndex(startingIndex), endingIndex(endingIndex), currentIndex(startingIndex)
{
}

Animation::~Animation()
{
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
		}
		updateTexture();
		animationElapsed = 0;
	}
}

void Animation::restartAnimation(){
	currentIndex = startingIndex;
	updateTexture();
}