/*******************************************************************************
* Name            : MainDriver.cpp
* Project         : DungeonMap
* Module          : App
* Description     : Main driver of Dungeon Mapper
* Copyright       : Open Source
* Creation Date   : 5/14/2018
* Original Author : Tanner Voas
*
******************************************************************************/

#include <omp.h>
#include <cstdio>
#include <iostream>
#include <fstream>


#include "glad/glad.h"  // Include order can matter here
#ifdef __APPLE__
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_opengl.h>
#else
    #include <SDL.h>
    #include <SDL_opengl.h>
#endif

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "./World.h"
#include "./Camera.h"
#include "./Block.h"
#include "./settings.h"
#include "./bvh.h"

bool saveOutput = false;
float timePast = 0;

// Resolution Controls - Planned change to settings file
#define RESOLUTION 1
#if RESOLUTION == 1  // Small Test
    int screen_width = 800;
    int screen_height = 600;
#endif
#if RESOLUTION == 2  // Large Test
    int screen_width = 1600;
    int screen_height = 1200;
#endif
#if RESOLUTION == 3  // 2K Ultra Widescreen
    int screen_width = 3440;
    int screen_height = 1440;
#endif
#if RESOLUTION == 4  // Half Ultra Widescreen
    int screen_width = 1720;
    int screen_height = 720;
#endif
#if RESOLUTION == 5  // 1K Ultra Widescreen
    int screen_width = 2560;
    int screen_height = 1080;
#endif
#if RESOLUTION == 6  // HD
    int screen_width = 1920;
    int screen_height = 1080;
#endif

bool DEBUG_ON = SHOW_SHADER_LOAD;

bool fullscreen = false;
bool hardmode = false;

// Function Declaration
void Win2PPM(int width, int height);
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
void drawGeometry(int shaderProgram, World my_world, float time, Camera p);
void create_cube_map(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right, GLuint* tex_cube);
bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);

// Function Definitions
float rand01() {
    return rand() / static_cast<float>(RAND_MAX);
}
void drawGeometry(int shaderProgram, World my_world, float time, Camera p) {
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniTexScale = glGetUniformLocation(shaderProgram, "texScale");
    glm::vec2 texScale = glm::vec2(1, 1);  // Gets view matrix from Camera
    glUniform2fv(uniTexScale, 1, glm::value_ptr(texScale));

    int loc = 0;
    // Skybox
    int type = 0;
    glm::mat4 model;
    glDepthMask(GL_FALSE);
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, type);  // Set texture ID to use
    glDrawArrays(GL_TRIANGLES, loc, loc + my_world.getVerticeCount(0));  // (Primitive Type, Start Vertex, End Vertex)
    glDepthMask(GL_TRUE);
    loc += my_world.getVerticeCount(0);

    // Blocks
    int possible_hit_counts = 0;
    Block* possible_hits = my_world.my_bv.check_ray(Triplet(p.position.x, p.position.y, p.position.z), FAR * 2.0f, &possible_hit_counts);  // Need to apply this to shadows.
    glm::mat4* models = new glm::mat4[possible_hit_counts];  // Will store our model matrices
    for (int i = 0; i < possible_hit_counts; i++) {  // for all objects
        Block* block = &my_world.blocks[possible_hits[i].block_loc];
        if (block->feature_type == 2) {
            block->advanceTimeStep();
        }
        models[i] = block->getModelMatrix(0);  // Get matrix for item i in world
        texScale = glm::vec2(block->hx, block->hz);  // Gets view matrix from Camera
        glUniform2fv(uniTexScale, 1, glm::value_ptr(texScale));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(models[i]));
        glUniform1i(uniTexID, block->feature_type + 1);  // Set texture ID to use
        glDrawArrays(GL_TRIANGLES, loc, loc + my_world.getVerticeCount(1));  // (Primitive Type, Start Vertex, End Vertex)
    }
    loc += my_world.getVerticeCount(1);
    // Rolling Ball
    texScale = glm::vec2(1, 1);  // Gets view matrix from Camera
    glUniform2fv(uniTexScale, 1, glm::value_ptr(texScale));
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(p.getModelMatrix()));
    glUniform1i(uniTexID, 5);  // Set texture ID to use
    glDrawArrays(GL_TRIANGLES, loc, loc + my_world.getVerticeCount(2));  // (Primitive Type, Start Vertex, End Vertex)
    free(models);
    free(possible_hits);
}
void create_cube_map(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right, GLuint* tex_cube) {
    // generate a cube-map texture to hold all the sides
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, tex_cube);

    // load each image and copy into a side of the cube-map texture
    load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);
    load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
    load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
    load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
    load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
    load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);

    // format cube map texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    SDL_Surface* surface = SDL_LoadBMP(file_name);
    if (surface == NULL) {  // If it failed, print the error
        printf("Error: \"%s\"\n", SDL_GetError()); return 1;
    }
    // non-power-of-2 dimensions check
    if ((surface->w & (surface->w - 1)) != 0 || (surface->h & (surface->h - 1)) != 0) {
        fprintf(stderr,
            "WARNING: image %s is not power-of-2 dimensions\n",
            file_name);
    }
    // copy image data into 'target' side of cube map
    printf("%d: %d %d | %d\n", side_target, surface->w, surface->h, surface->pixels);
    glTexImage2D(side_target, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);
    return true;
}
// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile) {
    FILE *fp;
    int64_t length;
    char *buffer;

    // open the file containing the text of the shader code
    fp = fopen(shaderFile, "r");

    // check for errors in opening the file
    if (fp == NULL) {
        printf("can't open shader source file %s\n", shaderFile);
        return NULL;
    }

    // determine the file size
    fseek(fp, 0, SEEK_END);  // move position indicator to the end of the file;
    length = ftell(fp);  // return the value of the current position

                         // allocate a buffer with the indicated number of bytes, plus one
    buffer = new char[length + 1];

    // read the appropriate number of bytes from the file
    fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
    fread(buffer, 1, length, fp);  // read all of the bytes

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
    } else if (DEBUG_ON) {
        printf("Vertex Shader:\n=====================\n");
        printf("%s\n", vs_text);
        printf("=====================\n\n");
    }
    if (fs_text == NULL) {
        printf("Failed to read from fragent shader file %s\n", fShaderFileName);
        exit(1);
    } else if (DEBUG_ON) {
        printf("\nFragment Shader:\n=====================\n");
        printf("%s\n", fs_text);
        printf("=====================\n\n");
    }

    // Load Vertex Shader
    const char *vv = vs_text;
    glShaderSource(vertex_shader, 1, &vv, NULL);  // Read source
    glCompileShader(vertex_shader);  // Compile shaders

                                    // Check for errors
    GLint  compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printf("Vertex shader failed to compile:\n");
        if (DEBUG_ON) {
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

    // Check for Errors
    if (!compiled) {
        printf("Fragment shader failed to compile\n");
        if (DEBUG_ON) {
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
void Win2PPM(int width, int height) {
    char outdir[10] = "out/";  // Must be defined!
    int i, j;
    FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;

    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for image\n");
    }

    /* Open the file */
    snprintf(fname, sizeof(fname), "%simage_%04d.ppm", outdir, counter);
    if ((fptr = fopen(fname, "w")) == NULL) {
        fprintf(stderr, "ERROR: Failed to open file for window capture\n");
    }

    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

    /* Write the PPM file */
    fprintf(fptr, "P6\n%d %d\n255\n", width, height);
    for (j=height-1; j >= 0; j--) {
        for (i=0; i < width; i++) {
            fputc(image[3*j*width+3*i+0], fptr);
            fputc(image[3*j*width+3*i+1], fptr);
            fputc(image[3*j*width+3*i+2], fptr);
        }
    }

    free(image);
    fclose(fptr);
    counter++;
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

                                                         // Create a context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // OpenGL functions using glad library
    if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("\nOpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n\n", glGetString(GL_VERSION));
    } else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }


    float* vertices;

    // Texture Info
    int num_tex = 1;
    char* textures[] = {
        "brick.bmp"
    };
    char* texture_destinations[] = {
        "tex0",
        "tex1"
    };

    SDL_Surface** surfaces = reinterpret_cast<SDL_Surface**>(malloc(num_tex * sizeof(SDL_Surface*)));
    GLuint* tex = new GLuint[num_tex + 1];
    // create_cube_map("mp_drakeq/drakeq_ft.bmp", "mp_drakeq/drakeq_bk.bmp", "mp_drakeq/drakeq_up.bmp", "mp_drakeq/drakeq_dn.bmp", "mp_drakeq/drakeq_lf.bmp", "mp_drakeq/drakeq_rt.bmp", &tex[0]);

    for (int i = 0; i < num_tex; i++) {
        //// Allocate Texture i ///////
        surfaces[i] = SDL_LoadBMP(textures[i]);
        if (surfaces[i] == NULL) {  // If it failed, print the error
            printf("Error: \"%s\"\n", SDL_GetError()); return 1;
        }
        glGenTextures(1, &tex[i + 1]);

        glActiveTexture(GL_TEXTURE0 + i + 1);
        glBindTexture(GL_TEXTURE_2D, tex[i + 1]);

        // What to do outside 0-1 range
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load the texture into memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaces[i]->w, surfaces[i]->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surfaces[i]->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        SDL_FreeSurface(surfaces[i]);
        //// End Allocate Texture ///////
    }

    // free(surfaces);

    // Build a Vertex Array Object. This stores mapping from shader inputs/attributes to VBO data
    GLuint vao;
    glGenVertexArrays(1, &vao);  // Create a VAO
    glBindVertexArray(vao);  // Bind the above created VAO to the current context
                             // Allocate memory on the graphics card to store geometry (vertex buffer object)
    GLuint vbo;
    glGenBuffers(1, &vbo);  // Create 1 buffer called vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);  // Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, my_world.total_elements * sizeof(float), vertices, GL_STATIC_DRAW);  // upload vertices to vbo

    int texturedShader = InitShader("vertexTex.glsl", "fragmentTex.glsl");

    int phongShader = InitShader("vertex.glsl", "fragment.glsl");

    // Tell OpenGL how to set fragment shader input
    GLint posAttrib = glGetAttribLocation(texturedShader, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(5 * sizeof(float)));
    glEnableVertexAttribArray(normAttrib);

    GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(0);  // Unbind the VAO once we have set all the attributes

    glEnable(GL_DEPTH_TEST);
    // Make proj matrice once
    glm::mat4 proj = glm::perspective(3.14f / 4, aspect, (static_cast<float>(TEN_TIMES_NEAR)) / 10.f, static_cast<float>(FAR));  // FOV, aspect, near, far
    GLint uniProj = glGetUniformLocation(texturedShader, "proj");
    // Event Loop (Loop forever processing each event as fast as possible)
    SDL_Event windowEvent;
    bool quit = false;

    GLint uniEye = glGetUniformLocation(texturedShader, "eyePos");

    while (!quit) {
        while (SDL_PollEvent(&windowEvent)) {
            // List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
            // Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
            if (windowEvent.type == SDL_QUIT) {
                quit = true;  // Exit event loop
            }
            if ((windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) || self.won) {
                quit = true;  // Exit event loop
            }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) {  // If "f" is pressed
                fullscreen = !fullscreen;
                SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);  // Set to full screen
            }
        }
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

        // Applies character movement
        if (z_ != 0) {
            self.stepForward(sprint*z_, last);
        }
        if (x_ != 0) {
            self.stepRight(sprint*x_, last);
        }
        if (y_ != 0) {
            self.stepUp(sprint*y_, last);
        }

        // Clear the screen to default color
        glClearColor(0.f, 0.f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(texturedShader);  // Set the active shader (only one can be used at a time)

        glm::mat4 view = self.getViewMatrix(sprint);  // Gets view matrix from Camera
        GLint uniView = glGetUniformLocation(texturedShader, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        glm::vec3 self_point = self.point;  // Gets view matrix from Camera
        GLint uniPoint = glGetUniformLocation(texturedShader, "point");
        glUniform3fv(uniPoint, 1, glm::value_ptr(self_point));
        // printf("Point: %f %f %f\n", self_point.x, self_point.y, self_point.z);

        for (int i = 0; i < num_tex + 1; i++) {  // Activate textures
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, tex[i]);
            glUniform1i(glGetUniformLocation(texturedShader, texture_destinations[i]), i);
        }

        glBindVertexArray(vao);  // Bind the VAO for the shader(s) we are using

        drawGeometry(texturedShader, my_world, timePast, self);  // Handles division of our vbo and textures
        SDL_GL_SwapWindow(window);  // Double buffering

        last = timePast;
        timePast = SDL_GetTicks() / 1000.f;
        last = timePast - last;
        if (SHOW_FRAMERATE && count % 50 == 0) {
            printf("fps (time): %f (%f)\n", 1 / (last), last);
        }
        count += 1;
    }

    // Clean Up
    glDeleteProgram(phongShader);

    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
