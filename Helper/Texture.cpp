//
//  Texture.cpp
//
//  Created by mutexre on 04/11/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#include <stdlib.h>
#include "Texture.hpp"
#include "Helper.hpp"

Texture::Texture(Type type, const string& imageName, ...) {
    switch (type) {
        case Type::t2d: {
            GLint format;
            unique_ptr<unsigned char> imageData = loadImage(imageName.c_str(), format, w, h);
            texture = createTexture2D(imageData.get(), format, w, h);
            this->type = Type::t2d;
        }
        break;

        default:
            throw runtime_error("unsupported texture type");
    }
}

Texture::Texture(const string& imageName) : Texture(Type::t2d, imageName) {}

Texture::Texture(const void* data, unsigned w, unsigned h) {
    texture = createTexture2D(data, GL_RGBA, w, h);
    type = Type::t2d;
}

Texture::~Texture() {
    if (texture) glDeleteTextures(1, &texture);
}

void Texture::bind(unsigned short target) {
    glActiveTexture(GL_TEXTURE0 + target);
    glBindTexture(GLenum(type), texture);
}

GLuint Texture::createTexture2D(const void* imageData, GLint format, GLsizei w, GLsizei h) {
    GLuint texture;
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    if (imageData)
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, imageData);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return texture;
}

void Texture::texImage(const void* imageData, GLint format, unsigned w, unsigned h)
{
    glTexImage2D(GLenum(type), 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, imageData);
}
