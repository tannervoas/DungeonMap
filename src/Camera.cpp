/*******************************************************************************
* Name            : Camera.cpp
* Project         : DungeonMap
* Module          : App
* Description     : Handles View Matrix
* Copyright       : Tanner Voas.
* Creation Date   : 5/17/2018
* Original Author : Tanner Voas
*
******************************************************************************/
#include "./Camera.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "./WorldManager.h"
#include "./settings.h"

glm::vec3 project(glm::vec3 source, glm::vec3 dest) {
    float mag = source.x * dest.x + source.y * dest.y + source.z * dest.z;
    return mag * dest;
}
float magnitude(glm::vec3 v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

Camera::Camera(void) {
    elements = 0;
}

Camera::Camera(int x_, int y_, int z_, int width_, int height_) {
    position = glm::vec3(x_, y_, z_);
    look = glm::vec3();
    up = glm::vec3(0, 1, 0);
    width = width_;
    height = height_;
    turn_speed = 0.05f;
    move_speed = 50.0f;
    horiz = 3.14f / 2;
    vert = 0;
    look.x = cosf(vert)*sinf(horiz);
    look.y = sinf(vert);
    look.z = cosf(vert)*cosf(horiz);
    point = glm::normalize(look);
    look = look + position;
    downLim = -1.5f;
    upLim = 1.5f;
    elements = 0;
    frotation = 0.f;
    srotation = 0.f;
    xzproj = glm::normalize(project(point, glm::vec3(1, 0, 0)) + project(point, glm::vec3(0, 0, 1)));
    right = glm::normalize(glm::cross(xzproj, up));
}

glm::mat4 Camera::getViewMatrix(void) {
    glm::mat4 view;
    glm::vec3 dis = 1.0f * up - 5.0f * point;
    view = glm::lookAt(
        position + dis,  // Cam Position
        look + dis,  // Look at point
        up);  // Up
    return view;
}

glm::mat4 Camera::getModelMatrix(void) {
    model = glm::mat4();
    return model;
}

float * Camera::getModelData() {
    if (elements == 0) {
        std::ifstream modelFile;
        modelFile.open("models/sphere.txt");
        modelFile >> elements;
        modelData = new float[elements];
        for (int i = 0; i < elements; i++) {
            modelFile >> modelData[i];
        }
        numTris = elements / 8;
    }
    return modelData;
}

int Camera::getElements(void) {
    return elements;
}

int Camera::getVerticeCount(void) {
    return numTris;
}

void Camera::Rotate(float dx, float dy) {
    horiz += -1 * turn_speed * static_cast<float>(2 * dx / width - 1);
    vert += -1 * turn_speed * static_cast<float>(2 * dy / height - 1);
    if (vert < downLim) {
        vert = downLim;
    } else if (vert > upLim) {
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

void Camera::stepForward(float dir, float time) {
    glm::vec3 amount = xzproj * (float) (1.0 * move_speed * dir);
    position = position + amount;;
    look = look + amount;
}

void Camera::stepRight(int dir, float time) {
    glm::vec3 amount = right * (float) (1.0 * move_speed * dir);
    position = position + amount;;
    look = look + amount;
}

void Camera::stepUp(int dir, float time) {
    glm::vec3 amount = up * (float) (1.0 * move_speed * dir);
    position = position + amount;;
    look = look + amount;
}
