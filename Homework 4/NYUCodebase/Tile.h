#ifndef TILE_H
#define TILE_H
enum TILE_TYPE {SOLID, SOLID_GRASS, WATER, CHECKPOINT, HAZARD, HAZARD_SPIKE_HORIZONTAL, HAZARD_SPIKE_VERTICAL, HAZARD_FIRE, BACKGROUND_TILE};
struct Tile{
	unsigned tileID;
	TILE_TYPE tileType;
	float x, y;
	float width, height;
	bool canCollide;
};

#endif