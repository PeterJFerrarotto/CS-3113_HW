#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H

inline float lerp(float v0, float v1, float t){
	return (1 - t)*v0 + t*v1;
}

inline float easeIn(float from, float to, float time){
	float tVal = time*time*time*time*time;
	return (1.0f - tVal)*from + tVal*to;
}

inline float easeOut(float from, float to, float time) {
	float oneMinusT = 1.0f - time; 
	float tVal = 1.0f - (oneMinusT * oneMinusT * oneMinusT * oneMinusT * oneMinusT);
	return (1.0f - tVal)*from + tVal*to;
}

inline float randomRanged(float maximum){
	return (-maximum * 0.5) + (maximum * ((float)rand() / (float)RAND_MAX));
}

#endif
