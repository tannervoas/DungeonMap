/*
* Author: Tanner Voas
* Notes: Represents our camera and players position.
*/
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Player.h"
#include "World.h"
#include "settings.h"

glm::vec3 project(glm::vec3 source, glm::vec3 dest) {
	float mag = source.x * dest.x + source.y * dest.y + source.z * dest.z;
	return mag * dest;
}
float magnitude(glm::vec3 v) {
	return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

Player::Player(void)
{
	elements = 0;
}

Player::Player(int x_, int y_, int z_, int width_, int height_, World my_world_)
{
	position = glm::vec3(x_,y_,z_);
	look = glm::vec3();
	up = glm::vec3(0, 1, 0);
	width = width_;
	height = height_;
	turn_speed = 0.05f;
	move_speed = 50.0f;
	horiz = 3.14f/2;
	vert = 0;
	look.x = cosf(vert)*sinf(horiz);
	look.y = sinf(vert);
	look.z = cosf(vert)*cosf(horiz);
	point = glm::normalize(look);
	look = look + position;
	my_world = my_world_;
	downLim = -1.5f;
	upLim = 1.5f;

	elements = 0;
	xzproj = glm::normalize(project(point, glm::vec3(1, 0, 0)) + project(point, glm::vec3(0, 0, 1)));
	right = glm::normalize(glm::cross(xzproj, up));
}

glm::mat4 Player::getViewMatrix(float sprint)
{
	glm::mat4 view;
    view = glm::lookAt(  // Delete the (10,5,0) after we stop drawing the player sphere
            position + glm::vec3(10, 5, 0),  //Cam Position
			look + glm::vec3(10, 5, 0),  //Look at point
			up); //Up
	return view;
}

glm::mat4 Player::getNoTranViewMatrix(float time)
{
	return glm::lookAt(
		glm::vec3(),  //Cam Position
		point,  //Look at point
		up); //Up;
}

glm::mat4 Player::getModelMatrix(void)
{
	model = glm::mat4();
	model = glm::translate(model, position);
	return model;
}

float * Player::getModelData()
{
	if (elements == 0) {
		std::ifstream modelFile;
		modelFile.open("models/sphere.txt");
		modelFile >> elements;
		modelData = new	float[elements];
		for (int i = 0; i < elements; i++) {
			modelFile >> modelData[i];
		}
		numTris = elements / 8;
	}
	return modelData;
}

int Player::getElements(void)
{
	return elements;
}

int Player::getVerticeCount(void)
{
	return numTris;
}

void Player::Rotate(float dx, float dy) {
	horiz += -1 * turn_speed * float(2 * dx / width - 1);
	vert += -1 * turn_speed * float(2 * dy / height - 1);
	if (vert < downLim) {
		vert = downLim;
	}
	else if (vert > upLim) {
		vert = upLim;
	}
	look.x = cosf(vert)*sinf(horiz);
	look.y = sinf(vert);
	look.z = cosf(vert)*cosf(horiz);
	point = glm::normalize(look);
	look = point + position;
	xzproj = glm::normalize(project(point, glm::vec3(1, 0, 0)) + project(point, glm::vec3(0, 0, 1)));
	right = glm::normalize(glm::cross(xzproj, up));
}

void Player::stepForward(float dir, float time)
{
	glm::vec3 amount = xzproj * GLOBAL_TIME_STEP * move_speed * (float)dir;
	position = position + amount;;
	look = look + amount;
}

void Player::stepRight(int dir, float time)
{
	glm::vec3 amount = right * GLOBAL_TIME_STEP * move_speed * (float)dir;
	position = position + amount;;
	look = look + amount;
}

void Player::stepUp(int dir, float time)
{
	glm::vec3 amount = up * GLOBAL_TIME_STEP * move_speed * (float)dir;
	position = position + amount;;
	look = look + amount;
}
