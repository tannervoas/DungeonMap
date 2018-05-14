/*
* Author: Tanner Voas
* Notes: Block element of our scene. No complicated behavior and just blocks user.
*/
#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Block.h"

Block::Block(void)
{
}

Block::Block(float x_, float y_, float z_, float hx_, float hy_, float hz_, float rx_, float ry_, float rz_)
{
	x = x_;
	y = y_;
	z = z_;
	ox = x_;
	oy = y_;
	oz = z_;
	hx = hx_;
	hy = hy_;
	hz = hz_;
	rx = rx_;
	ry = ry_;
	rz = rz_;
	elements = 0;
	feature_type = 0;
	blockModel = glm::translate(blockModel, glm::vec3(x, y, z));
	blockModel = glm::rotate(blockModel, rx, glm::vec3(1.0f, 0.f, 0.f));
	blockModel = glm::rotate(blockModel, ry, glm::vec3(0.f, 1.0f, 0.f));
	blockModel = glm::rotate(blockModel, rz, glm::vec3(0.f, 0.f, 1.0f));
	blockModel = glm::scale(blockModel, glm::vec3(hx, hy, hz));
	blockInvModel = glm::inverse(blockModel);
	noTransBlockModel = glm::rotate(noTransBlockModel, rx, glm::vec3(1.0f, 0.f, 0.f));
	noTransBlockModel = glm::rotate(noTransBlockModel, ry, glm::vec3(0.f, 1.0f, 0.f));
	noTransBlockModel = glm::rotate(noTransBlockModel, rz, glm::vec3(0.f, 0.f, 1.0f));
	noTransBlockModel = glm::scale(noTransBlockModel, glm::vec3(hx, hy, hz));
	noTransInvBlockModel = glm::inverse(noTransBlockModel);
	first_cent = 1;
	retention = 0.99f;
	str = 0.0f;
	vx = 0;
	vy = 0;
	vz = 0;
	lt = 0;
	ut = 0;
	time_ = 0;
	timeDir = 1;
	block_loc = 0;
}

void Block::advanceTimeStep(void)
{
	time_ = time_ + GLOBAL_TIME_STEP * timeDir;
	if (time_ > ut) {
		time_ = ut;
		timeDir = -1;
	}
	else if (time_ < lt) {
		time_ = lt;
		timeDir = 1;
	}
	glm::vec3 displace = glm::vec3(vx, vy, vz) * time_;
	x = displace.x + ox;
	y = displace.y + oy;
	z = displace.z + oz;
	blockModel = glm::mat4();
	blockModel = glm::translate(blockModel, glm::vec3(x, y, z));
	blockModel = glm::rotate(blockModel, rx, glm::vec3(1.0f, 0.f, 0.f));
	blockModel = glm::rotate(blockModel, ry, glm::vec3(0.f, 1.0f, 0.f));
	blockModel = glm::rotate(blockModel, rz, glm::vec3(0.f, 0.f, 1.0f));
	blockModel = glm::scale(blockModel, glm::vec3(hx, hy, hz));
	blockInvModel = glm::inverse(blockModel);
}

glm::mat4 Block::getModelMatrix(float time)
{
	return blockModel;
}

glm::mat4 Block::getInvModelMatrix(void)
{
	return blockInvModel;
}

float * Block::getModelData()
{
	if (elements == 0) {
		std::ifstream modelFile;
		modelFile.open("models/cube.txt");
		modelFile >> elements;
		modelData = new	float[elements];
		for (int i = 0; i < elements; i++) {
			modelFile >> modelData[i];
		}
		numTris = elements / 8;
	}
	return modelData;
}

int Block::getElements(void)
{
	return elements;
}

int Block::getVerticeCount(void)
{
	return numTris;
}

char Block::inside(glm::vec4 pos)
{
	if (!(pos.x >= -0.5f && pos.x <= 0.5f)) {
		return 0;
	}
	if (!(pos.y >= -0.5f && pos.y <= 0.5f)) {
		return 0;
	}
	if (!(pos.z >= -0.5f && pos.z <= 0.5f)) {
		return 0;
	}
	return 1;
}

int smallest6(float a, float b, float c, float d, float e, float f) {
	float smallest = a;
	int small = 1;
	if (b < smallest) {
		smallest = b;
		small = 2;
	}
	if (c < smallest) {
		smallest = c;
		small = 3;
	}
	if (d < smallest) {
		smallest = d;
		small = 4;
	}
	if (e < smallest) {
		smallest = e;
		small = 5;
	}
	if (f < smallest) {
		smallest = f;
		small = 6;
	}
	return small;
}

glm::vec3 Block::verified(glm::vec4 new_pos, glm::vec4 new_pos_store, float r, glm::vec4 old_pos, glm::vec4 * vv) //WHYYYYY+
{
	if (new_pos.x - r / hx <= 0.5 && new_pos.x + r / hx >= -0.5 && new_pos.y - r / hy <= 0.5 && new_pos.y + r / hy >= -0.5 && new_pos.z - r / hz <= 0.5 && new_pos.z + r / hz >= -0.5) {
		//new_pos moved inside this block.
		float pxd = fabs(new_pos.x - r / hx - 0.5);
		float nxd = fabs(new_pos.x + r / hx + 0.5);
		float pyd = fabs(new_pos.y - r / hy - 0.5);
		float nyd = fabs(new_pos.y + r / hy + 0.5);
		float pzd = fabs(new_pos.z - r / hz - 0.5);
		float nzd = fabs(new_pos.z + r / hz + 0.5);
		int small = smallest6(pxd, nxd, pyd, nyd, pzd, nzd); //Tells us the closest wall to our new_pos. Allows for proper response.
		if (small == 1) {
			new_pos.x = 0.5f + r / hx;
			if (vv->x < 0.f) {
				vv->x = 0.f;
			}
			if (feature_type == 1) {
				vv->x += str / hx;
			}
		} 
		else if (small == 2) {
			new_pos.x = -0.5f - r / hx;
			if (vv->x > 0.f) {
				vv->x = 0.f;
			}
			if (feature_type == 1) {
				vv->x -= str / hx;
			}
		}
		else if (small == 3) {
			new_pos.y = 0.5f + r / hy;
			if (vv->y < 0.f) {
				vv->y = 0.f;
			}
			if (feature_type == 1) {
				vv->y += str / hy;
			}
		}
		else if (small == 4) {
			new_pos.y = -0.5f - r / hy;
			if (vv->y > 0.f) {
				vv->y = 0.f;
			}
			if (feature_type == 1) {
				vv->y -= str / hy;
			}
		}
		else if (small == 5) {
			new_pos.z = 0.5f + r / hz;
			if (vv->z < 0.f) {
				vv->z = 0.f;
			}
			if (feature_type == 1) {
				vv->z += str / hz;
			}
		}
		else if (small == 6) {
			new_pos.z = -0.5f - r / hz;
			if (vv->z > 0.f) {
				vv->z = 0.f;
			}
			if (feature_type == 1) {
				vv->z -= str / hz;
			}
		}
		if (feature_type == 3) {
			printf("Congrats you won!!!!\n");
		}
		vv->x = vv->x * retention;
		vv->y = vv->y * retention;
		vv->z = vv->z * retention;
	}
	new_pos = blockModel * new_pos;
	glm::vec3 ret = glm::vec3(new_pos.x, new_pos.y, new_pos.z);
	return ret;
}

Triplet Block::center(void)
{
	if (first_cent) {
		glm::vec4 uuu = getModelMatrix(0) * glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		glm::vec4 uud = getModelMatrix(0) * glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
		glm::vec4 udu = getModelMatrix(0) * glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
		glm::vec4 duu = getModelMatrix(0) * glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
		glm::vec4 udd = getModelMatrix(0) * glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
		glm::vec4 ddu = getModelMatrix(0) * glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
		glm::vec4 dud = getModelMatrix(0) * glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
		glm::vec4 ddd = getModelMatrix(0) * glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
		cent = (1.0f / 8.0f) * (uuu + uud + udu + duu + udd + ddu + dud + ddd);
		rad = mag3(uuu - cent);
		float tmp_r = mag3(uud - cent);
		if (tmp_r > rad) {
			rad = tmp_r;
		}
		tmp_r = mag3(udu - cent);
		if (tmp_r > rad) {
			rad = tmp_r;
		}
		tmp_r = mag3(duu - cent);
		if (tmp_r > rad) {
			rad = tmp_r;
		}
		tmp_r = mag3(udd - cent);
		if (tmp_r > rad) {
			rad = tmp_r;
		}
		tmp_r = mag3(ddu - cent);
		if (tmp_r > rad) {
			rad = tmp_r;
		}
		tmp_r = mag3(dud - cent);
		if (tmp_r > rad) {
			rad = tmp_r;
		}
		tmp_r = mag3(ddd - cent);
		if (tmp_r > rad) {
			rad = tmp_r;
		}
	}
	if (feature_type == 2) {
		glm::vec4 dis = 0.5f * (glm::vec4(vx, vy, vz, 0.0f) * ut + glm::vec4(vx, vy, vz, 0.0f) * lt);
		cent = cent + dis;
		dis = glm::vec4(vx, vy, vz, 0.0f) * ut - dis;
		rad = rad + sqrt(pow(dis.x, 2) + pow(dis.y, 2) + pow(dis.z, 2));
	}
	return Triplet(cent.x, cent.y, cent.z);
}

float Block::mag3(glm::vec4 d) {
	return Triplet(d.x, d.y, d.z).getFloatMag();
}

float Block::radius(void)
{
	return rad;
}
