#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "../include/glad.h"
#include "../include/SDL.h"
#undef main

#include "util.h"

#define check_gl_error() _check_gl_error(__FILE__, __LINE__)

void _check_gl_error(const char *file, int line) {
    GLenum err(glGetError());
    
    while (err != GL_NO_ERROR) {
        std::string error;
        
        switch(err) {
            case GL_INVALID_OPERATION: error="INVALID_OPERATION"; break;
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FB_OPERATION"; break;
            default: error="UNKNOWN_ERROR"; break;
        }
        std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
        err = glGetError();
    }
}
/*
  * Global data used by our render callback:
 */
static struct {
    GLuint vertex_buffer, element_buffer;
    GLuint textures[2];
    GLuint vertex_shader, fragment_shader, program;
    
    struct {
        GLint fade_factor;
        GLint textures[2];
    } uniforms;
    
    struct {
        GLint position;
    } attributes;
    
    GLfloat fade_factor;
} g_resources;

/*
* Functions for creating OpenGL objects:
*/
static GLuint make_buffer (
GLenum target, 
const void *buffer_data, 
GLsizei buffer_size) 
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
    return buffer;
}

static GLuint make_texture(const char* filename) {
    int width, height;
    void *pixels = read_tga(filename, &width, &height);
    GLuint texture;
    
    if (!pixels) return 0;
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level */
        GL_RGB8,                    /* internal format */
        width, height, 0,           /* width, height, border */
        GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
        pixels                      /* pixels */
        );
    free(pixels);
    return texture;
}

static void show_info_log(
GLuint object,
PFNGLGETSHADERIVPROC glGet__iv,
PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
)
{
    GLint log_length;
    char *log;
    
    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

static GLuint make_shader(GLenum type, const char *filename) {
    GLint length;
    GLchar *source = file_contents(filename, &length);
    GLuint shader;
    GLint shader_ok;
    
    if (!source) return 0;
    
    shader= glCreateShader(type);
    glShaderSource(shader,  1, (const GLchar**)&source, &length);
    free(source);
    glCompileShader(shader);
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
        show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint make_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLint program_ok;
    GLuint program = glCreateProgram();
    
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

/*
* Data used to seed our vertex array and element array buffers.
*/
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f
};

static const GLushort g_element_buffer_data[] = { 0, 1, 2, 3 };

/*
* Load and create all our resources.
*/
static int make_resources(void) {
    g_resources.vertex_buffer = make_buffer(
        GL_ARRAY_BUFFER,
        g_vertex_buffer_data,
        sizeof(g_vertex_buffer_data)
        );
    g_resources.element_buffer = make_buffer(
        GL_ELEMENT_ARRAY_BUFFER,
        g_element_buffer_data,
        sizeof(g_element_buffer_data)
        );
    
    g_resources.textures[0] = make_texture("../res/hello1.tga");
    g_resources.textures[1] = make_texture("../res/hello2.tga");
    
    if (g_resources.textures[0] == 0 || g_resources.textures[1] == 0) return 0;
    
    g_resources.vertex_shader = make_shader(
        GL_VERTEX_SHADER,
        "../code/hello.vs"
        );
    if (g_resources.vertex_shader == 0) return 0;
    
    g_resources.fragment_shader = make_shader(
        GL_FRAGMENT_SHADER,
        "../code/hello.fs"
        );
    if (g_resources.fragment_shader == 0) return 0;
    
    g_resources.program = make_program(g_resources.vertex_shader, g_resources.fragment_shader);
    if (g_resources.program == 0) return 0;
    
    g_resources.uniforms.fade_factor = glGetUniformLocation(g_resources.program, "fade_factor");
    g_resources.uniforms.textures[0] = glGetUniformLocation(g_resources.program, "textures[0]");
    g_resources.uniforms.textures[1] = glGetUniformLocation(g_resources.program, "textures[1]");
    
    g_resources.attributes.position = glGetAttribLocation(g_resources.program, "position");
    
    return 1;
}

static void update_fade_factor(void) {
    float milliseconds = (float)SDL_GetTicks();
    g_resources.fade_factor = sinf(milliseconds * 0.001f) * 0.5f + 0.5f;
}

static void render(void) {
    glUseProgram(g_resources.program);
    
    check_gl_error();
    
    glUniform1f(g_resources.uniforms.fade_factor, g_resources.fade_factor);
    
    check_gl_error();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_resources.textures[0]);
    glUniform1i(g_resources.uniforms.textures[0], 0);
    
    check_gl_error();
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_resources.textures[1]);
    glUniform1i(g_resources.uniforms.textures[1], 1);
    
    check_gl_error();
    
    glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
    glVertexAttribPointer(
        g_resources.attributes.position, /* attribute */
        2,                               /* size */
        GL_FLOAT,                        /* type */
        GL_FALSE,                        /* normalized? */
        sizeof(GLfloat)*2,               /* stride */
        (void*)0                         /* array buffer offset */
        );
    glEnableVertexAttribArray(g_resources.attributes.position);
    
    check_gl_error();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
    
    check_gl_error();
    
    glDrawElements(
        GL_TRIANGLE_STRIP,  /* mode */
        4,                  /* count */
        GL_UNSIGNED_SHORT,  /* type */
        (void*)0            /* element array buffer offset */
        );
    
    glDisableVertexAttribArray(g_resources.attributes.position);
}

static void printSDL_GL_Attributes() {
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

void processInput(int *quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || 
            (event.type == SDL_KEYDOWN &&
             event.key.keysym.sym == SDLK_ESCAPE)) {
            *quit = 1;
        }
    }
}

int main(int argc, char **argv) {
    SDL_Window *window;
    SDL_GLContext context;
    
    int quit = 0;
    
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "Failed to init SDL. Error: %s\n", SDL_GetError());
        return 0;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    window = SDL_CreateWindow(
        "Modern OpenGL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        400,
        300,
        SDL_WINDOW_OPENGL
        );
    
    if (window == NULL) {
        fprintf(stderr, "Could not init window. Error: %s\n", SDL_GetError());
        return 1;
    }
    
    // Set SDL window icon
    SDL_Surface *surface;     // Declare an SDL_Surface to be filled in with pixel data from an image file
    Uint16 pixels[16*16] = {  // ...or with raw pixel data:
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
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
    };
    surface = SDL_CreateRGBSurfaceFrom(pixels,16,16,16,16*2,0x0f00,0x00f0,0x000f,0xf000);
    
    // The icon is attached to the window pointer
    SDL_SetWindowIcon(window, surface);
    
    // ...and the surface containing the icon pixel data is no longer required.
    SDL_FreeSurface(surface);
    
    context = SDL_GL_CreateContext(window);
    
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    
    printSDL_GL_Attributes();
    
    if (!make_resources()) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }
    
    /*
    static struct {
        GLuint vertex_buffer, element_buffer;
        GLuint textures[2];
        GLuint vertex_shader, fragment_shader, program;
        
        struct {
            GLint fade_factor;
            GLint textures[2];
        } uniforms;
        
        struct {
            GLint position;
        } attributes;
        
        GLfloat fade_factor;
    } g_resources;
    */
    
    printf("Texture0: %d\n", g_resources.textures[0]);
    printf("Texture1: %d\n", g_resources.textures[1]);
    printf("Fade factor: %d\n", g_resources.uniforms.fade_factor);
    
    while(!quit) {
        processInput(&quit);
        update_fade_factor();
        render();
        SDL_GL_SwapWindow(window);
    }
    return 0;
}