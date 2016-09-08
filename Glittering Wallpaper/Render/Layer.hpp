//
//  Layer.h
//  Glittering Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

struct Layer
{
protected:
    struct
    {
        vector<vec3> coords;
        vector<vec2> pos;
        vector<vec4> colors;
        vector<int> indices;
    }
    data;
    
    struct
    {
        GLuint indices = 0;
        GLuint coords = 0;
        GLuint pos = 0;
        GLuint colors = 0;
    }
    buffers;
    
    GLuint vao = 0;
    GLenum mode;
    
    shared_ptr<Program> program;
    mat3 transform;
    float speed = 1;

protected:
    void clearData()
    {
        data.indices.clear();
        data.coords.clear();
        data.pos.clear();
        data.colors.clear();
    }

    void prepareForRender(double t, int frame)
    {
        program->bind();
        program->setUniform("transform", transform);
        program->setUniform("t", float(t));
        program->setUniform("frame", frame);
        program->setUniform("speed", speed);
        glBindVertexArray(vao);
    }

public:
    Layer(GLenum mode) {
        this->mode = mode;
    }
    
    virtual ~Layer() {
        releaseBuffers();
    }
    
    void setProgram(const shared_ptr<Program>& p) {
        this->program = p;
    }
    
    void setTransform(const mat3& m) {
        transform = m;
    }
    
    void setSpeed(float s) {
        speed = s;
    }
    
    virtual void initData(const Model& model) = 0;
    
    void initBuffers()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(4, (GLuint*)&buffers);
        
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned), data.indices.data(), GL_STATIC_DRAW);
        
        auto coordAttr = program->getAttributeLocation("coord");
        glBindBuffer(GL_ARRAY_BUFFER, buffers.coords);
        glEnableVertexAttribArray(coordAttr);
        glVertexAttribPointer(coordAttr, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(GL_ARRAY_BUFFER, data.coords.size() * sizeof(vec3), data.coords.data(), GL_STATIC_DRAW);
        
        auto posAttr = program->getAttributeLocation("pos");
        glBindBuffer(GL_ARRAY_BUFFER, buffers.pos);
        glEnableVertexAttribArray(posAttr);
        glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(GL_ARRAY_BUFFER, data.pos.size() * sizeof(vec2), data.pos.data(), GL_STATIC_DRAW);
        
        auto colorAttr = program->getAttributeLocation("color");
        glBindBuffer(GL_ARRAY_BUFFER, buffers.colors);
        glEnableVertexAttribArray(colorAttr);
        glVertexAttribPointer(colorAttr, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(GL_ARRAY_BUFFER, data.colors.size() * sizeof(vec4), data.colors.data(), GL_STATIC_DRAW);
    }
    
    void releaseBuffers()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(4, reinterpret_cast<GLuint*>(&buffers));
    }
    
    virtual void render(double t, int frame)
    {
        prepareForRender(t, frame);
        glDrawElements(mode, GLsizei(data.indices.size()), GL_UNSIGNED_INT, nullptr);
    }
};
