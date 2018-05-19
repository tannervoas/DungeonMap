/*******************************************************************************
* Name            : Camera.h
* Project         : DungeonMap
* Module          : App
* Description     : Header file for Camera.cpp
* Copyright       : Tanner Voas.
* Creation Date   : 5/17/2018
* Original Author : Tanner Voas
*
******************************************************************************/
#ifndef SRC_CAMERA_H_
#define SRC_CAMERA_H_

#include <stdio.h>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "./settings.h"

/**
* Camera
**/
struct Camera {  // Our camera and rolling ball
    // Attributes
    glm::vec3 position, look, up, point;
    int width, height;
    float turn_speed, move_speed;
    float horiz, vert;
    float downLim, upLim;
    int elements, numTris;
    float* modelData;
    float frotation;
    float srotation;

    glm::vec3 right;
    glm::vec3 xzproj;
    glm::vec3 xyproj;
    glm::mat4 model;
    glm::vec3 rotor;
    // Methods
    Camera(void);  // Default constructor
    Camera(int x_, int y_, int z_, int width_, int height_);
    glm::mat4 getViewMatrix(void);
    glm::mat4 getModelMatrix(void);  // Returns blocks Model Matrix
    float * getModelData();  // Info for the model - vertices - which make up the shape.
    int getElements(void);
    int getVerticeCount(void);
    // Turning
    void Rotate(float dx, float dy);
    // Movement
    void stepForward(float dir, float time);
    void stepRight(int dir, float time);
    void stepUp(int dir, float time);
    // Restricts us to ground level. These are deprecated
    // Checks if new position is valid. Used to block movement into walls
    // Others
};

#endif  // SRC_CAMERA_H_
