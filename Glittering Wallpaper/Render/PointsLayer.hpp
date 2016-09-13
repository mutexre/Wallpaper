//
//  PointsLayer.h
//  Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

class PointsLayer : public Layer
{
private:
    std::function<vec4(Vertex*)> colorFunc;

public:
    PointsLayer(const shared_ptr<Grid>& grid,
                const std::function<vec4(Vertex*)>& colorFunc) : Layer(grid, GL_POINTS)
    {
        this->colorFunc = colorFunc;
    }
    
    void copyCoords()
    {
        for (int row = 0; row < grid->getRowsCount(); row++)
        {
            for (int col = 0; col < grid->getColumnsCount() - row % 2; col++)
            {
                vec2 coord = grid->getVertexCoord(row, col);
                data.coords.push_back(vec3(coord, 1));
            }
        }
    }
    
    void copyColors(const array<vec4, 3>& colors)
    {
        auto& vertices = grid->getVertices();
        for (Vertex* v : vertices)
            data.colors.push_back(colorFunc(v));
    }
};
