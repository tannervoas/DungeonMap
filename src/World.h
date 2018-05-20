/*
* Author: Tanner Voas
* Notes: Represent all objects in our scene and presents them to the MainDriver in a manageable way.
*/
#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED

#include <stdio.h>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Block.h"
#include "bvh.h"
/**
* World
**/
struct World
{
	//Attributes
	BVH my_bv;
	int width, height;
	Block* blocks;
	int block_count;
	int total_count;
	int startx, starty, startz;

	int constructed;
	float* modelData;
	int* elements;
	int* numTris;
	int total_elements;

	//Methods
	World(void); //Default constructor
	World(char* file); //Reads map information from file
	World(int width_, int height_);

	float* BuildVertices(void); //Combines all the objects vertices into one array
	int* getElements(void);
	int getElements(int index);
	int* getVerticeCount(void);
	int getVerticeCount(int index);
	glm::mat4 getModelMatrix(int i, float time); //Get the model matrix of an element in our scene
	char enforceRules(glm::vec3 pos); //Enforces certain rules for the player such as no flight. Deprecated.
	glm::vec3 verified(glm::vec3 new_pos, float r, glm::vec3 old_pos, glm::vec3* vv); //New colision detection method.
	char getType(int index); //Gets object texture type
	char getIndex(int index); //Gets object index in its types coresponding array
};

#endif
