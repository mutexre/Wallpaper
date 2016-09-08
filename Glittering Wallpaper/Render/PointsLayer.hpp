//
//  PointsLayer.h
//  Glittering Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

class PointsLayer : public Layer
{
public:
    PointsLayer() : Layer(GL_POINTS)
    {}
    
    virtual void initData(const Model& model)
    {
        clearData();
        
        int index = 0;
        
        for (int row = 0; row < model.rows + 1; row++)
        {
            for (int col = 0; col < model.columns + 1; col++)
            {
                vec2 center = model.getCoord(row, col);
                vec2 pos{ row, col };
                vec4 color = model.getColor(row, col);
                
                data.coords.push_back(vec3(center, 1));
                data.pos.push_back(pos);
                data.colors.push_back(color);
                
                for (int dir = 0; dir < 6; dir++)
                {
                    vec2 coord = center + model.thickness * model.getDirection(dir);
                    
                    data.coords.push_back(vec3(coord, 1));
                    data.pos.push_back(pos);
                    data.colors.push_back(color);
                    
                    data.indices.push_back(index);
                    for (int i = 0; i < 2; i++)
                        data.indices.push_back(index + 1 + (dir + i) % 6);
                }
                
                index += 7;
            }
        }
    }
    
    virtual void render(double t, int frame)
    {
        prepareForRender(t, frame);
        glDrawElements(GL_TRIANGLES, GLsizei(data.indices.size()), GL_UNSIGNED_INT, nullptr);
    }
};
