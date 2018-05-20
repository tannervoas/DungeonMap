/*
* Author: Tanner Voas
* Notes: BVH tree which is used to speed up processing of scenes with large number of features.
* Offers support for BVH with any branching factor from 1 to Infinity but currently only 2 works.
* havent figured out why it breaks for more than 2 but this isnt a big issue.
* Uses K-means clustering to generate each partition of our feature sets based on there centerpoint and then
* passes each cluster into its own BVH. stops recurring when feature_count is less than branching factor.
*/
#ifndef BVH_INCLUDED
#define BVH_INCLUDED

#include <assert.h>
#include <stdio.h>
#include "Block.h"
#include "triplet.h"
#include "settings.h"

/**
* BVH
**/
struct BVH //Self made 3 variable vector or coordinate system.
{
	BVH* children; //BVH array which points to children BVH's in the tree structure
	Block* nodes; //Feature array which points to leaf Features in the tree structure.
	int end; //Notifies if we are at the end of the tree
	int spec;
	int offspring; //Number of children BVH has. Sometime we store more than branching factor of children in nodes
	BVH(Block* features, int feature_count); //Main constructor. Build BVH tree with K-means clustering
	BVH(void); //Default constructor

	float x, y, z, r; //Stores information for our BVH's bounding spherical volume.

	void special(Block* features, int feature_count); //Called when we have more features than our branching factor but no way
																																				 //to cluster them. i.e they have the same center value

	//Updates the means of all cluster in this BVH
	void updateMeans(Block* features, int feature_count, int** clusters, int* cluster_counts, Triplet* means);
	//Updates the cluster based on the current mean of each cluster
	int updateClusters(Block* features, int feature_count, int** clusters, int* cluster_counts, Triplet* means);
	//Calculates bounding volume of our current BVH
	void updateBoundingVolume(Block* features, int feature_count);
	//ray sphere intersect.  Modified version of features intersect_sphere.
	int intersect(Triplet ray_d, Triplet origin);
	int inside(Triplet origin, float r_);
	//recursive method that checks if a ray intersect us and then check or the children. Assemble array of possible feature that you may have hit.
	Block * check_ray(Triplet origin, float r_, int * possible_hit_counts);
};

#endif
