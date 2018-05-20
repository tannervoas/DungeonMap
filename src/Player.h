/*
* Author: Tanner Voas
* Notes: Represents our camera and players position.
*/
#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include <stdio.h>
#include "World.h"

/**
* Player
**/
struct Player //Our camera and rolling ball
{
	//Attributes
	glm::vec3 position, look, up, point;
	int width, height;
    float turn_speed, move_speed;
	float horiz, vert;
	World my_world;
	float downLim, upLim;
	int elements, numTris;
	float* modelData;

	glm::vec3 right;
	glm::vec3 xzproj;
	glm::vec3 xyproj;
	glm::mat4 model;
	glm::vec3 rotor;
	//Methods
	Player(void); //Default constructor
	Player(int x_, int y_, int z_, int width_, int height_, World my_world_);
	glm::mat4 getViewMatrix(float sprint);
	glm::mat4 getNoTranViewMatrix(float time);
	glm::mat4 getModelMatrix(void); //Returns blocks Model Matrix
	float * getModelData(); //Info for the model - vertices - which make up the shape.
	int getElements(void);
	int getVerticeCount(void);
	//Turning
	void Rotate(float dx, float dy);
	//Movement
	void stepForward(float dir, float time);
	void stepRight(int dir, float time);
	void stepUp(int dir, float time);
};

#endif