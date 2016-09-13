//
//  LinesLayer.h
//  Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

class LinesLayer : public Layer
{
private:
    std::function<vec4(Edge*, Vertex*)> colorFunc;
    
public:
    LinesLayer(const shared_ptr<Grid>& grid,
               const std::function<vec4(Edge*, Vertex*)>& colorFunc)
     : Layer(grid, GL_LINES)
    {
        this->colorFunc = colorFunc;
    }
    
    void copyCoords()
    {
        auto& edges = grid->getEdges();
        for (Edge* e : edges)
        {
            for (int i = 0; i < 2; i++)
            {
                Vertex* v = e->getVertex(i);
                ivec2 pos = v->getPosition();
                vec2 coord = grid->getVertexCoord(pos.x, pos.y);
                data.coords.push_back(vec3(coord, 1));
            }
        }
    }
    
    void copyColors(const array<vec4, 2>& colors)
    {
        auto& edges = grid->getEdges();
        for (Edge* e : edges)
        {
            for (int i = 0; i < 2; i++)
            {
                auto v = e->getVertex(i);
                vec4 color = colorFunc(e, v);
                data.colors.push_back(color);
            }
        }
    }
};
