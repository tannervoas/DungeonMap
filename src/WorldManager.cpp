/*******************************************************************************
* Name            : WorldManager.cpp
* Project         : DungeonMap
* Module          : App
* Description     : Handles All Aspect of the world and sends information to MainDriver to render
* Copyright       : Tanner Voas.
* Creation Date   : 5/17/2018
* Original Author : Tanner Voas
*
******************************************************************************/
#include "./WorldManager.h"
#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>

// Main constructor.

WorldManager::WorldManager(void) {
    width = 0;
    height = 0;
}

WorldManager::WorldManager(char * file) {
    constructed = 0;
    objects = reinterpret_cast<Object*>(malloc(sizeof(Object) * MAX_OBJECT_COUNT));
    object_count = 0;
    FILE *fp;
    char line[MAX_FILE_LINE_LENGTH];  // Assumes no line is longer than MAX_FILE_LINE_LENGTH characters!

    std::string fileName = file;

    // open the file containing the scene description
    fp = fopen(fileName.c_str(), "r");

    // check for errors in opening the file
    if (fp == NULL) {
        printf("Can't open file '%s'\n", fileName.c_str());
        exit(-1);  // Exit
    }
    // Loop through reading each line
    while (fgets(line, sizeof(line), fp)) {  // Assumes no line is longer than 1024 characters!
        char command[100];
        int fieldsRead = sscanf(line, "%s ", command);  // Read first word in the line (i.e., the command type)
        if (fieldsRead < 1) {  // No command read. Blank line
            continue;
        } else if (line[0] == '%') {  //Comment Line
            continue;
        } else if (line[0] == '0') {
            if (object_count < MAX_OBJECT_COUNT) {
                float x, y, z, hx, hy, hz, rx, ry, rz;
                sscanf(line, "0 %f %f %f %f %f %f %f %f %f", &x, &y, &z, &hx, &hy, &hz, &rx, &ry, &rz);
                objects[object_count] = Object(x, y, z, hx, hy, hz, rx, ry, rz);
                objects[object_count].object_loc = object_count;
                object_count += 1;
            } else {
                printf("To many objects\n");
            }
        } else if (line[0] == '1') {  // Camera | 1 x y z - Camera
            sscanf(line, "1 %d %d %d", &startx, &starty, &startz);
        } else {}  // Makes rest of maps objects
    }
    // my_bv = BVH(objects, object_count);
}

WorldManager::WorldManager(int width_, int height_) {  // Generates dynamic map. Not done yet.
    width = width_;
    height = height_;
}

float * WorldManager::BuildVertices(void) {
    if (constructed == 0) {
        total_elements = 0;
        total_count = object_count;
        elements = new int[total_count];
        numTris = new int[total_count];
        float** tmp_store = static_cast<float**>(malloc((total_count) * sizeof(float*)));
        int i = 0;
        int i_hold = i;
        int elements_hold = total_elements;
        /* No Skybox Currently
        float points[] = {  // Skybox is an inverted cube.
            -10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,
            -10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,
            -10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,

            -10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,
            -10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            -10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,
            -10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,
            -10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            -10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,

            10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,

            -10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,
            -10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,
            -10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,

            -10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            -10.0f,  10.0f,  10.0f, 0, 0, 0, 0, 0,
            -10.0f,  10.0f, -10.0f, 0, 0, 0, 0, 0,

            -10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            -10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f, -10.0f, 0, 0, 0, 0, 0,
            -10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0,
            10.0f, -10.0f,  10.0f, 0, 0, 0, 0, 0
        };
        Object bb = Object(0, 0, 0, 0, 0, 0, 0, 0, 0);
        tmp_store[0] = bb.getModelData();
        for (int j = 0; j < 288; j++) {  // NO....
            if (j % 8 < 3)
                tmp_store[0][j] = tmp_store[0][j] * 20.0f;
        }
        elements[0] = 288;
        numTris[0] = 36;
        total_elements += elements[0];
        */
        // Calculating Object info
        if (object_count > 0) {  // Get First Object info
            Object obj = Object(0, 0, 0, 0, 0, 0, 0, 0, 0);
            tmp_store[0] = obj.getModelData();
            elements[0] = obj.getElements();
            numTris[0] = obj.getVerticeCount();
            total_elements += elements[0];
        }
        // Done reading in info. Now merge.
        modelData = new float[total_elements];
        total_elements = 0;
        for (i = 0; i < 1; i++) {
            std::copy(tmp_store[i], tmp_store[i] + elements[i], modelData + total_elements);
            total_elements += elements[i];
        }
        constructed = 1;
    }
    return modelData;
}

int* WorldManager::getElements(void) {
    return elements;
}

int WorldManager::getElements(int index) {
    return elements[index];
}

int* WorldManager::getVerticeCount(void) {
    return numTris;
}

int WorldManager::getVerticeCount(int index) {
    return numTris[index];
}

glm::mat4 WorldManager::getModelMatrix(int i, float time) {
    if (i >= 0 && i < object_count) {
        return objects[i].getModelMatrix(time);
    }
    return glm::mat4();
}

char WorldManager::getType(int index) {
    if (index >= 0 && index < object_count) {
        return 0;  // objects[i].model;
    }
    return -1;
}

char WorldManager::getIndex(int index) {
    if (index >= 0 && index < object_count) {
        return index;
    }
    return -1;
}
