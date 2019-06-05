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

int currEnv = 0;

float lastPressed = 0.0f;
float keyCooldown = 0.3f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
bool show_another_window = false, show_demo_window = false;

glm::mat4 projection = glm::mat4();
glm::mat4 view = glm::mat4();
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
    };
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

void drawImgui() {
    ImGui_ImplSdlGL3_NewFrame(window);

    ImGui::ColorEdit3("clear color", (float *)&clear_color);

    ImGui::Checkbox("Matrices Window", &show_another_window);
    ImGui::Checkbox("Demo Window", &show_demo_window);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (show_another_window) {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::Begin("Matrices Window", &show_another_window);
        ImGui::Text("Projection Matrix");
        for (unsigned int i = 0; i < 4; i++) {
            ImGui::SliderFloat4("Row", &projection[0][i], -100.0f, 100.0f);
        }
        ImGui::Text("View Matrix");
        for (unsigned int i = 0; i < 4; i++) {
            ImGui::SliderFloat4("Row", &view[0][i], -100.0f, 100.0f);
        }
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    if (show_demo_window) {

        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    ImGui::Render();
    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
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

        Shader lightingShader("colors.vs", "colors.fs");
        Shader lampShader("lamp.vs", "lamp.fs");
        Shader modelShader("model.vs", "model.fs");

        Model suit("../res/nanosuit/nanosuit.obj");
        Model cyborg("../res/cyborg/cyborg.obj");

        unsigned int diffuseMap = loadTexture("../res/container2.png");
        unsigned int specularMap = loadTexture("../res/container2_specular.png");
        //unsigned int emissionMap = loadTexture("../res/matrix.jpg");

        GLfloat cubeVerts[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

        glm::vec3 cubePositions[NUM_CUBES];
        generateRandomPositions(NUM_CUBES, cubePositions);
        glm::vec3 cubeRotations[NUM_CUBES];
        float rotationAmounts[NUM_CUBES];
        generateCubeRotations(NUM_CUBES, cubeRotations, rotationAmounts);

        glm::vec4 clearColors[] = {
            glm::vec4(0.75f, 0.52f, 0.3f, 1.0f),
            glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(0.85f, 0.85f, 0.85f, 1.0f)};

        glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f)};

        glm::vec3 pointLightColors[] = {
            glm::vec3(1.0f, 0.6f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.2f, 0.2f, 1.0f),
            glm::vec3(0.2f, 0.2f, 0.6f),
            glm::vec3(0.3f, 0.3f, 0.7f),
            glm::vec3(0.0f, 0.0f, 0.3f),
            glm::vec3(0.4f, 0.4f, 0.4f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.3f, 0.1f, 0.1f),
            glm::vec3(0.4f, 0.7f, 0.1f),
            glm::vec3(0.4f, 0.7f, 0.1f),
            glm::vec3(0.4f, 0.7f, 0.1f),
            glm::vec3(0.4f, 0.7f, 0.1f)};

        glm::vec3 dirLightDirections[] = {
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(-0.2f, -1.0f, -0.3f)};

        glm::vec3 dirLightAmbients[] = {
            glm::vec3(0.3f, 0.24f, 0.14f),
            glm::vec3(0.05f, 0.05f, 0.1f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.5f, 0.5f, 0.5f)};

        glm::vec3 dirLightDiffuses[] = {
            glm::vec3(0.7f, 0.42f, 0.26f),
            glm::vec3(0.2f, 0.2f, 0.7f),
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(1.0f, 1.0f, 1.0f)};

        glm::vec3 dirLightSpeculars[] = {
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(0.7f, 0.7f, 0.7f),
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(1.0f, 1.0f, 1.0f)};

        glm::vec3 spotLightAmbients[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)};

        glm::vec3 spotLightDiffuses[] = {
            glm::vec3(0.8f, 0.8f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)};

        glm::vec3 spotLightSpeculars[] = {
            glm::vec3(0.8f, 0.8f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)};

        float spotLightCutOffs[] = {
            12.5f, 10.0f, 10.0f, 7.0f};

        float spotLightOuterCutOffs[] = {
            13.0f, 12.5f, 15.0f, 10.0f};

        float pointLightConstant = 1.0f;

        float pointLightLinears[] = {
            0.09f, 0.09f, 0.09f, 0.09f,
            0.09f, 0.09f, 0.09f, 0.09f,
            0.14f, 0.14f, 0.22f, 0.14f,
            0.07f, 0.07f, 0.07f, 0.07f};

        float pointLightQuadratics[] = {
            0.032f, 0.032f, 0.032f, 0.032f,
            0.032f, 0.032f, 0.032f, 0.032f,
            0.07f, 0.07f, 0.2f, 0.07f,
            0.017f, 0.017f, 0.017f, 0.017f};

        float spotLightConstant = 1.0f;

        float spotLightLinears[] = {
            0.09f, 0.009f, 0.09f, 0.07f};

        float spotLightQuadratics[] = {
            0.032f, 0.0032f, 0.032f, 0.017f};

        GLuint cubeVAO, cubeVBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), &cubeVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(float)));
        glBindVertexArray(0);

        unsigned int lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glBindVertexArray(lightVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        lightingShader.use();
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);
        //lightingShader.setInt("material.emission", 2);
        lightingShader.setFloat("material.shininess", 64.0f);

        modelShader.use();
        modelShader.setFloat("shininess", 64.0f);

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

            glClearColor(clearColors[currEnv].x, clearColors[currEnv].y, clearColors[currEnv].z, clearColors[currEnv].w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            lightingShader.use();
            lightingShader.setVec3("dirLight.direction", dirLightDirections[currEnv]);
            lightingShader.setVec3("viewPos", camera.Position);
            projection = glm::perspective(camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
            view = camera.GetViewMatrix();
            lightingShader.setMat4("projection", projection);
            lightingShader.setMat4("view", view);

            lightingShader.setVec3("dirLight.ambient", dirLightAmbients[currEnv]);
            lightingShader.setVec3("dirLight.diffuse", dirLightDiffuses[currEnv]);
            lightingShader.setVec3("dirLight.specular", dirLightSpeculars[currEnv]);

            int point = currEnv * 4;

            lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
            lightingShader.setVec3("pointLights[0].ambient", pointLightColors[point + 0] * 0.1f);
            lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[point + 0]);
            lightingShader.setVec3("pointLights[0].specular", pointLightColors[point + 0]);
            lightingShader.setFloat("pointLights[0].constant", pointLightConstant);
            lightingShader.setFloat("pointLights[0].linear", pointLightLinears[point + 0]);
            lightingShader.setFloat("pointLights[0].quadratic", pointLightQuadratics[point + 0]);

            lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
            lightingShader.setVec3("pointLights[1].ambient", pointLightColors[point + 1] * 0.1f);
            lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[point + 1]);
            lightingShader.setVec3("pointLights[1].specular", pointLightColors[point + 1]);
            lightingShader.setFloat("pointLights[1].constant", pointLightConstant);
            lightingShader.setFloat("pointLights[1].linear", pointLightLinears[point + 1]);
            lightingShader.setFloat("pointLights[1].quadratic", pointLightQuadratics[point + 1]);

            lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
            lightingShader.setVec3("pointLights[2].ambient", pointLightColors[point + 2] * 0.1f);
            lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[point + 2]);
            lightingShader.setVec3("pointLights[2].specular", pointLightColors[point + 2]);
            lightingShader.setFloat("pointLights[2].constant", pointLightConstant);
            lightingShader.setFloat("pointLights[2].linear", pointLightLinears[point + 2]);
            lightingShader.setFloat("pointLights[2].quadratic", pointLightQuadratics[point + 2]);

            lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
            lightingShader.setVec3("pointLights[3].ambient", pointLightColors[point + 3] * 0.1f);
            lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[point + 3]);
            lightingShader.setVec3("pointLights[3].specular", pointLightColors[point + 3]);
            lightingShader.setFloat("pointLights[3].constant", pointLightConstant);
            lightingShader.setFloat("pointLights[3].linear", pointLightLinears[point + 3]);
            lightingShader.setFloat("pointLights[3].quadratic", pointLightQuadratics[point + 3]);

            lightingShader.setVec3("spotLight.position", camera.Position);
            lightingShader.setVec3("spotLight.direction", camera.Front);
            lightingShader.setVec3("spotLight.ambient", spotLightAmbients[currEnv]);
            lightingShader.setVec3("spotLight.diffuse", spotLightDiffuses[currEnv]);
            lightingShader.setVec3("spotLight.specular", spotLightSpeculars[currEnv]);
            lightingShader.setFloat("spotLight.constant", spotLightConstant);
            lightingShader.setFloat("spotLight.linear", spotLightLinears[currEnv]);
            lightingShader.setFloat("spotLight.quadratic", spotLightQuadratics[currEnv]);
            lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLightCutOffs[currEnv])));
            lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLightOuterCutOffs[currEnv])));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);
            //glActiveTexture(GL_TEXTURE2);
            //glBindTexture(GL_TEXTURE_2D, emissionMap);

            glBindVertexArray(cubeVAO);
            glm::mat4 model = glm::mat4(1.0f);

            for (unsigned int i = 0; i < NUM_CUBES; i++) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                model = glm::rotate(model, glm::radians(rotationAmounts[i]), cubeRotations[i]);
                lightingShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            lampShader.use();
            lampShader.setMat4("projection", projection);
            lampShader.setMat4("view", view);

            glBindVertexArray(lightVAO);

            for (unsigned int i = 0; i < 4; i++) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.25f));
                lampShader.setMat4("model", model);
                lampShader.setVec3("lampColor", pointLightColors[point + i]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            glBindVertexArray(0);

            modelShader.use();
            modelShader.setMat4("projection", projection);
            modelShader.setMat4("view", view);

            modelShader.setVec3("dirLight.ambient", dirLightAmbients[currEnv]);
            modelShader.setVec3("dirLight.diffuse", dirLightDiffuses[currEnv]);
            modelShader.setVec3("dirLight.specular", dirLightSpeculars[currEnv]);

            modelShader.setVec3("pointLights[0].position", pointLightPositions[0]);
            modelShader.setVec3("pointLights[0].ambient", pointLightColors[point + 0] * 0.1f);
            modelShader.setVec3("pointLights[0].diffuse", pointLightColors[point + 0]);
            modelShader.setVec3("pointLights[0].specular", pointLightColors[point + 0]);
            modelShader.setFloat("pointLights[0].constant", pointLightConstant);
            modelShader.setFloat("pointLights[0].linear", pointLightLinears[point + 0]);
            modelShader.setFloat("pointLights[0].quadratic", pointLightQuadratics[point + 0]);

            modelShader.setVec3("pointLights[1].position", pointLightPositions[1]);
            modelShader.setVec3("pointLights[1].ambient", pointLightColors[point + 1] * 0.1f);
            modelShader.setVec3("pointLights[1].diffuse", pointLightColors[point + 1]);
            modelShader.setVec3("pointLights[1].specular", pointLightColors[point + 1]);
            modelShader.setFloat("pointLights[1].constant", pointLightConstant);
            modelShader.setFloat("pointLights[1].linear", pointLightLinears[point + 1]);
            modelShader.setFloat("pointLights[1].quadratic", pointLightQuadratics[point + 1]);

            modelShader.setVec3("pointLights[2].position", pointLightPositions[2]);
            modelShader.setVec3("pointLights[2].ambient", pointLightColors[point + 2] * 0.1f);
            modelShader.setVec3("pointLights[2].diffuse", pointLightColors[point + 2]);
            modelShader.setVec3("pointLights[2].specular", pointLightColors[point + 2]);
            modelShader.setFloat("pointLights[2].constant", pointLightConstant);
            modelShader.setFloat("pointLights[2].linear", pointLightLinears[point + 2]);
            modelShader.setFloat("pointLights[2].quadratic", pointLightQuadratics[point + 2]);

            modelShader.setVec3("pointLights[3].position", pointLightPositions[3]);
            modelShader.setVec3("pointLights[3].ambient", pointLightColors[point + 3] * 0.1f);
            modelShader.setVec3("pointLights[3].diffuse", pointLightColors[point + 3]);
            modelShader.setVec3("pointLights[3].specular", pointLightColors[point + 3]);
            modelShader.setFloat("pointLights[3].constant", pointLightConstant);
            modelShader.setFloat("pointLights[3].linear", pointLightLinears[point + 3]);
            modelShader.setFloat("pointLights[3].quadratic", pointLightQuadratics[point + 3]);

            modelShader.setVec3("spotLight.position", camera.Position);
            modelShader.setVec3("spotLight.direction", camera.Front);
            modelShader.setVec3("spotLight.ambient", spotLightAmbients[currEnv]);
            modelShader.setVec3("spotLight.diffuse", spotLightDiffuses[currEnv]);
            modelShader.setVec3("spotLight.specular", spotLightSpeculars[currEnv]);
            modelShader.setFloat("spotLight.constant", spotLightConstant);
            modelShader.setFloat("spotLight.linear", spotLightLinears[currEnv]);
            modelShader.setFloat("spotLight.quadratic", spotLightQuadratics[currEnv]);
            modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLightCutOffs[currEnv])));
            modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLightOuterCutOffs[currEnv])));

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(10.0f, -5.0f, -10.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            modelShader.setMat4("model", model);
            suit.Draw(modelShader);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-10.0f, -5.0f, 10.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            modelShader.setMat4("model", model);
            cyborg.Draw(modelShader);

            //drawImgui();
            SDL_GL_SwapWindow(window);
        }

        close();
    }
    return 0;
}

float getRandomNumber() {
    // generate float between 0.0 and 1.0
    float r = (float)rand() / (float)RAND_MAX;
    // adjust to -1.0 ... 1.0 space
    r = (r * 2.0f) - 1.0f;
    // Adjust to position range
    r *= positionRange;

    return r;
}

float getRandForRotationAxis() {
    return (float)rand() / (float)RAND_MAX;
}

float getRandForRotationAmount() {
    float r = (float)rand() / (float)RAND_MAX;
    r *= rotationRange;
    return r;
}

void generateRandomPositions(unsigned int numCubes, glm::vec3 *cubePos) {
    for (unsigned int i = 0; i < numCubes; i++) {
        cubePos[i] = glm::vec3(getRandomNumber(), getRandomNumber(), getRandomNumber());
    }
}

void generateCubeRotations(unsigned int numCubes, glm::vec3 *cubeRot, float *rotAmt) {
    for (unsigned int i = 0; i < numCubes; i++) {
        cubeRot[i] = glm::vec3(getRandForRotationAxis(), getRandForRotationAxis(), getRandForRotationAxis());
        rotAmt[i] = getRandForRotationAmount();
    }
}