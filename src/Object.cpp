/*
* Author: Tanner Voas
* Notes: Block element of our scene. No complicated behavior and just blocks user.
*/
#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Object.h"

Object::Object(void)
{
}

Object::Object(float x_, float y_, float z_, float hx_, float hy_, float hz_, float rx_, float ry_, float rz_)
{
    x = x_;
    y = y_;
    z = z_;
    hx = hx_;
    hy = hy_;
    hz = hz_;
    rx = rx_;
    ry = ry_;
    rz = rz_;
    elements = 0;
    blockModel = glm::translate(blockModel, glm::vec3(x, y, z));
    blockModel = glm::rotate(blockModel, rx, glm::vec3(1.0f, 0.f, 0.f));
    blockModel = glm::rotate(blockModel, ry, glm::vec3(0.f, 1.0f, 0.f));
    blockModel = glm::rotate(blockModel, rz, glm::vec3(0.f, 0.f, 1.0f));
    blockModel = glm::scale(blockModel, glm::vec3(hx, hy, hz));
    blockInvModel = glm::inverse(blockModel);
    noTransBlockModel = glm::rotate(noTransBlockModel, rx, glm::vec3(1.0f, 0.f, 0.f));
    noTransBlockModel = glm::rotate(noTransBlockModel, ry, glm::vec3(0.f, 1.0f, 0.f));
    noTransBlockModel = glm::rotate(noTransBlockModel, rz, glm::vec3(0.f, 0.f, 1.0f));
    noTransBlockModel = glm::scale(noTransBlockModel, glm::vec3(hx, hy, hz));
    noTransInvBlockModel = glm::inverse(noTransBlockModel);
    first_cent = 1;
    object_loc = 0;
}

glm::mat4 Object::getModelMatrix(float time)
{
    return blockModel;
}

glm::mat4 Object::getInvModelMatrix(void)
{
    return blockInvModel;
}

float * Object::getModelData()
{
    if (elements == 0) {
        std::ifstream modelFile;
        modelFile.open("models/teapot.txt");
        modelFile >> elements;
        modelData = new	float[elements];
        for (int i = 0; i < elements; i++) {
            modelFile >> modelData[i];
        }
        numTris = elements / 8;
    }
    return modelData;
}

int Object::getElements(void)
{
    return elements;
}

int Object::getVerticeCount(void)
{
    return numTris;
}

glm::vec3 Object::center(void)
{
    if (first_cent) {
        glm::vec4 uuu = getModelMatrix(0) * glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        glm::vec4 uud = getModelMatrix(0) * glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
        glm::vec4 udu = getModelMatrix(0) * glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
        glm::vec4 duu = getModelMatrix(0) * glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
        glm::vec4 udd = getModelMatrix(0) * glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
        glm::vec4 ddu = getModelMatrix(0) * glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
        glm::vec4 dud = getModelMatrix(0) * glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
        glm::vec4 ddd = getModelMatrix(0) * glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
        cent = (1.0f / 8.0f) * (uuu + uud + udu + duu + udd + ddu + dud + ddd);
        rad = mag3(uuu - cent);
        float tmp_r = mag3(uud - cent);
        if (tmp_r > rad) {
            rad = tmp_r;
        }
        tmp_r = mag3(udu - cent);
        if (tmp_r > rad) {
            rad = tmp_r;
        }
        tmp_r = mag3(duu - cent);
        if (tmp_r > rad) {
            rad = tmp_r;
        }
        tmp_r = mag3(udd - cent);
        if (tmp_r > rad) {
            rad = tmp_r;
        }
        tmp_r = mag3(ddu - cent);
        if (tmp_r > rad) {
            rad = tmp_r;
        }
        tmp_r = mag3(dud - cent);
        if (tmp_r > rad) {
            rad = tmp_r;
        }
        tmp_r = mag3(ddd - cent);
        if (tmp_r > rad) {
            rad = tmp_r;
        }
    }
    return glm::vec3(cent.x, cent.y, cent.z);
}

float Object::mag3(glm::vec4 d) {
    return 1;  // glm::mag(glm::vec3(d.x, d.y, d.z));
}

float Object::radius(void)
{
    return rad;
}
