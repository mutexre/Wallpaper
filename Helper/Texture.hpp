//
//  Texture.hpp
//
//  Created by mutexre on 04/11/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#include <string>
#include <OpenGLES/ES3/gl.h>

#pragma once

using namespace std;

class Texture
{
public:
    enum class Type {
        t2d = GL_TEXTURE_2D,
    };

private:
    GLuint texture;
    Type type;
    unsigned w, h;

private:
    static GLuint createTexture2D(const void* imageData, GLint format, GLsizei w, GLsizei h);

public:
    Texture() : texture(0) {}
    Texture(const string& imageName);
    Texture(Type, const string& imageName, ...);
    Texture(const void* data, unsigned w, unsigned h);
    virtual ~Texture();

    unsigned getW() const;
    unsigned getH() const;

    void bind(unsigned short target = 0);
    void texImage(const void* data, GLint format, unsigned w, unsigned h);
};
