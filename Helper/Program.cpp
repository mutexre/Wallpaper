//
//  Program.cpp
//  LookseryTestTask
//
//  Created by mutexre on 26/10/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#include <glm/gtc/type_ptr.hpp>
#include "Program.hpp"

Program::Program(const Source& src) {
    load(src);
}

Program::~Program() {
    if (program) glDeleteProgram(program);
}

GLuint Program::compile(GLenum type, const string& source) {
    GLint isCompiled;
    const GLchar* sources[] = { source.c_str() };

    GLuint ID = glCreateShader(type);
    if (!ID) throw runtime_error("failed to create shader");

    glShaderSource(ID, 1, sources, 0);
    glCompileShader(ID);
    glGetShaderiv(ID, GL_COMPILE_STATUS, &isCompiled);

    if (!isCompiled) {
        GLsizei logLength;

        glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLsizei actualCompilationLogLength;

            auto log = new GLchar[logLength + 1];
            memset(log, 0, logLength + 1);

            glGetShaderInfoLog(ID, logLength, &actualCompilationLogLength, log);

            throw runtime_error(log);
        }

        throw runtime_error("failed to compile shader");
    }

    return ID;
}

void Program::link(GLuint id) {
    GLint isLinked;

    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        GLsizei logLength;

        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLsizei actualCompilationLogLength;

            auto log = new GLchar[logLength + 1];
            memset(log, 0, logLength + 1);

            glGetProgramInfoLog(id, logLength, &actualCompilationLogLength, log);

            throw runtime_error(log);
        }

        throw runtime_error("link program failed");
    }
}

void Program::queryActiveAttributes() {
    attribs.clear();

    int total = -1;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &total);
    for (int i = 0; i < total; i++) {
        int nameLen = -1;
        int size = -1;
        GLenum type = GL_ZERO;
        char name[100];

        glGetActiveAttrib(program, GLuint(i), sizeof(name) - 1, &nameLen, &size, &type, name);
        name[nameLen] = 0;

        GLuint location = glGetAttribLocation(program, name);

        string attribName(name);
        attribs[attribName] = Attrib{ attribName, location, type, size };
    }
}

void Program::queryActiveUniforms() {
    uniforms.clear();

    int total = -1;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);
    for (int i = 0; i < total; i++) {
        int nameLen = -1;
        int size = -1;
        GLenum type = GL_ZERO;
        char name[100];

        glGetActiveUniform(program, GLuint(i), sizeof(name) - 1, &nameLen, &size, &type, name);
        name[nameLen] = 0;

        GLuint location = glGetUniformLocation(program, name);

        string uniformName(name);
        uniforms[uniformName] = Uniform{ uniformName, location, type, size };
    }
}

const char* convertGlslShaderType(GLuint type) {
    switch (type) {
        case GL_VERTEX_SHADER: return "vertex";
        case GL_FRAGMENT_SHADER: return "fragment";
        default: return "";
    }
}

void Program::load(const Source& src) {
    struct Shaders {
        map<GLuint, GLuint> ids;

        ~Shaders() {
            for (auto id : ids)
                if (id.second) glDeleteShader(id.second);
        }

        void compile(GLuint type, const string& src) {
            try {
                ids[type] = Program::compile(type, src);
            }
            catch (const runtime_error& err) {
                throw runtime_error(string(convertGlslShaderType(type))
                                         + " shader compilation failed: "
                                         + err.what());
            }
        }

        void attach(GLuint program) {
            for (auto id : ids)
                if (id.second) glAttachShader(program, id.second);
        }
    }
    shaders;

    shaders.compile(GL_VERTEX_SHADER, src.vertex);
    shaders.compile(GL_FRAGMENT_SHADER, src.fragment);

	auto id = glCreateProgram();
	if (!id) throw runtime_error("failed to create shader program");

    shaders.attach(id);

    try {
        link(id);
    }
    catch (const runtime_error& err) {
        throw runtime_error(string("failed to link program: ") + err.what());
    }

    program = id;

    queryActiveAttributes();
    queryActiveUniforms();
}

void Program::bind() {
    glUseProgram(program);
}

bool Program::isAttributeActive(const string& name) {
    return attribs.find(name) != attribs.end();
}

bool Program::isUniformActive(const string& name) {
    return uniforms.find(name) != uniforms.end();
}

GLuint Program::getAttributeLocation(const string& name) {
    return attribs[name].location;
}

GLuint Program::getUniformLocation(const string& name) {
    return uniforms[name].location;
}

void Program::setVertexAttr(const string& name, float value) {
    glVertexAttrib1f(getAttributeLocation(name), value);
}

void Program::setVertexAttr(const string& name, float x, float y, float z, float w) {
    glVertexAttrib4f(getAttributeLocation(name), x, y, z, w);
}

void Program::setVertexAttr(const string& name, const vec4& xyzw) {
    glVertexAttrib4fv(getAttributeLocation(name), (const GLfloat*)&xyzw);
}

void Program::setUniform(const string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Program::setUniform(const string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Program::setUniform(const string& name, float x, float y, float z, float w) {
    glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Program::setUniform(const string& name, const vec4& xyzw) {
    glUniform4fv(getUniformLocation(name), 1, (const GLfloat*)&xyzw);
}

void Program::setUniform(const string& name, const mat3& m, bool transpose) {
    glUniformMatrix3fv(getUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, (const GLfloat*)value_ptr(m));
}
