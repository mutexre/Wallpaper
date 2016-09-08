//
//  Program.hpp
//  LookseryTestTask
//
//  Created by mutexre on 26/10/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#include <string>
#include <map>
#include <OpenGLES/ES3/gl.h>
#include <glm/glm.hpp>

#pragma once

using namespace std;
using namespace glm;

class Program
{
public:
    struct Source {
        string vertex, geometry, fragment;
    };

private:
    struct Uniform {
        string name;
        GLuint location;
        GLenum type;
        GLint size;
    };

    struct Attrib {
        string name;
        GLuint location;
        GLenum type;
        GLint size;
    };

private:
    GLuint program;
    map<string, Uniform> uniforms;
    map<string, Attrib> attribs;

private:
    static GLuint compile(GLenum type, const string& source);
    static void link(GLuint id);

    void queryActiveAttributes();
    void queryActiveUniforms();

public:
    Program() : program(0) {}
    Program(const Source&);
    virtual ~Program();

    void load(const Source& src);

    void bind();

    bool isAttributeActive(const string&);
    bool isUniformActive(const string&);

    GLuint getAttributeLocation(const string& name);
    GLuint getUniformLocation(const string& name);

    void setVertexAttr(const string&, float);
    void setVertexAttr(const string&, float, float, float, float);
    void setVertexAttr(const string&, const vec4&);

    void setUniform(const string&, int);
    void setUniform(const string&, float);
    void setUniform(const string&, float, float, float, float);
    void setUniform(const string&, const vec4&);
    void setUniform(const string&, const mat3&, bool transpose = false);
};
