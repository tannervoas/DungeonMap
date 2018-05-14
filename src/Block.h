/*
* Author: Tanner Voas
* Notes: Block element of our scene. Many types of blocks are encapsulated in this class.
*/
#ifndef BLOCK_INCLUDED
#define BLOCK_INCLUDED
#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>

#include <stdio.h>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "triplet.h"

/**
* Block
**/
struct Block //Self made block class with variable behavior
{
	//Attributes
	float x, y, z, ox, oy, oz, hx, hy, hz, rx, ry, rz, str, vx, vy, vz, lt, ut;
	float* modelData;
	int elements;
	int numTris;
	int feature_type;
	int first_cent;
	int timeDir;
	glm::mat4 blockModel;
	glm::mat4 blockInvModel;
	glm::mat4 noTransBlockModel;
	glm::mat4 noTransInvBlockModel;
	glm::vec4 cent;
	float rad;
	float retention;
	float time_;
	int block_loc;
	//Methods
	Block(void); //Default constructor
	Block(float x_, float y_, float z_, float hx_, float hy_, float hz_, float rx_, float ry_, float rz_);
	void advanceTimeStep(void); //Advances time for moving blocks
	glm::mat4 getModelMatrix(float time); //Returns blocks Model Matrix
	glm::mat4 getInvModelMatrix(void); //Returns inverse of blocks Model matrix
	float* getModelData(); //Info for the model - vertices - which make up the shape.
	int getElements(void);
	int getVerticeCount(void);
	char inside(glm::vec4 pos); //Deprecated Function
	glm::vec3 verified(glm::vec4 new_pos, glm::vec4 new_pos_store, float r, glm::vec4 old_pos, glm::vec4 * vv); //Tests to see if our new_pos crossed inside the block and if it does we modify out new_pos and velocity.
	Triplet center(void); //Center of the block - or blocks area it traverses - which is used by BVH
	float mag3(glm::vec4 d);
	float radius(void); //Radius of the block - or radius of sphere around blocks path of traversal - which is used by BVH. 
};

#endif