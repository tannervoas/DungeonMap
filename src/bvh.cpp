/*
* Author: Tanner Voas
* Notes: BVH tree which is used to speed up processing of scenes with large number of features.
* Offers support for BVH with any branching factor from 1 to Infinity but currently only 2 works.
* havent figured out why it breaks for more than 2 but this isnt a big issue.
* Uses K-means clustering to generate each partition of our feature sets based on there centerpoint and then
* passes each cluster into its own BVH. stops recurring when feature_count is less than branching factor.
*/
#include <math.h>
#include <stdlib.h>
#include "bvh.h"
#include "triplet.h"

//Main constructor. Clusters our feature set and then makes each of these clusters into a BVH.
BVH::BVH(Block* features, int feature_count)
{
	if (SHOW_CLUSTERING_TREE) {
		printf("<---- Total Features: %d ---->\n", feature_count);
	}
	if (!USE_BVH) { //Done use BVH. Just store features.
		nodes = features;
		offspring = feature_count;
	}
	else { //We arent an end BVH
		end = 0;
		offspring = BRANCHING_FACTOR;
		spec = 0;
		if (feature_count > BRANCHING_FACTOR) {
			children = (BVH*)malloc(BRANCHING_FACTOR * sizeof(BVH));
			nodes = NULL;
			//Cluster our features into BRANCHING_FACTOR groups based off the features centerpoint.
			int** clusters = (int**)malloc(BRANCHING_FACTOR * sizeof(int*));
			int* cluster_counts = (int*)malloc(BRANCHING_FACTOR * sizeof(int));
			Triplet* means = (Triplet*)malloc(BRANCHING_FACTOR * sizeof(Triplet));
			int i;
			for (i = 0; i < BRANCHING_FACTOR; i++) { //Instantiate our variables to perform K-means clustering
				clusters[i] = (int*)malloc(feature_count * sizeof(int));
				cluster_counts[i] = 0;
			}
			int j;
			for (i = 0; i < feature_count; i++) { // Place initial clusters
				j = i % BRANCHING_FACTOR;
				clusters[j][cluster_counts[j]] = i;
				cluster_counts[j] += 1;
			}
			updateMeans(features, feature_count, clusters, cluster_counts, means); //Calculates initial mean
			int change = 1;
			while (change) {
				change = updateClusters(features, feature_count, clusters, cluster_counts, means); //recluster
				updateMeans(features, feature_count, clusters, cluster_counts, means); //calculate new mean again
			}
			int placed = 0;
			for (i = 0; i < BRANCHING_FACTOR; i++) {

				//Building new feature array
				Block* new_features = (Block*)malloc(sizeof(Block) * cluster_counts[i]);
				for (j = 0; j < cluster_counts[i]; j++) {
					new_features[j] = features[clusters[i][j]];
				}
				int k;
				if (cluster_counts[i] == feature_count) { //All Features  are identical and cant break up anymore. Store them all in nodes
					special(new_features, cluster_counts[i]);
				}
				else if (cluster_counts[i] != 0) { //Was able to break up features. Recursively cluster our clusters.
					children[i] = BVH(new_features, cluster_counts[i]);
				}
			}
		}
		else { //We are an end BVH
			end = 1;
			offspring = feature_count;
			children = NULL;
			nodes = (Block*)malloc(feature_count * sizeof(Block));
			int i;
			for (i = 0; i < feature_count; i++) {

				nodes[i] = features[i];
			}
		}
		updateBoundingVolume(features, feature_count);
	}
}

BVH::BVH(void)
{
}

void BVH::special(Block * features, int feature_count)
{
	end = 1;
	spec = 1;
	offspring = feature_count;
	children = NULL;
	nodes = (Block*)malloc(feature_count * sizeof(Block));
	int i;
	for (i = 0; i < feature_count; i++) {

		nodes[i] = features[i];
	}
}

void BVH::updateMeans(Block * features, int feature_count, int ** clusters, int * cluster_counts, Triplet * means)
{
	int i, j;
	for (i = 0; i < BRANCHING_FACTOR; i++) {
		Triplet new_mean = Triplet(0, 0, 0);
		for (j = 0; j < cluster_counts[i]; j++) {
			new_mean = new_mean + features[clusters[i][j]].center();
		}
		means[i] = new_mean / cluster_counts[i];
	}
}

int BVH::updateClusters(Block * features, int feature_count, int ** clusters, int * cluster_counts, Triplet * means)
{
	int iter;
	for (iter = 0; iter < BRANCHING_FACTOR; iter++) {
	}
	int changed = 0;
	//Initializae new data
	int** new_clusters = (int**)malloc(BRANCHING_FACTOR * sizeof(int*));
	int* new_cluster_counts = (int*)malloc(BRANCHING_FACTOR * sizeof(int));
	int i, j;
	for (i = 0; i < BRANCHING_FACTOR; i++) {
		new_clusters[i] = (int*)malloc(feature_count * sizeof(int));
		new_cluster_counts[i] = 0;
	}
	//Calculate new  clusters
	for (i = 0; i < feature_count; i++) {
		float min_dist = INFINITY;
		int closest_cluster;
		for (j = 0; j < BRANCHING_FACTOR; j++) {
			float dist = (features[i].center() - means[j]).getFloatMag();
			if (dist < min_dist) {
				min_dist = dist;
				closest_cluster = j;
			}
		}
		new_clusters[closest_cluster][new_cluster_counts[closest_cluster]] = i;
		new_cluster_counts[closest_cluster] += 1;
	}
	//Check for change and if they changed
	for (i = 0; i < BRANCHING_FACTOR; i++) {
		if (cluster_counts[i] != new_cluster_counts[i]) {
			changed = 1;
		}
		else {
			for (j = 0; j < cluster_counts[i]; j++) {
				if (clusters[i][j] != new_clusters[i][j]) {
					changed = 1;
				}
			}
		}
		if (changed) {
			free(clusters[i]);
			clusters[i] = new_clusters[i];
			cluster_counts[i] = new_cluster_counts[i];
		}
		else {
			free(new_clusters[i]);
		}
	}
	free(new_clusters);
	free(new_cluster_counts);
	for (iter = 0; iter < BRANCHING_FACTOR; iter++) {
	}
	return changed;
}

//Construct bounding volume of our BVH.
void BVH::updateBoundingVolume(Block * features, int feature_count)
{
	Triplet mean = Triplet(0,0,0);
	int i;
	for (i = 0; i < feature_count; i++) {
		mean = mean + features[i].center();
	}
	mean = mean / feature_count; //Ideally mean would be centered with respect to each features farthest point but instead we use just each items center.
	x = mean.x;
	y = mean.y;
	z = mean.z;
	r = 0;
	float tmp_r, sub_r;
	for (i = 0; i < feature_count; i++) { //Figure out farthest distance of each feature from our mean. Set this distance as our 'r'.
		if (features[i].feature_type == 0 || features[i].feature_type == 1 || features[i].feature_type == 2 || features[i].feature_type == 3) { //Rectangle
			tmp_r = (features[i].center() - mean).getFloatMag() + features[i].radius();
		}
		if (tmp_r > r) {
			r = tmp_r;
		}
	}
}

//Simple sphere intersect. We only care if we hit, but dont need to find out when.
int BVH::intersect(Triplet ray_d, Triplet origin)
{
	float tmp2;

	tmp2 = 4 * (pow(ray_d.x * (origin.x - x) + ray_d.y * (origin.y - y) + ray_d.z * (origin.z - z), 2) - (ray_d.x * ray_d.x + ray_d.y * ray_d.y + ray_d.z * ray_d.z) * (pow(origin.x - x, 2) + pow(origin.y - y, 2) + pow(origin.z - z, 2) - pow(r, 2)));
	if (tmp2 < 0) {
		return 0;
	}
	else {
		return 1;
	}
	return INFINITY;
}

//Simple sphere intersect. We only care if we hit, but dont need to find out when.
int BVH::inside(Triplet origin, float r_)
{
	float tmp = sqrt(pow((origin.x - x), 2) + pow((origin.y - y), 2) + pow((origin.z - z), 2));
	if (tmp > (r_ + r)) {
		return 0;
	}
	else {
		return 1;
	}
	return INFINITY;
}

Block * BVH::check_ray(Triplet origin, float r_,int * possible_hit_counts)
{
	if (!USE_BVH) { //Dont use BVH.
		*possible_hit_counts = offspring;
		return nodes;
	}
	int i, j;
	if (inside(origin, r_)) { //sphere inside our bounding volume
		if (end == 1) { //We are an end node. Return nodes
			*possible_hit_counts = offspring;
			Block* send_out = (Block*)malloc(offspring * sizeof(Block));
			for (i = 0; i < offspring; i++) {
				send_out[i] = nodes[i];
			}
			return send_out;
		}
		else { //We arent end. check_ray against our children and then union there returned arrays
			Block** childs = (Block**)malloc(BRANCHING_FACTOR * sizeof(Block*));
			int* childs_next = (int*)malloc(BRANCHING_FACTOR * sizeof(int));
			for (i = 0; i < BRANCHING_FACTOR; i++) { //Get each of our childrens possible hits
				int next_hits = 0;
				childs[i] = children[i].check_ray(origin, r_, &next_hits);
				childs_next[i] = next_hits;
				if (childs[i] != NULL) {
					*possible_hit_counts += next_hits;
				}
			}
			Block* my_nodes = (Block*)malloc(*possible_hit_counts * sizeof(Block));
			int current = 0;
			for (i = 0; i < BRANCHING_FACTOR; i++) { //Combine childrens possible hits.
				for (j = 0; j < childs_next[i]; j++) {
					my_nodes[current] = childs[i][j];
					current += 1;
				}
				free(childs[i]);
			}
			free(childs);
			free(childs_next);
			return my_nodes;
		}
	}
	return NULL;
}
