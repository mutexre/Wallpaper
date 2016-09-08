//
//  helper.h
//  LookseryTestTask
//
//  Created by mutexre on 08/10/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#include <string>
#include <glm/glm.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include "Option.hpp"
#include "Program.hpp"
#include "Texture.hpp"

#pragma once

using namespace std;
using namespace glm;

/*#define GF_PROPERTY_GETTER(name, type, var) type name() const { return var; }
#define GF_PROPERTY_SETTER(class, name, type, var) class& name(type x) { this->var = x; return *this; }
#define GF_PROPERTY(class, getter, setter, type, var) \
    GF_PROPERTY_GETTER(getter, type, var); \
    GF_PROPERTY_SETTER(class, setter, type, var);

#define GF_PROPERTY_GETTER_CONST_REF(name, type, var) const type& name() const { return var; return *this; }
#define GF_PROPERTY_SETTER_CONST_REF(class, name, type, var) class& name(const type& x) { this->var = x; return *this; }
#define GF_PROPERTY_CONST_REF(class, getter, setter, type, var) \
    GF_PROPERTY_GETTER_CONST_REF(getter, type, var); \
    GF_PROPERTY_SETTER_CONST_REF(class, setter, type, var);*/

string readFile(const string& path);

Option<string> findResourcePathByName(CFBundleRef bundle, const char* name, const char* ext);
Option<string> findResourcePathByName(const char* name, const char* ext);
Option<string> findResourcePathByName(const string& name, const string& ext);

Option<Program::Source> readProgramSource(const char* vertex, const char* fragment);
shared_ptr<Program> loadProgram(const char* vertexShaderName, const char* fragmentShaderName);

unique_ptr<unsigned char>
loadImage(const char* imageName, GLint& format, unsigned& w, unsigned& h);

unique_ptr<unsigned char>
createTextBitmap(const char* text,
                 const char* fontName,
                 float fontSize,
                 vec4 backgroundColor,
                 vec4 fontColor,
                 unsigned w, unsigned h,
                 float x, float y);

shared_ptr<Texture>
createTextureFromText(const char* text,
                      const char* fontName,
                      float fontSize,
                      vec4 backgroundColor,
                      vec4 fontColor,
                      unsigned w, unsigned h,
                      float x, float y);

shared_ptr<Texture> createTexture(const string&, unsigned w, unsigned h);

void checkGlErrors();
void printGlBindings();
