/*******************************************************************************
* Name            : WorldManager.h
* Project         : DungeonMap
* Module          : App
* Description     : Header file for WorldManager.cpp
* Copyright       : Tanner Voas.
* Creation Date   : 5/17/2018
* Original Author : Tanner Voas
*
******************************************************************************/
#ifndef SRC_WORLDMANAGER_H_
#define SRC_WORLDMANAGER_H_

#include <stdio.h>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "./settings.h"
#include "./Object.h"
#include "./Camera.h"
/**
* WorldManager
**/
struct WorldManager {
    // Attributes
    int width, height;
    int object_count;
    int total_count;
    int startx, starty, startz;
    Object* objects;
    int constructed;
    float* modelData;
    int* elements;
    int* numTris;
    int total_elements;

    // Methods
    WorldManager(void);  // Default constructor
    explicit WorldManager(char* file);  // Reads map information from file
    WorldManager(int width_, int height_);

    float* BuildVertices(void);  // Combines all the objects vertices into one array
    int* getElements(void);
    int getElements(int index);
    int* getVerticeCount(void);
    int getVerticeCount(int index);
    glm::mat4 getModelMatrix(int i, float time);  // Get the model matrix of an element in our scene
    char getType(int index);  // Gets object texture type
    char getIndex(int index);  // Gets object index in its types coresponding array
};

#endif  // SRC_WORLDMANAGER_H_
