#ifndef __PARTICLE_H__
#define __PARTICLE_H__

const unsigned int MAX_PARTICLES = 100000;
const int PARTICLE_MESH_SIZE = 30;

typedef struct vec2 {
	float x;
	float y;
} vec2;

class Particle {
public:
	Particle():life(0) {}

	void init(float x, float y, float w, float h,
			  float vx, float vy, float l) {
		life = l;

		pos.x = x;
		pos.y = y;

		size.x = w;
		size.y = h;

		vel.x = vx;
		vel.y = vy;

		acc.x = 0;
		acc.y = 0;
	}

	bool in_use() const {
		return life > 0;
	}

	bool update() {
		if (!in_use()) {
			return true;
		}

		float delta = 0.05;
		float grav = 2.0;

		acc.y += grav;

		acc.x *= delta;
		acc.y *= delta;

		vel.x += acc.x;
		vel.y += acc.y;

		pos.x += vel.x;
		pos.y += vel.y;

		acc.x = 0.0;
		acc.y = 0.0;

		life--;

		return life < 0;
	}

	Particle *get_next() const {
		return _next;
	}

	void set_next(Particle *next) {
		_next = next;
	}

	vec2 get_pos() const {
		return pos;
	}

	vec2 get_size() const {
		return size;
	}


private:
	Particle *_next;
	float life;
	vec2 pos;
	vec2 vel;
	vec2 acc;
	vec2 size;
};

class ParticlePool {
public:
	ParticlePool() {
		first_available = &particles[0];

		for (int i = 0; i < POOL_SIZE - 1; i++) {
			particles[i].set_next(&particles[i + 1]);
		}
		particles[POOL_SIZE - 1].set_next(NULL);
	}

	void create(float x, float y, float w, float h,
				 float vx, float vy, float life) {

		if (first_available == NULL) {
			return;
		}

		Particle *new_particle = first_available;

		first_available = new_particle->get_next();

		new_particle->init(x, y, w, h, vx, vy, life);
	}

	void update() {
		for (int i = 0; i < POOL_SIZE; i++) {
			if (particles[i].update()) {
				particles[i].set_next(first_available);
				first_available = &particles[i];
			}
		}
	}

	int draw() {
		int quads = 0;
		int quads30 = 0;

		for (int i = 0; i < POOL_SIZE; i++) {
			if (particles[i].in_use()) {

				int quads30i = quads30;

				vec2 pos = particles[i].get_pos();
				vec2 size = particles[i].get_size();

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

private:
	Particle *first_available;
	static const int POOL_SIZE = MAX_PARTICLES;
	Particle particles[POOL_SIZE];
	float vertices[MAX_PARTICLES * PARTICLE_MESH_SIZE];
};

#endif
