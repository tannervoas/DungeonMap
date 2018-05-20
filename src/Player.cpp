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
	locky = y_;
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
	jumps = 0;
	max_jumps = MAX_JUMPS;
	jump_vel = 0.0f;
	jump_pos = 0.0f;
	jump_height = 3.0f;
	jump_scale = 10.0f;
	first_jump = 1;
	downLim = -1.5f;
	upLim = 1.5f;
	won = 0;
	hitx = 0;
	hity = 0;
	hitz = 0;
	active = 1;
	elements = 0;
	frotation = 0.f;
	srotation = 0.f;
	xzproj = glm::normalize(project(point, glm::vec3(1, 0, 0)) + project(point, glm::vec3(0, 0, 1)));
	right = glm::normalize(glm::cross(xzproj, up));
}

glm::mat4 Player::getViewMatrix(float sprint)
{
	float vmag = sqrt(pow(velocityVec.x, 2) + pow(velocityVec.y, 2) + pow(velocityVec.z, 2));
	//printf("SPRINT: %f\n", sprint);
	if (vmag > sprint * TERMINAL_VELOCITY) {
		velocityVec = (TERMINAL_VELOCITY * sprint) * velocityVec / vmag;
	}
	glm::mat4 view;
	if (active) {
		glm::vec3 tmp_position = position + velocityVec * GLOBAL_TIME_STEP + gravityVec *GLOBAL_TIME_STEP * GLOBAL_TIME_STEP * 0.5f;
		velocityVec = velocityVec + gravityVec * 0.01f;
		glm::vec3 new_tmp_position = isValid(tmp_position, RADIUS * TEN_TIMES_NEAR / 10.0f, position); //Weird Issue. Gravity allows it to travel through the blocks
		glm::vec3 pos_change = new_tmp_position - position;
		glm::vec3 xzchange = project(pos_change, glm::vec3(1, 0, 0)) + project(pos_change, glm::vec3(0, 0, 1));
		float mag = magnitude(xzchange);
		if (mag > 0.001f) {

			frotation -= mag;

			rotor = glm::cross(glm::normalize(xzchange), up);
		}
		position = new_tmp_position;
		look = position + point;

		
	}
	glm::vec3 dis = 1.0f * up - 5.0f * point;
	view = glm::lookAt(
			position + dis,  //Cam Position
			look + dis,  //Look at point
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
	float increase = pow(RADIUS, (1.f / 3.f));
	model = glm::translate(model, position);
	model = glm::rotate(model, frotation, rotor);
	model = glm::scale(model, glm::vec3(increase, increase, increase));
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
	if (active) {
		velocityVec = velocityVec + xzproj * GLOBAL_TIME_STEP * move_speed * dir;
	}
	else if (ALLOW_DEVELOPER) {
		glm::vec3 amount = xzproj * GLOBAL_TIME_STEP * move_speed * (float)dir;
		position = position + amount;;
		look = look + amount;
	}
}

void Player::stepRight(int dir, float time)
{
	if (active) {
		velocityVec = velocityVec + right * GLOBAL_TIME_STEP * move_speed * (float)dir;
	}
	else if (ALLOW_DEVELOPER) {
		glm::vec3 amount = right * GLOBAL_TIME_STEP * move_speed * (float)dir;
		position = position + amount;;
		look = look + amount;
	}
}

void Player::stepUp(int dir, float time)
{
	
	if (active) {
		velocityVec = velocityVec + up * GLOBAL_TIME_STEP * move_speed * (float)dir;
	}
	else if (ALLOW_DEVELOPER) {
		glm::vec3 amount = up * GLOBAL_TIME_STEP * move_speed * (float)dir;
		position = position + amount;;
		look = look + amount;
	}
}

void Player::enforeRules(void) {
	if (!FLIGHT) {
		look.y -= position.y;
		position.y = 0.0f;
	}
}

void Player::enforeRules(glm::vec3 * pos, glm::vec3 * loo) {
	if (!FLIGHT) {
		loo->y = loo->y - pos->y + locky;
		pos->y = locky;
	}
}

char Player::isValid(glm::vec3 pos)
{
	return my_world.enforceRules(pos);
}

glm::vec3 Player::isValid(glm::vec3 new_pos, float r, glm::vec3 old_pos)
{

	return my_world.verified(new_pos, r, old_pos, &velocityVec);
}

void Player::Jump(void)
{
	if (jumps < max_jumps) {
		jumps += 1;
		jump_vel += jump_height;
	}
}

void Player::setForces(glm::vec3 vv, glm::vec3 gv)
{
	velocityVec = vv;
	gravityVec = gv;
}
