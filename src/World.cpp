/*
* Author: Tanner Voas
* Notes: Represent all objects in our scene and presents them to the MainDriver in a manageable way.
*/
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <omp.h>
#include "settings.h"
#include "World.h"
#include "Player.h"

using namespace std;

//Main constructor. 

World::World(void)
{
	width = 0;
	height = 0;
}

World::World(char * file)
{
	constructed = 0;
	blocks = (Block*)malloc(sizeof(Block) * MAX_BLOCKS);
	block_count = 0;
	FILE *fp;
	char line[1024]; //Assumes no line is longer than 1024 characters!

	string fileName = file;

	// open the file containing the scene description
	fp = fopen(fileName.c_str(), "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("Can't open file '%s'\n", fileName.c_str());
		exit(-1);  //Exit
	}
	//Loop through reading each line
	while (fgets(line, 1024, fp)) { //Assumes no line is longer than 1024 characters!
		char command[100];
		int fieldsRead = sscanf(line, "%s ", command); //Read first word in the line (i.e., the command type)
		if (fieldsRead < 1) { //No command read
							  //Blank line
			continue;
		}
		else if (line[0] == '%') {
		}
		else if (line[0] == '0') { //New Block | 0 x y z hx hy hz rx ry rz- Block
			if (block_count < MAX_BLOCKS) {
				float x, y, z, hx, hy, hz, rx, ry, rz;
				sscanf(line, "0 %f %f %f %f %f %f %f %f %f", &x, &y, &z, &hx, &hy, &hz, &rx, &ry, &rz);
				blocks[block_count] = Block(x, y, z, hx, hy, hz, rx, ry, rz);
				blocks[block_count].block_loc = block_count;
				block_count += 1;
			}
		}
		else if (line[0] == '1') { //Player | 1 x y z - Player
			sscanf(line, "1 %d %d %d", &startx, &starty, &startz);
		}
		else if (line[0] == '2') { //Velocity | 2 x y z - Velocity
			float x, y, z;
			sscanf(line, "2 %f %f %f", &x, &y, &z);
			velocityVec = glm::vec3(x, y, z);
		}
		else if (line[0] == '3') { //Gravity | 3 x y z - Gravity
			float x, y, z;
			sscanf(line, "3 %f %f %f", &x, &y, &z);
			gravityVec = glm::vec3(x, y, z);
		}
		else if (line[0] == '4') { //New Bouncy Block | %4 x y z hx hy hz rx ry rz str- Bounce Block
			if (block_count < MAX_BLOCKS) {
				float x, y, z, hx, hy, hz, rx, ry, rz, str;
				sscanf(line, "4 %f %f %f %f %f %f %f %f %f %f", &x, &y, &z, &hx, &hy, &hz, &rx, &ry, &rz, &str);
				blocks[block_count] = Block(x, y, z, hx, hy, hz, rx, ry, rz);
				blocks[block_count].feature_type = 1;
				blocks[block_count].str = str;
				blocks[block_count].block_loc = block_count;
				block_count += 1;
			}
		}
		else if (line[0] == '5') { //New Moving Block | %5 x y z hx hy hz rx ry rz vx vy vz lt ut - Moveing Block
			if (block_count < MAX_BLOCKS) {
				float x, y, z, hx, hy, hz, rx, ry, rz, vx, vy, vz, lt, ut;
				sscanf(line, "5 %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &x, &y, &z, &hx, &hy, &hz, &rx, &ry, &rz, &vx, &vy, &vz, &lt, &ut);
				blocks[block_count] = Block(x, y, z, hx, hy, hz, rx, ry, rz);
				blocks[block_count].feature_type = 2;
				blocks[block_count].vx = vx;
				blocks[block_count].vy = vy;
				blocks[block_count].vz = vz;
				blocks[block_count].lt = lt;
				blocks[block_count].ut = ut;
				blocks[block_count].block_loc = block_count;
				block_count += 1;
			}
		}
		else if (line[0] == '6') { //Win Block | %6 x y z hx hy hz rx ry rz - Win Block
			if (block_count < MAX_BLOCKS) {
				float x, y, z, hx, hy, hz, rx, ry, rz;
				sscanf(line, "6 %f %f %f %f %f %f %f %f %f", &x, &y, &z, &hx, &hy, &hz, &rx, &ry, &rz);
				blocks[block_count] = Block(x, y, z, hx, hy, hz, rx, ry, rz);
				blocks[block_count].feature_type = 3;
				blocks[block_count].block_loc = block_count;
				block_count += 1;
			}
		}
		else { //Makes rest of maps objects
		}
	}
	my_bv = BVH(blocks, block_count);
}

World::World(int width_, int height_) //Generates dynamic map. Not done yet.
{
	width = width_;
	height = height_;
}

float * World::BuildVertices(void)
{
	if (constructed == 0) {
		total_elements = 0;
		total_count = block_count;
		elements = new int[total_count];
		numTris = new int[total_count];
		float** tmp_store = (float**)malloc((total_count) * sizeof(float*));
		int i = 0;
		int i_hold = i;
		int elements_hold = total_elements;
		float points[] = { //Skybox is an inverted cube.
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
		Block bb = Block(0, 0, 0, 0, 0, 0, 0, 0, 0);
		tmp_store[0] = bb.getModelData();
		for (int j = 0; j < 288; j++) {
			if (j % 8 < 3)
				tmp_store[0][j] = tmp_store[0][j] * 20.0f;
		}
		elements[0] = 288;
		numTris[0] = 36;
		total_elements += elements[0];
		//Calculating block info
		if (block_count > 0) { //Get Block info
			Block b = Block(0, 0, 0, 0, 0, 0, 0, 0, 0);
			tmp_store[1] = b.getModelData();
			elements[1] = b.getElements();
			numTris[1] = b.getVerticeCount();
			total_elements += elements[1];
		}
		//Player Sphere info
		Player p = Player();
		tmp_store[2] = p.getModelData();
		elements[2] = p.getElements();
		numTris[2] = p.getVerticeCount();
		total_elements += elements[2];
		//Done reading in info. Now merge.
		modelData = new float[total_elements];
		total_elements = 0;
		for (i = 0; i < 3; i++) {
			copy(tmp_store[i], tmp_store[i] + elements[i], modelData + total_elements);
			total_elements += elements[i];
		}
		constructed = 1;
	}
	return modelData;
}

int* World::getElements(void)
{
	return elements;
}

int World::getElements(int index)
{
	return elements[index];
}

int* World::getVerticeCount(void)
{
	return numTris;
}

int World::getVerticeCount(int index)
{
	return numTris[index];
}

glm::mat4 World::getModelMatrix(int i, float time)
{
	if (i < block_count) {
		//Blocks
		return blocks[i].getModelMatrix(time);
	}
	return glm::mat4();
}

char World::enforceRules(glm::vec3 pos)
{
	char valid = 1;
	if (SOLID) {
		int possible_hit_counts = 0;
		Block* possible_hits = my_bv.check_ray(Triplet(pos.x, pos.y, pos.z), TEN_TIMES_NEAR / 10.0f, &possible_hit_counts); //Need to apply this to shadows.
		//printf("Pos Hits: %d\n", possible_hit_counts);
		//#pragma omp parallel for schedule(dynamic) num_threads(MAX_THREADS) //parallel call
		for (int i = 0; i < possible_hit_counts; i++) {
			if (possible_hits[i].inside(possible_hits[i].getInvModelMatrix() * glm::vec4(pos, 1.0f))) {
				valid = 0;
			}
		}
		free(possible_hits);
	}
	return valid;
}

glm::vec3 World::verified(glm::vec3 new_pos, float r, glm::vec3 old_pos, glm::vec3 * vv)
{
	if (SOLID) {
		int possible_hit_counts = 0;
		Block* possible_hits = my_bv.check_ray(Triplet(new_pos.x, new_pos.y, new_pos.z), TEN_TIMES_NEAR / 10.0f, &possible_hit_counts);
																															
		for (int i = 0; i < possible_hit_counts; i++) {
			Block* block = &blocks[possible_hits[i].block_loc];
			glm::vec4 tmp = block->getInvModelMatrix() * glm::vec4(new_pos, 1.0f);
			glm::vec4 tmp_vv = (block->noTransInvBlockModel * glm::vec4((*vv), 1.0f));
			new_pos = block->verified(tmp, tmp, r, block->getInvModelMatrix() * glm::vec4(old_pos, 1.0f), &tmp_vv);
			tmp_vv = block->noTransBlockModel * tmp_vv;
			vv->x = tmp_vv.x;
			vv->y = tmp_vv.y;
			vv->z = tmp_vv.z;
		}
		free(possible_hits);
	}
	return new_pos;
}

char World::getType(int index)
{
	if (index < block_count) {
		return 1;
	}
	return -1;
}

char World::getIndex(int index)
{
	if (index < block_count) {
		return index;
	}
	return -1;
}
