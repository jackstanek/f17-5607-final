#include <fstream>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glad/glad.h"

#include "game.hpp"
#include "model.hpp"
#include "map.hpp"

#define DEBUG_ON true

int screenWidth = 800;
int screenHeight = 600;
bool saveOutput = false;

int framesRendered = 0;
int lastPrint = 0;

// eric: this is my code for floating keys in hand if we want to use it
//if (key_stats[i] == KEY_IN_HAND) {
//	model = glm::translate(glm::mat4(), glm::vec3(key_position[i]));
//	model = glm::rotate(model, (float)(elapsed_time*5.0*M_PI), glm::vec3(1.0f, 1.0f, 1.0f));
//	glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
//	glUniform3fv(uniform_color, 1, glm::value_ptr(key_colors[i]));
//	glUniform1i(uniform_texID, -1);
//	glDrawArrays(GL_TRIANGLES, start, vertex_number[4]);
//}

static char* readShaderSource(const char* shaderFile);
void Win2PPM(int width, int height);
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
const float quad_data[24] = {1,1,0,1, -1,1,1,1, -1,-1,1,0,  1,-1,0,0, 1,1,0,1, -1,-1,1,0};
glm::mat4 floating_obj(glm::mat4 model, int t)
{
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.2f * std::sin(t / 750.0f)));
    model = glm::rotate(model, t / 2000.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    return model;
}
Game::Game(int diff) :
    map(Map::ParseMapFile(10 * diff)),
    mp(new ModelPool),
    render_passes(NUM_RENDER_PASSES)
{
	difficulty = diff;
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

    //Ask SDL to get a recent version of OpenGL (3 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);//

    //Create a window (offsetx, offsety, width, height, flags)
    window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

    //Create a context to draw in
    context = SDL_GL_CreateContext(window);

    //Load OpenGL extentions with GLAD
    if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("\nOpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n\n", glGetString(GL_VERSION));
    } else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
    }

    //// Allocate Texture 0 (Wood) ///////
    SDL_Surface* surface = SDL_LoadBMP("wood.bmp");
    if (surface == NULL) { //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError());
    }
    glGenTextures(1, &tex0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);

    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////

    //// Allocate Texture 1 (Brick) ///////
    surface = SDL_LoadBMP("brick.bmp");
    if (surface == NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError());
    }
    glGenTextures(1, &tex1);

    //Load the texture into memory
    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, tex1);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //How to filter
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    //// End Allocate Texture ///////

    //Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
    glGenVertexArrays(1, &model_vao); //Create a VAO
    glBindVertexArray(model_vao); //Bind the above created VAO to the current context

    //Allocate memory on the graphics card to store geometry (vertex buffer object)
    glGenBuffers(1, &vbo[VBO_MODELS]);  //Create 1 buffer called vbo

    // Load models to the GPU
    floor_id = mp->Add("models/floor.txt");
    wall_id = mp->Add("models/cube.txt");
    //char_id = mp->Add("models/sphere.txt");
    char_id = mp->AddObj("models/Gwynn.obj");
    // key_id = mp->Add("models/knot.txt");
    key_id = mp->AddObj("models/slime.obj");
    goal_id = mp->Add("models/teapot.txt");
    //goal_id = mp->AddObj("models/Gwynn.obj");
    mp->LoadToGPU(vbo[0]);

    player = map->NewPlayerAtStart(char_id);

    //GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
    //GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_TEXTURE_2D);

    texturedShader = InitShader("vertexTex.glsl", "fragmentTex.glsl");
    quadShader = InitShader("vertexQuad.glsl", "fragmentQuad.glsl");

    //Tell OpenGL how to set fragment shader input
    GLint posAttrib = glGetAttribLocation(texturedShader, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    //Attribute, vals/attrib., type, normalized?, stride, offset
    //Binds to VBO current GL_ARRAY_BUFFER
    glEnableVertexAttribArray(posAttrib);

    //GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
    //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(colAttrib);

    GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(normAttrib);

    GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

    uniView = glGetUniformLocation(texturedShader, "view");
    uniProj = glGetUniformLocation(texturedShader, "proj");

    glBindVertexArray(0); //Unbind the VAO in case we want to create a new one

    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    glGenBuffers(1, &vbo[VBO_QUAD]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_QUAD]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW); //upload vertices to vbo

    //Tell OpenGL how to set vertex shader input
    GLint quadPosAttrib = glGetAttribLocation(quadShader, "position");
    glVertexAttribPointer(quadPosAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(quadPosAttrib);

    GLint quadTexAttrib = glGetAttribLocation(quadShader, "texcoord");
    glVertexAttribPointer(quadTexAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(quadTexAttrib);

    glBindVertexArray(0); //Unbind the VAO in case we want to create a new one

    for (int i = 0; i < NUM_RENDER_PASSES; i++) {
        render_passes[i] = new RenderPass(screenWidth, screenHeight, i);
    }

    /* Unbind all framebuffers */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Game::~Game()
{
    for (auto pass : render_passes) {
        delete pass;
    }

    delete map;
    delete mp;
    delete player;
    glDeleteProgram(texturedShader);
    glDeleteProgram(quadShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &model_vao);
    SDL_GL_DeleteContext(context);
    SDL_Quit();
}

void Game::Render()
{
    time = SDL_GetTicks();
    glUseProgram(texturedShader);

    glm::mat4 view = glm::lookAt(player->CamPosition(time),//glm::vec3(1.f, -3.f, 0.7f),  //Cam Position
                                 player->LookAtPosition(time),
                                 glm::vec3(0.0f, 0.0f, 1.0f)); //Up

    /* Run each render pass to its own framebuffer */
    const static GLint uni_pass_mode = glGetUniformLocation(texturedShader, "pass_mode");
    for (auto pass : render_passes) {
        pass->Activate();
        glUniform1i(uni_pass_mode, pass->GetID());

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glBindVertexArray(model_vao);
        glUseProgram(texturedShader);

        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 0.1f, 10.0f); //FOV, aspect, near, far
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
        glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

        glBindVertexArray(model_vao);

        RenderMap();
        RenderCharacter();
    }

    /* Render the fullscreen texture quad */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(quadShader);
    glBindVertexArray(quad_vao);

    glUniform4fv(glGetUniformLocation(quadShader, "in_lightDir"), 1, glm::value_ptr(view * glm::vec4(-1, 1, -1, 0)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_passes[0]->GetTarget());
    glUniform1i(glGetUniformLocation(quadShader, "texDiffuse"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, render_passes[1]->GetTarget());
    glUniform1i(glGetUniformLocation(quadShader, "texNormal"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, render_passes[2]->GetTarget());
    glUniform1i(glGetUniformLocation(quadShader, "texPos"), 2);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    SDL_GL_SwapWindow(window); //Double buffering

    framesRendered++;

    if (time - lastPrint > 1000) {
        std::printf("%d FPS\n", framesRendered * 1000 / (time - lastPrint));
        lastPrint = time;
        framesRendered = 0;
    }
}

void Game::RenderMap()
{
    static const int uni_texid = glGetUniformLocation(texturedShader, "texID"),
        uni_model = glGetUniformLocation(texturedShader, "model"),
        uni_color = glGetUniformLocation(texturedShader, "inColor");

    static const glm::vec3 key_colors[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 1.0f)
    };

    int w = map->GetWidth(),
        h = map->GetHeight();

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            int tile = map->TileAtPoint(i, j);
            glm::mat4 model;

            /* Draw the floor */
            model = glm::translate(model, glm::vec3((float) i, (float) j, -1.0f));
            glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(uni_texid, 0);
            glDrawArrays(GL_TRIANGLES, mp->Offset(wall_id), mp->Length(wall_id));

            model = glm::mat4();
            model = glm::translate(model, glm::vec3((float) i, (float) j, 0.f));
            int render_id = 0;

            /* Render the tile */
            if (tile == TT_WALL) {
                glUniform1i(uni_texid, 1); //Set texture ID to use (-1 = no texture)
                //SJG: Here we draw only the first object (start at 0, draw numTris1 triangles)
                render_id = wall_id;
            } else if (tile >= TT_DOOR_A && tile <= TT_DOOR_E) {
                if (map->KeyCollected(tile)) {
                    continue;
                }

                glUniform1i(uni_texid, -1);
                glUniform3fv(uni_color, 1, glm::value_ptr(key_colors[DoorIndex(tile)]));
                render_id = wall_id;
            } else if (tile >= TT_KEY_A && tile <= TT_KEY_E) {
                if (map->KeyCollected(tile)) {
                    continue;
                }

                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
                model = floating_obj(model, time);
                glUniform1i(uni_texid, -1);
                glUniform3fv(uni_color, 1, glm::value_ptr(key_colors[KeyIndex(tile)]));
                render_id = key_id;
            } else if (tile == TT_GOAL) {
                model = floating_obj(model, time);
                glm::vec3 goal_color(1.0f, 1.0f, 1.0f);
                glUniform3fv(uni_color, 1, glm::value_ptr(goal_color));
                glUniform1i(uni_texid, -1);
                render_id = goal_id;
            }

            glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, mp->Offset(render_id), mp->Length(render_id));
        }
    }
}

void Game::RenderCharacter()
{
    static const int uni_texid = glGetUniformLocation(texturedShader, "texID"),
        uni_model = glGetUniformLocation(texturedShader, "model"),
        uni_color = glGetUniformLocation(texturedShader, "inColor"),
        uni_alpha = glGetUniformLocation(texturedShader, "alpha");

    glBindBuffer(GL_ARRAY_BUFFER, player->model_id);

    glm::mat4 model;

    model = glm::translate(model, player->WorldPosition(time));
    model = glm::rotate(model, player->Rotation(), glm::vec3(0,0,1));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));

    glUniform1i(uni_texid, -1);
    glUniform1f(uni_alpha, 1.0f);
    glm::vec3 char_color(map->PctComplete());
    glUniform3fv(uni_color, 1, glm::value_ptr(char_color));

    glDrawArrays(GL_TRIANGLES, mp->Offset(player->model_id), mp->Length(player->model_id));
}

void Game::OnKeyDown(const SDL_KeyboardEvent& ev)
{
    switch (ev.keysym.sym) {
        default:
            if(!(player->MoveInDirection(ev.keysym.sym, map, time))) {
                nextAction = ev.keysym.sym;
                nextTime = time;
            }
    }
}

void Game::ChangeMap()
{
    delete map;
    //complexity = 10 * difficulty;
    difficulty++;
    printf("complexity = %d, difficulty = %d\n", 10 * difficulty, difficulty);
    map = Map::ParseMapFile(10 * difficulty);
    player = map->NewPlayerAtStart(char_id);
    nextAction = -1;
}

void Game::Update(){
    if(nextAction != -1 && (time - nextTime < ANIM_SPEED/2)) {
        if (player->MoveInDirection(nextAction, map, time)) nextAction = -1;
    }
}

static char* readShaderSource(const char* shaderFile)
{
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

void Win2PPM(int width, int height)
{
    char outdir[10] = "out/"; //Must be exist!
    int i,j;
    FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;

    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
        fprintf(stderr,"ERROR: Failed to allocate memory for image\n");
    }

    /* Open the file */
    sprintf(fname,"%simage_%04d.ppm",outdir,counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
        fprintf(stderr,"ERROR: Failed to open file for window capture\n");
    }

    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

    /* Write the PPM file */
    fprintf(fptr,"P6\n%d %d\n255\n",width,height);
    for (j=height-1;j>=0;j--) {
        for (i=0;i<width;i++) {
            fputc(image[3*j*width+3*i+0],fptr);
            fputc(image[3*j*width+3*i+1],fptr);
            fputc(image[3*j*width+3*i+2],fptr);
        }
    }

    free(image);
    fclose(fptr);
    counter++;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName)
{
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
    glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
    glCompileShader(vertex_shader); // Compile shaders

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

    //Check for Errors
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
