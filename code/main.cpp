#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/Camera.h"
#include "../include/Model.h"
#include "../include/Shader.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtx/string_cast.hpp"

#include "../include/imgui.h"
#include "../include/imgui_impl_sdl_gl3.h"

#include "../include/SDL.h"
#include "../include/glad.h"

#undef main

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

bool blinn = false;
float specularPower[] = { 4.0f, 8.0f, 16.0f, 32.0f, 64.0f };
int specIndex = 0;

const unsigned int NUM_CUBES = 10;
const float positionRange = 5.0f;
const float rotationRange = 45.0f;

bool init();
void close();
void processInput();
void printSDL_GL_Attributes();
void drawImgui();
void drawTestImgui();
void generateRandomPositions(unsigned int numCubes, glm::vec3 *cubePos);
void generateCubeRotations(unsigned int numCubes, glm::vec3 *cubeRot, float *rotAmount);
unsigned int loadTexture(char const *path);

SDL_Window *window = NULL;
SDL_GLContext context;

bool quit = false;
bool captureMouse = true;

bool printInfo = false;

int currEnv = 1;

float lastPressed = 0.0f;
float keyCooldown = 0.3f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float frameTime = 0.25f;

ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
bool show_another_window = false, show_demo_window = false;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("ERROR::INIT_SDL. SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    
    window = SDL_CreateWindow(
        "SDL_GL Main Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL);
    
    if (window == NULL) {
        printf("ERROR::INIT_WINDOW. SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Set SDL window icon
    SDL_Surface *surface;     // Declare an SDL_Surface to be filled in with pixel data from an image file
    Uint16 pixels[16 * 16] = {// ...or with raw pixel data:
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
        0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
        0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
        0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
        0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
        0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
        0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
        0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
        0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
        0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
        0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
        0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
        0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
        0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
        0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
        0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
        0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
        0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
        0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
        0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
        0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff};
    surface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 16, 16 * 2, 0x0f00, 0x00f0, 0x000f, 0xf000);
    
    // The icon is attached to the window pointer
    SDL_SetWindowIcon(window, surface);
    
    // ...and the surface containing the icon pixel data is no longer required.
    SDL_FreeSurface(surface);
    
    context = SDL_GL_CreateContext(window);
    
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    
    printSDL_GL_Attributes();
    /*
    SDL_CaptureMouse(SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    */
    
    srand((unsigned int)time(0));
    return true;
}

void close() {
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, numComponents;
    unsigned char *data = stbi_load(path, &width, &height, &numComponents, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (numComponents == 1)
            format = GL_RED;
        else if (numComponents == 3)
            format = GL_RGB;
        else if (numComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Unable to load image: " << path << std::endl;
    }
    stbi_image_free(data);
    
    return textureID;
}
void processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSdlGL3_ProcessEvent(&event);
        if (event.type == SDL_QUIT ||
            (event.type == SDL_KEYDOWN &&
             event.key.keysym.sym == SDLK_ESCAPE)) {
            quit = true;
        }
        if (event.type == SDL_MOUSEMOTION && captureMouse) {
            camera.ProcessMouseMovement((float)event.motion.xrel, (float)-event.motion.yrel);
        }
        if (event.type == SDL_MOUSEWHEEL && captureMouse) {
            //camera.ProcessMouseScroll((float)event.wheel.y);
        }
    }
    
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    
    lastPressed += deltaTime;
    
    if (keystate[SDL_SCANCODE_W]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (keystate[SDL_SCANCODE_S]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (keystate[SDL_SCANCODE_A]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (keystate[SDL_SCANCODE_D]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (keystate[SDL_SCANCODE_Q] || keystate[SDL_SCANCODE_SPACE]) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (keystate[SDL_SCANCODE_E] || keystate[SDL_SCANCODE_LSHIFT]) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (keystate[SDL_SCANCODE_F] && lastPressed >= keyCooldown) {
        captureMouse = !captureMouse;
        lastPressed = 0.0f;
    }
    if (keystate[SDL_SCANCODE_R] && lastPressed >= keyCooldown) {
        currEnv += 1;
        if (currEnv >= 4)
            currEnv = 0;
        lastPressed = 0.0f;
    }
    if (keystate[SDL_SCANCODE_B] && lastPressed >= keyCooldown) {
        blinn = !blinn;
        lastPressed = 0.0f;
    }
    if (keystate[SDL_SCANCODE_V] && lastPressed >= keyCooldown) {
        specIndex++;
        if (specIndex >= 5) specIndex = 0;
        lastPressed = 0.0f;
    }
    if (keystate[SDL_SCANCODE_C] && lastPressed >= keyCooldown) {
        specIndex--;
        if (specIndex < 0) specIndex = 4;
        lastPressed = 0.0f;
    }
    if (keystate[SDL_SCANCODE_X] && lastPressed >= keyCooldown) {
        printInfo = !printInfo;
        lastPressed = 0.0f;
    }
}

void printSDL_GL_Attributes() {
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));
    int attribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribs);
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        printf("Error found in GL.\n");
    }
    printf("Max vertex attributes: %d\n", attribs);
}

extern "C" int main(int argc, char **argv) {
    if (!init()) {
        printf("No go!\n");
    } else {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui_ImplSdlGL3_Init(window);
        ImGui::StyleColorsDark();
        
        //glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        Shader blinnShader("blinn.vs", "blinn.fs");
        
        unsigned int floorTexture = loadTexture("../res/wood.png");
        
        GLfloat planeVerts[] = {
            // positions            // normals         // texcoords
            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
            
            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
            10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
        };
        
        
        GLuint planeVAO, planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), &planeVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(float)));
        glBindVertexArray(0);
        
        blinnShader.use();
        blinnShader.setInt("floorTexture", 0);
        
        glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
        
        while (!quit) {
            if (captureMouse) {
                SDL_SetRelativeMouseMode(SDL_TRUE);
            } else {
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            uint32_t ticks = SDL_GetTicks();
            float timeValue = ticks / 1000.0f;
            
            deltaTime = timeValue - lastFrame;
            lastFrame = timeValue;
            
            processInput();
            
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            blinnShader.use();
            blinnShader.setMat4("projection", projection);
            blinnShader.setMat4("view", view);
            
            blinnShader.setVec3("viewPos", camera.Position);
            blinnShader.setVec3("lightPos", lightPos);
            blinnShader.setInt("blinn", blinn);
            blinnShader.setFloat("specPow", specularPower[specIndex]);
            
            glBindVertexArray(planeVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            if (printInfo) {
                printf(blinn ? "Blinn-Phong " : "Blinn ");
                printf("%f\n", specularPower[specIndex]);
            }
            SDL_GL_SwapWindow(window);
        }
        
        close();
    }
    return 0;
}