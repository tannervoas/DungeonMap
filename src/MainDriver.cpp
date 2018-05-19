/*******************************************************************************
* Name            : MainDriver.cpp
* Project         : DungeonMap
* Module          : App
* Description     : Implementation of Dungeon Mapper
* Copyright       : Tanner Voas.
* Creation Date   : 5/17/2018
* Original Author : Tanner Voas
*
******************************************************************************/
#include "glad/glad.h"  // Include order can matter here
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "./settings.h"
#include "./WorldManager.h"
#include "./Camera.h"

bool saveOutput = false;
float timePast = 0;

bool fullscreen = false;
int screen_width = 800;
int screen_height = 600;

void Win2PPM(int width, int height);

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile) {
    FILE *fp;
    long length;
    char *buffer;

    // open the file containing the text of the shader code
    fp = fopen(shaderFile, "r");

    // check for errors in opening the file
    if (fp == NULL) {
        printf("can't open shader source file %s\n", shaderFile);
        return NULL;
    }

    // determine the file size
    fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
    length = ftell(fp);  // return the value of the current position

                         // allocate a buffer with the indicated number of bytes, plus one
    buffer = new char[length + 1];

    // read the appropriate number of bytes from the file
    fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
    fread(buffer, 1, length, fp); // read all of the bytes

                                  // append a NULL character to indicate the end of the string
    buffer[length] = '\0';

    // close the file
    fclose(fp);

    // return the string
    return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName) {
    GLuint vertex_shader, fragment_shader;
    GLchar *vs_text, *fs_text;
    GLuint program;

    // check GLSL version
    printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Create shader handlers
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read source code from shader files
    vs_text = readShaderSource(vShaderFileName);
    fs_text = readShaderSource(fShaderFileName);

    // error check
    if (vs_text == NULL) {
        printf("Failed to read from vertex shader file %s\n", vShaderFileName);
        exit(1);
    }
    else if (DEBUG_MAINDRIVER) {
        printf("Vertex Shader:\n=====================\n");
        printf("%s\n", vs_text);
        printf("=====================\n\n");
    }
    if (fs_text == NULL) {
        printf("Failed to read from fragent shader file %s\n", fShaderFileName);
        exit(1);
    }
    else if (DEBUG_MAINDRIVER) {
        printf("\nFragment Shader:\n=====================\n");
        printf("%s\n", fs_text);
        printf("=====================\n\n");
    }

    // Load Vertex Shader
    const char *vv = vs_text;
    glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
    glCompileShader(vertex_shader); // Compile shaders

                                    // Check for errors
    GLint  compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printf("Vertex shader failed to compile:\n");
        if (DEBUG_MAINDRIVER) {
            GLint logMaxSize, logLength;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
            printf("printing error message of %d bytes\n", logMaxSize);
            char* logMsg = new char[logMaxSize];
            glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
            printf("%d bytes retrieved\n", logLength);
            printf("error message: %s\n", logMsg);
            delete[] logMsg;
        }
        exit(1);
    }

    // Load Fragment Shader
    const char *ff = fs_text;
    glShaderSource(fragment_shader, 1, &ff, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);

    //Check for Errors
    if (!compiled) {
        printf("Fragment shader failed to compile\n");
        if (DEBUG_MAINDRIVER) {
            GLint logMaxSize, logLength;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
            printf("printing error message of %d bytes\n", logMaxSize);
            char* logMsg = new char[logMaxSize];
            glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
            printf("%d bytes retrieved\n", logLength);
            printf("error message: %s\n", logMsg);
            delete[] logMsg;
        }
        exit(1);
    }

    // Create the program
    program = glCreateProgram();

    // Attach shaders to program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    // Link and set program to use
    glLinkProgram(program);

    return program;
}

void drawGeometry(int shaderProgram, WorldManager myWorld, float time, Camera cam) {

    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    int loc = 0;
    for (int i = 0; i < myWorld.total_count; i++) {
        glm::mat4 model = myWorld.objects[i].getModelMatrix(0);  //Fix this zero thing
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, loc, loc + myWorld.getVerticeCount(i));
        loc += myWorld.getVerticeCount(i);
    }
    /*
    int loc = 0;
    //Skybox
    int type = 0;
    glm::mat4 model;
    glDepthMask(GL_FALSE);
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, type); //Set texture ID to use
    glDrawArrays(GL_TRIANGLES, loc, loc + my_world.getVerticeCount(0)); //(Primitive Type, Start Vertex, End Vertex)
    glDepthMask(GL_TRUE);
    loc += my_world.getVerticeCount(0);

    //Blocks
    int possible_hit_counts = 0;
    Block* possible_hits = my_world.my_bv.check_ray(Triplet(p.position.x, p.position.y, p.position.z), FAR * 2.0f, &possible_hit_counts); //Need to apply this to shadows.
    glm::mat4* models = new glm::mat4[possible_hit_counts]; //Will store our model matrices
    for (int i = 0; i < possible_hit_counts; i++) { //for all objects
        Block* block = &my_world.blocks[possible_hits[i].block_loc];
        if (block->feature_type == 2) {
            block->advanceTimeStep();
        }
        models[i] = block->getModelMatrix(0); //Get matrix for item i in world
        texScale = glm::vec2(block->hx, block->hz); //Gets view matrix from player
        glUniform2fv(uniTexScale, 1, glm::value_ptr(texScale));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(models[i]));
        glUniform1i(uniTexID, block->feature_type + 1); //Set texture ID to use
        glDrawArrays(GL_TRIANGLES, loc, loc + my_world.getVerticeCount(1)); //(Primitive Type, Start Vertex, End Vertex)

    }
    loc += my_world.getVerticeCount(1);
    //Rolling Ball
    texScale = glm::vec2(1, 1); //Gets view matrix from player
    glUniform2fv(uniTexScale, 1, glm::value_ptr(texScale));
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(p.getModelMatrix()));
    glUniform1i(uniTexID, 5); //Set texture ID to use
    glDrawArrays(GL_TRIANGLES, loc, loc + my_world.getVerticeCount(2)); //(Primitive Type, Start Vertex, End Vertex)
    free(models);
    free(possible_hits);
    */
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);  // Initialize Graphics (for OpenGL)

    // Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create a window (offsetx, offsety, width, height, flags)
    SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
    float aspect = screen_width / static_cast<float>(screen_height);  // aspect ratio (needs to be updated if the window is resized)

    // The above window cannot be resized which makes some code slightly easier.
      // Below show how to make a full screen window or allow resizing
      // SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
      // SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
      // SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen



      // Create a context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("\nOpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n\n", glGetString(GL_VERSION));
    } else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }

    // Load in world map
    WorldManager myWorld = WorldManager("sample.map");
    float* vertices = myWorld.BuildVertices();


    // Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
    GLuint vao;
    glGenVertexArrays(1, &vao);  // Create a VAO
    glBindVertexArray(vao);  // Bind the above created VAO to the current context

    // Allocate memory on the graphics card to store geometry (vertex buffer object)
    GLuint vbo[1];
    glGenBuffers(1, vbo);  // Create 1 buffer called vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);  // Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, myWorld.total_elements * sizeof(float), vertices, GL_STATIC_DRAW);  // upload vertices to vbo
    // GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
    // GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

    int texturedShader = InitShader("vertexTex.glsl", "fragmentTex.glsl");

    // Tell OpenGL how to set fragment shader input
    GLint posAttrib = glGetAttribLocation(texturedShader, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(normAttrib);

    GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);  // Unbind the VAO

    // Maybe we need a second VAO, e.g., some of our models are stored in a second format
    // GLuint vao2;
      // glGenVertexArrays(1, &vao2); //Create the VAO
    // glBindVertexArray(vao2); //Bind the above created VAO to the current context
    //   Creat VBOs ...
    //   Set-up attributes ...
    // glBindVertexArray(0); //Unbind the VAO

    glEnable(GL_DEPTH_TEST);
    glm::mat4 proj = glm::perspective(3.14f / 4, aspect, (float) 1.0, (float) 10.0); //FOV, aspect, near, far
    GLint uniProj = glGetUniformLocation(texturedShader, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
    Camera cam = Camera(10, 10, 0, screen_width, screen_height);
    int dragging = 0;
    float tmpx = 0.0f;
    float tmpy = 0.0f;
    // Event Loop (Loop forever processing each event as fast as possible)
    SDL_Event windowEvent;
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) break;
            // List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
            // Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
                break;  // Exit event loop
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) {  // If "f" is pressed
                fullscreen = !fullscreen;
                SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);  // Toggle fullscreen
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_w) {  //Step forward
                cam.stepForward(1, 1);
            }
            else if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_s) { //Step back
                cam.stepForward(1, 1);
            }
            else if (windowEvent.type == SDL_MOUSEBUTTONDOWN) { //Start rotate view
                dragging = 1;
            }
            else if (windowEvent.type == SDL_MOUSEBUTTONUP) { //End rotate view
                dragging = 0;
            }
            else if (windowEvent.type == SDL_MOUSEMOTION) { //Apply rotation
                tmpx = windowEvent.motion.x;
                tmpy = windowEvent.motion.y;
            }
        }
        float dragTime = 1.0f;
        if (dragging) {
            _LOG("HELLO\n");
            cam.Rotate(tmpx, tmpy); //Applies rotation to player
        }
        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!saveOutput) timePast = SDL_GetTicks() / 1000.f;
        if (saveOutput) timePast += .07;  // Fix framerate at 14 FPS

        glm::mat4 view = cam.getViewMatrix();
        GLint uniView = glGetUniformLocation(texturedShader, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(vao);
        drawGeometry(texturedShader, myWorld, timePast, cam); //Handles division of our vbo and textures
        if (saveOutput) Win2PPM(screen_width, screen_height);

        SDL_GL_SwapWindow(window);  // Double buffering
    }

    // Clean Up
    glDeleteProgram(texturedShader);

    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}


// Write out PPM image from screen
void Win2PPM(int width, int height) {
    char outdir[10] = "out/";  // Must be defined!
    int i, j;
    FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;
    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3 * width*height * sizeof(char));
    if (image == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for image\n");
    }
    /* Open the file */
    snprintf(fname, sizeof(fname), "%simage_%04d.ppm", outdir, counter);
    if ((fptr = fopen(fname, "w")) == NULL) {
        fprintf(stderr, "ERROR: Failed to open file to write image\n");
    }
    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
    /* Write the PPM file */
    fprintf(fptr, "P6\n%d %d\n255\n", width, height);
    for (j = height - 1; j >= 0; j--) {
        for (i = 0; i < width; i++) {
            fputc(image[3 * j*width + 3 * i + 0], fptr);
            fputc(image[3 * j*width + 3 * i + 1], fptr);
            fputc(image[3 * j*width + 3 * i + 2], fptr);
        }
    }
    free(image);
    fclose(fptr);
    counter++;
}
