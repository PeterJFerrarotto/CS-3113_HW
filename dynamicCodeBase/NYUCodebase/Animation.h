#ifndef ANIMATION_H
#define ANIMATION_H

enum ANIMATION_TYPE{ ANIMATION_IDLE, ANIMATION_RUN, ANIMATION_WALK, ANIMATION_JUMP, ANIMATION_FALL };

class Texture;
class Animation
{
protected:
	ANIMATION_TYPE animationType;
	Texture* currentTexture;
	unsigned startingIndex, endingIndex;
	unsigned currentIndex;
	float animationElapsed;
	bool loop;
	void updateTexture();
public:
	//Note: animations can only be used with even spritesheets!
	Animation(ANIMATION_TYPE animationType, unsigned startingIndex, unsigned endingIndex);
	~Animation();

	ANIMATION_TYPE getAnimationType();
	Texture* getTexture();
	bool getDoLoop();
	
	void setTexture(Texture* texture);
	void setDoLoop(bool loop);

	void runAnimation(float elapsed, float fps);

	void restartAnimation();
};

#endif