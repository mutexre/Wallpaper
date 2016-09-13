//
//  TrianglesLayer.h
//  Wallpaper
//
//  Created by mutexre on 07/09/16.
//
//

#pragma once

class TrianglesLayer : public Layer
{
private:
    std::function<vec4(Triangle*)> colorFunc;

public:
    TrianglesLayer(const shared_ptr<Grid>& grid,
                   const std::function<vec4(Triangle*)>& colorFunc)
     : Layer(grid, GL_TRIANGLES)
    {
        this->colorFunc = colorFunc;
    }
    
    void copyCoords()
    {
        auto& triangles = grid->getTriangles();
        for (Triangle* tri : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                Vertex* v = tri->getVertex(i);
                ivec2 pos = v->getPosition();
                vec2 coord = grid->getVertexCoord(pos.x, pos.y);
                data.coords.push_back(vec3(coord, 1));
            }
        }
    }
    
    void copyColors(const vec4& color)
    {
        auto& triangles = grid->getTriangles();
        for (Triangle* triangle : triangles)
        {
            vec4 color = colorFunc(triangle);
            for (int i = 0; i < 3; i++)
                data.colors.push_back(color);
        }
    }
};
