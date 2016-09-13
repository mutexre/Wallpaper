//
//  Layer.h
//  Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

struct Layer
{
protected:
    shared_ptr<Grid> grid;
    
    struct
    {
        vector<vec3> coords;
        vector<vec4> colors;
        vector<int> indices;
    }
    data;
    
    struct
    {
        GLuint indices = 0;
        GLuint coords = 0;
        GLuint colors = 0;
    }
    buffers;
    
    GLuint vao = 0;
    GLenum mode;
    bool useIndicesArray = false;
    
    shared_ptr<Program> program;
    mat3 transform, uvTransform;

protected:
    void prepareForRender()
    {
        program->bind();
        
        if (program->isUniformActive("transform"))
            program->setUniform("transform", transform);
        
        if (program->isUniformActive("uvTransform"))
            program->setUniform("uvTransform", uvTransform);
        
        glBindVertexArray(vao);
    }

public:
    Layer(const shared_ptr<Grid>& grid, GLenum mode) {
        this->grid = grid;
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
    
    void setUVTransform(const mat3& m) {
        uvTransform = m;
    }
    
    void clearData()
    {
        data.indices.clear();
        data.coords.clear();
        data.colors.clear();
    }
    
    void clearColorData() {
        data.colors.clear();
    }
    
    void allocBuffers(size_t vertexCount, size_t indexCount)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(3, (GLuint*)&buffers);
        
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.indices);
        if (indexCount > 0)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned), nullptr, GL_STATIC_DRAW);
        
        auto coordAttr = program->getAttributeLocation("coord");
        glBindBuffer(GL_ARRAY_BUFFER, buffers.coords);
        glEnableVertexAttribArray(coordAttr);
        glVertexAttribPointer(coordAttr, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vec3), nullptr, GL_STATIC_DRAW);
        
        auto colorAttr = program->getAttributeLocation("color");
        glBindBuffer(GL_ARRAY_BUFFER, buffers.colors);
        glEnableVertexAttribArray(colorAttr);
        glVertexAttribPointer(colorAttr, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
    }
    
    void syncIndices()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.indices);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data.indices.size() * sizeof(unsigned), data.indices.data());
    }
    
    void syncCoords()
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffers.coords);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data.coords.size() * sizeof(vec3), data.coords.data());
    }
    
    void syncColors()
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffers.colors);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data.colors.size() * sizeof(vec4), data.colors.data());
    }
    
    void releaseBuffers()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(3, reinterpret_cast<GLuint*>(&buffers));
    }
    
    virtual void render()
    {
        prepareForRender();
        
        if (useIndicesArray)
            glDrawElements(mode, GLsizei(data.indices.size()), GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mode, 0, GLsizei(data.coords.size()));
    }
};
