/*
* Author: Tanner Voas
* Notes: Setting page for our code. Can control most of the features in the code from this page.
*/
#ifndef SETTINGS_INCLUDED
#define SETTINGS_INCLUDED

#define E_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f

#define FLIGHT 1 //Enables flight (Q and E key)
#define SOLID 1 //Enables solid blocks. Without this we will fall through blocks
#define MAX_THREADS 6 //No parralel. Deprecated, although could be added again.
#define TEN_TIMES_NEAR 3 //Sets our near value (i.e 3 -> 0.3 new for our projection matrix)
#define FAR 1000 //Our far vlue for projection
#define TERMINAL_VELOCITY 100 //Speed limit of our player. Velocity cant pass this (if sprinting it can go SPRINT * This)
#define SPRINT_SPEED 10.0f //Effect of movement and speed limit while sprinting.
#define GLOBAL_TIME_STEP 0.01f //This isnt time based because of issues i was getting earlier. Thus each time we assume this much time passed. This should be fixed.
#define MAX_BLOCKS 1000000 //Block limit. If you want more than this then increase this number.
#define RADIUS 2.0f //Radius at which it draws our rolling ball with (RADIUS * TEN_TIMES_NEW / 10) is our actual radius we use.

//BVH Settings
#define BRANCHING_FACTOR 2 //Branching factor for BVH. Breaks if it isnt 2, not sure why.
#define USE_BVH 1 //Turns off BVH. Never actually tested it with it off though.

//Debugging Settings
#define SHOW_CLUSTERING_TREE 0 //Shows clustering of our BVH.
#define SHOW_SHADER_LOAD false //Turns shader loading messages on or off
#define SHOW_FRAMERATE 1 //Causes current framerate to be printed out
#endif


/*Current Issues
*/
