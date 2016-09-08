//
//  helper.cpp
//  LookseryTestTask
//
//  Created by mutexre on 09/10/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <UIKit/UIKit.h>
#include <CoreText/CoreText.h>
#include "Helper.hpp"

string readFile(const string& path) {
    ifstream ifs(path);
    stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

auto cfDeleter = [](CFTypeRef ref) {
    if (ref) CFRelease(ref);
};

Option<string> findResourcePathByName(CFBundleRef bundle, const char* name, const char* ext) {
    auto _name = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingUTF8);
    unique_ptr<const __CFString, decltype(cfDeleter)> nameCFStr{ _name, cfDeleter };

    auto _ext = CFStringCreateWithCString(kCFAllocatorDefault, ext, kCFStringEncodingUTF8);
    unique_ptr<const __CFString, decltype(cfDeleter)> extCFStr{ _ext, cfDeleter };

    auto _url = CFBundleCopyResourceURL(bundle, nameCFStr.get(), extCFStr.get(), nullptr);
    unique_ptr<const __CFURL, decltype(cfDeleter)> url{ _url, cfDeleter };
    if (url) {
        auto _path = CFURLCopyFileSystemPath(url.get(), kCFURLPOSIXPathStyle);
        unique_ptr<const __CFString, decltype(cfDeleter)> path{ _path, cfDeleter };

        size_t bufferSize = 256;
        Boolean result;
        do {
            unique_ptr<char[]> buffer(new char[bufferSize]);
            result = CFStringGetCString(path.get(), buffer.get(), bufferSize, kCFStringEncodingUTF8);
            if (result)
                return Option<string>(buffer.get());
            else
                bufferSize <<= 1;
        }
        while (bufferSize < 2 * 1024);
    }

    return Option<string>();
}

Option<string> findResourcePathByName(const char* name, const char* ext) {
    return findResourcePathByName(CFBundleGetMainBundle(), name, ext);
}

Option<string> findResourcePathByName(const string& name, const string& ext) {
    return findResourcePathByName(name.c_str(), ext.c_str());
}

Option<Program::Source> readProgramSource(const char* vertex, const char* fragment) {
    Program::Source src;

    auto bundle = CFBundleGetMainBundle();
    if (bundle) {
        auto vertexShaderPath = findResourcePathByName(bundle, vertex, "vsh");
        if (!vertexShaderPath) {
            printf("vertex shader not found: %s\n", vertex);
            return Option<Program::Source>();
        }

        auto fragmentShaderPath = findResourcePathByName(bundle, fragment, "fsh");
        if (!fragmentShaderPath) {
            printf("fragment shader not found: %s\n", fragment);
            return Option<Program::Source>();
        }

        src.vertex = readFile(vertexShaderPath.value);
        src.fragment = readFile(fragmentShaderPath.value);
    }

    return Option<Program::Source>(src);
}

shared_ptr<Program>
loadProgram(const char* vertexShaderName,
            const char* fragmentShaderName)
{
    auto programSrc = readProgramSource(vertexShaderName, fragmentShaderName);
    if (!programSrc) return 0;

    return make_shared<Program>(programSrc.value);
}

unique_ptr<unsigned char>
loadImage(const char* imageName, GLint& format, unsigned& w, unsigned& h)
{
    UIImage* img = [UIImage imageNamed:[NSString stringWithUTF8String:imageName]];
    CGImageRef image = img.CGImage;
    if (!image) throw runtime_error(string("Failed to load image: ") + imageName);

    w = static_cast<unsigned>(CGImageGetWidth(image));
    h = static_cast<unsigned>(CGImageGetHeight(image));
    auto data = unique_ptr<unsigned char>(new unsigned char[(w * h) << 2]);

    CGContextRef context = CGBitmapContextCreate(data.get(), w, h, 8, w << 2,
                                                 CGImageGetColorSpace(image),
                                                 kCGImageAlphaPremultipliedLast);

    CGContextDrawImage(context, CGRectMake(0, 0, w, h), image);
    CGContextRelease(context);

    format = GL_RGBA;

    return data;
}

unique_ptr<unsigned char>
createTextBitmap(const char* text,
                 const char* fontName,
                 float fontSize,
                 vec4 backgroundColor,
                 vec4 fontColor,
                 unsigned w, unsigned h,
                 float x, float y)
{
    auto data = unique_ptr<unsigned char>(new unsigned char[(w * h) << 2]);

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(data.get(), w, h, 8, w << 2,
                                                 colorSpace,
                                                 kCGImageAlphaPremultipliedLast);

    CFStringRef fontNameRef = CFStringCreateWithCString(kCFAllocatorDefault, fontName, kCFStringEncodingUTF8);
    CTFontRef font = CTFontCreateWithName(fontNameRef, fontSize, nullptr);
    CGRect textRect = CGRectMake(0.f, 0.f, w, h);
    CGMutablePathRef path = CGPathCreateMutable();
//    CGPathAddRect(path, nullptr, textRect);

    CGRect boundingBox = CTFontGetBoundingBox(font);

    float midHeight = h * 0.5f;
    midHeight -= boundingBox.size.height * 0.5f;

    CGPathAddRect(path, nullptr, CGRectMake(0, midHeight, w, boundingBox.size.height));

    CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, text, kCFStringEncodingUTF8);
    auto strLen = CFStringGetLength(str);
    CFRange range = CFRangeMake(0, strLen);

    CGFloat rgba[] = { fontColor.r, fontColor.g, fontColor.b, fontColor.a };
    CGColorRef color = CGColorCreate(colorSpace, rgba);

    CTTextAlignment paragraphAlignment = kCTTextAlignmentCenter;
    CTParagraphStyleSetting paragraphStyleSetting = {
        kCTParagraphStyleSpecifierAlignment,
        sizeof(paragraphAlignment),
        &paragraphAlignment
    };
    CTParagraphStyleRef paragraphStyle = CTParagraphStyleCreate(&paragraphStyleSetting, 1);

    const void* keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName, kCTParagraphStyleAttributeName };
    const void* values[] = { font, color, paragraphStyle };
    CFDictionaryRef dict = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 3, nullptr, nullptr);

    CFAttributedStringRef attrStr = CFAttributedStringCreate(kCFAllocatorDefault, str, dict);
    CTFramesetterRef frameSetter = CTFramesetterCreateWithAttributedString(attrStr);
    CTFrameRef frame = CTFramesetterCreateFrame(frameSetter, range, path, nullptr);

    CGContextSetTextMatrix(context, CGAffineTransformIdentity);
    CGContextTranslateCTM(context, x, y);
    CGContextScaleCTM(context, 1.f, 1.f);

    CGContextSetRGBFillColor(context, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    CGContextFillRect(context, textRect);

    CTFrameDraw(frame, context);

    CFRelease(frame);
    CFRelease(frameSetter);
    CFRelease(attrStr);
    CFRelease(dict);
    CFRelease(paragraphStyle);
    CFRelease(color);
    CFRelease(str);
    CGPathRelease(path);
    CFRelease(font);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    CFRelease(fontNameRef);

    return data;
}

shared_ptr<Texture>
createTextureFromText(const char* text,
                      const char* fontName,
                      float fontSize,
                      vec4 backgroundColor,
                      vec4 fontColor,
                      unsigned w, unsigned h,
                      float x, float y)
{
    auto data = createTextBitmap(text, fontName, fontSize, backgroundColor, fontColor, w, h, x, y);
    return make_shared<Texture>(data.get(), w, h);
}

void checkGlErrors()
{
    GLenum error = glGetError();
    switch (error) {
        case GL_NO_ERROR: printf("No OpenGL errors\n"); break;
        case GL_INVALID_ENUM: printf("OpenGL error: GL_INVALID_ENUM\n"); break;
        case GL_INVALID_VALUE: printf("OpenGL error: GL_INVALID_VALUE\n"); break;
        case GL_INVALID_OPERATION: printf("OpenGL error: GL_INVALID_OPERATION\n"); break;
        case GL_OUT_OF_MEMORY: printf("OpenGL error: GL_OUT_OF_MEMORY\n"); break;
        default: printf("OpenGL error %u\n", error);
    };
}

GLint getCurrentVertexArrayBinding() {
    GLint binding;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &binding);
    return binding;
}

GLint getCurrentBufferBinding(GLenum target) {
    GLint binding;
    glGetIntegerv(target, &binding);
    return binding;
}

GLint getCurrentProgram() {
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    return program;
}

void printGlBindings(){
    printf("Bindings:\n  Vertex Array: %u\n  Array buffer: %u\n  Element Array buffer: %u\nProgram: %u\n",
           getCurrentVertexArrayBinding(),
           getCurrentBufferBinding(GL_ARRAY_BUFFER_BINDING),
           getCurrentBufferBinding(GL_ELEMENT_ARRAY_BUFFER_BINDING),
           getCurrentProgram());
}
