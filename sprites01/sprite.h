#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <stdio.h>
#include <math.h>
#include <cmath>

const unsigned int MAX_SPRITES = 10000;
const int SPRITE_MESH_SIZE = 30;

float norm( float val, float min, float max ) {
	return (val - min) / (max - min);
}

float lerp( float norm, float min, float max ) {
	return (max - min) * norm + min;
}

float map( float val, float srcmin, float srcmax,
		float dstmin, float dstmax ) {

	return lerp( norm( val, srcmin, srcmax ), dstmin, dstmax );
}

typedef struct vec2 {
	float x;
	float y;
} vec2;

class Sprite {
public:
	Sprite():life(0) {}

	void init( float x, float y, float w, float h,
			   float vx, float vy, float l ) {
		life = l;
		pos.x = x;
		pos.y = y;
		size.x = w;
		size.y = h;
		o_size.x = size.x;
		o_size.y = size.y;
		vel.x = vx;
		vel.y = vy;
	}

	bool in_use() const {
		return life > 0;
	}

	bool update(Sprite *sprites) {
		if (!in_use()) {
			return true;
		}

		float fact = 0.05f;
		float force = 2.0f;

		Sprite p1 = sprites[magnet_next];
		Sprite p2 = sprites[magnet_prev];

		float dx1 = p1.pos.x - pos.x;
		float dx2 = p2.pos.x - pos.x;
		float dy1 = p1.pos.y - pos.y;
		float dy2 = p2.pos.y - pos.y;

		float fx = fact * force *
			abs( dx1 ) / ( dx1 ) *
			pow( dx1, 2 ) /
			( pow( dy1, 2 )  + pow( dx1, 2 ) );

		float fy = fact * force *
			abs( dy1) / ( dy1 ) *
			pow( dy1, 2 ) /
			( pow( dy1, 2 )  + pow( dx1, 2 ) );

		float rx = fact * force *
			abs( dx2 ) / ( dx2 ) *
			pow( dx2, 2 ) /
			( pow( dy2, 2 )  + pow( dx2, 2 ) );

		float ry = fact * force *
			abs( dy2 ) / ( dy2 ) *
			pow( dy2, 2 ) /
			( pow( dy2, 2 ) + pow( dx2, 2 ) );

		vel.x += ( 4 * fx - rx ) * 4 / pow( life, 0.01 );
		vel.y += ( 4 * fy - ry ) * 4 / pow( life, 0.01 );

		pos.x += vel.x * 0.1;
		pos.y += vel.y * 0.09;

		float s = map( life, 0, 100, 0, o_size.x );
		size.x = size.y = s;

		life--;

		return life < 0;
	}

	float get_life() const {
		return life;
	}

	Sprite *get_next() const {
		return _next;
	}

	vec2 get_pos() const {
		return pos;
	}

	vec2 get_size() const {
		return size;
	}

	void set_index( int i ) {
		index = i;
	}

	void set_magnet( int next, int prev ) {
		magnet_next = next;
		magnet_prev = prev;
	}

	void set_next( Sprite *next ) {
		_next = next;
	}

private:
	Sprite *_next;
	float life;
	vec2 pos;
	vec2 vel;
	vec2 size;
	vec2 o_size;
	int magnet_next;
	int magnet_prev;
	int index;
};

class SpritePool {
public:
	SpritePool() {
		first_available = &sprites[0];

		int magnet_next;
		int magnet_prev;

		for ( int i = 0; i < POOL_SIZE; i++ ) {

			magnet_next = (i+1) % POOL_SIZE;
			magnet_prev = (i+2) % POOL_SIZE;

			sprites[i].set_index( i );
			sprites[i].set_magnet( magnet_next, magnet_prev );
			sprites[i].set_next( &sprites[i + 1] );
		}

		sprites[POOL_SIZE - 1].set_next( NULL );
	}

	Sprite *at(unsigned int idx) {
		if ( idx < POOL_SIZE ) {
			return &(sprites[idx]);
		} else {
			return NULL;
		}
	}

	void create( float x, float y, float w, float h,
				 float vx, float vy, float life ) {

		if ( first_available == NULL ) {
			return;
		}

		Sprite *new_sprite = first_available;

		first_available = new_sprite->get_next();

		new_sprite->init( x, y, w, h, vx, vy, life );
	}

	void update() {
		for ( int i = 0; i < POOL_SIZE; i++ ) {

			if ( sprites[i].update( sprites ) ) {
				sprites[i].set_next( first_available );
				first_available = &sprites[i];
			}
		}
	}


	int draw() {
		int quads = 0;
		int quads30 = 0;

		for ( int i = 0; i < POOL_SIZE; i++ ) {
			if ( sprites[i].in_use() ) {

				int quads30i = quads30;

				vec2 pos = sprites[i].get_pos();
				vec2 size = sprites[i].get_size();

				float x = pos.x;
				float y = pos.y;
				float xx = x + size.x;
				float yy = y + size.y;

				vertices[quads30i++] = x;
				vertices[quads30i++] = y;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 1;

				vertices[quads30i++] = xx;
				vertices[quads30i++] = y;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 1;
				vertices[quads30i++] = 1;

				vertices[quads30i++] = x;
				vertices[quads30i++] = yy;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 0;

				vertices[quads30i++] = x;
				vertices[quads30i++] = yy;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 0;

				vertices[quads30i++] = xx;
				vertices[quads30i++] = y;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 1;
				vertices[quads30i++] = 1;

				vertices[quads30i++] = xx;
				vertices[quads30i++] = yy;
				vertices[quads30i++] = 0;
				vertices[quads30i++] = 1;
				vertices[quads30i++] = 0;

				quads30 += 30;
				quads++;
			}
		}

		return quads * 6;
	}

	inline const float *get_vertices() const {
		return vertices;
	}

	inline const int size() const { return POOL_SIZE; }

private:
	Sprite *first_available;
	static const int POOL_SIZE = MAX_SPRITES;
	Sprite sprites[POOL_SIZE];
	float vertices[MAX_SPRITES * SPRITE_MESH_SIZE];
};

#endif
