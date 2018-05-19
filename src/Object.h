/*
* Author: Tanner Voas
* Notes: Block element of our scene. Many types of blocks are encapsulated in this class.
*/
#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_
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

/**
* Object
**/
struct Object //Self made block class with variable behavior
{
    //Attributes
    float x, y, z, hx, hy, hz, rx, ry, rz;
    float* modelData;
    int elements;
    int numTris;
    int first_cent;
    int timeDir;
    glm::mat4 blockModel;
    glm::mat4 blockInvModel;
    glm::mat4 noTransBlockModel;
    glm::mat4 noTransInvBlockModel;
    glm::vec4 cent;
    float rad;
    int object_loc;
    //Methods
    Object(void); //Default constructor
    Object(float x_, float y_, float z_, float hx_, float hy_, float hz_, float rx_, float ry_, float rz_);
    glm::mat4 getModelMatrix(float time); //Returns blocks Model Matrix
    glm::mat4 getInvModelMatrix(void); //Returns inverse of blocks Model matrix
    float* getModelData(); //Info for the model - vertices - which make up the shape.
    int getElements(void);
    int getVerticeCount(void);
    glm::vec3 center(void); //Center of the block - or blocks area it traverses - which is used by BVH
    float mag3(glm::vec4 d);
    float radius(void); //Radius of the block - or radius of sphere around blocks path of traversal - which is used by BVH. 
};

#endif