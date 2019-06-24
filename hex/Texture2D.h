#ifndef _TEXTURE_2D_H
#define _TEXTURE_2D_H

#include "../include/glad.h"

class Texture2D {
    public:
    GLuint ID;
    GLuint Width, Height;
    GLuint InternalFormat;
    GLuint ImageFormat;
    GLuint WrapS, WrapT;
    GLuint FilterMin, FilterMag;
    Texture2D();
    void Generate(GLuint width, GLuint height, unsigned char* data);
    void Bind() const;
};

#endif