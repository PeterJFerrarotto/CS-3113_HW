#ifndef TILE_H
#define TILE_H
enum TILE_TYPE {SOLID, SOLID_GRASS, WATER, HAZARD, HAZARD_SPIKE, HAZARD_FIRE};
struct Tile{
	unsigned tileID;
	float x, y;
	float width, height;
	bool canCollide;
};

#endif