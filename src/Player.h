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
	glm::vec4 look4;
	float locky;
	int width, height;
	float turn_speed, move_speed, jump_vel, jump_pos, jump_height, jump_scale;
	int max_jumps, jumps, first_jump;
	float horiz, vert;
	World my_world;
	float downLim, upLim;
	int won;
	int hitx, hity, hitz;
	int active;
	int elements, numTris;
	float* modelData;
	float frotation;
	float srotation;

	glm::vec3 velocityVec;
	glm::vec3 gravityVec;
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
	//Restricts us to ground level. These are deprecated
	void enforeRules(void);
	void enforeRules(glm::vec3 * pos, glm::vec3 * loo);
	//Checks if new position is valid. Used to block movement into walls
	char isValid(glm::vec3 pos);
	glm::vec3 isValid(glm::vec3 new_pos, float r, glm::vec3 old_pos);
	//Others
	void Jump(void); //Applies jump. Works even if flight isnt on. Deprecated function
	void setForces(glm::vec3 vv, glm::vec3 gv); //Sets gravity and initial velocity of our player
};

#endif