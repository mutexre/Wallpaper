//
//  TrianglesLayer.h
//  Glittering Wallpaper
//
//  Created by mutexre on 07/09/16.
//
//

#pragma once

class TrianglesLayer : public Layer
{
public:
    virtual void initData(const Model& model)
    {
        clearData();
        
        int index = 0;
        vec4 color = model.getTriangleColor();
        
        for (int row = 0; row < model.rows; row++)
        {
            for (int col = 0; col < model.columns; col++)
            {
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        vec2 xy = model.getCoord(row + i, col + j);
                        data.coords.push_back(vec3(xy, 1));
                        data.colors.push_back(color);
                    }
                }
                
                data.indices.push_back(index);
                data.indices.push_back(index + 1);
                data.indices.push_back(index + 3);
                
                data.indices.push_back(index);
                data.indices.push_back(index + 3);
                data.indices.push_back(index + 2);
                
                index += 4;
            }
        }
    }
};
