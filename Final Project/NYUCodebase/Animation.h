#ifndef ANIMATION_H
#define ANIMATION_H

enum ANIMATION_TYPE{ ANIMATION_IDLE, ANIMATION_RUN, ANIMATION_WALK, ANIMATION_JUMP, ANIMATION_FALL, ANIMATION_DESTROY, ANIMATION_DEACTIVATE, ANIMATION_HIDING, ANIMATION_FIRE, ANIMATION_FIRE_RETURN };

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
	bool animationCollides;
	void updateTexture();
	unsigned timesRun;
public:
	//Note: animations can only be used with even spritesheets!
	Animation(Animation* toCopy);
	Animation(ANIMATION_TYPE animationType, unsigned startingIndex, unsigned endingIndex);
	~Animation();

	ANIMATION_TYPE getAnimationType();
	Texture* getTexture();
	bool getDoLoop();

	bool animationIsDone();
	
	void setTexture(Texture* texture);
	void setDoLoop(bool loop);
	void setAnimationCollides(bool collides);

	void runAnimation(float elapsed, float fps);

	void restartAnimation();

	virtual void deepCopy(Animation* toCopy);

	void freeMemory();
	bool getAnimationCollides();
};

#endif