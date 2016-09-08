//
//  LinesLayer.h
//  Glittering Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

class LinesLayer : public Layer
{
private:
    void addLine(const Model& model,
                 int row0, int col0,
                 int row1, int col1,
                 int dir,
                 int& index,
                 const vec4& color)
    {
        vec2 p[2] = {
            model.getCoord(row0, col0),
            model.getCoord(row1, col1)
        };
        
        vec2 v[4] = {
            p[0] + model.thickness * model.getDirection(dir),
            p[0] + model.thickness * model.getDirection(dir + 1),
            p[1] + model.thickness * model.getDirection(dir + 3),
            p[1] + model.thickness * model.getDirection((dir + 4) % 6)
        };
        
        for (int i = 0; i < 4; i++)
        {
            data.coords.push_back(vec3(v[i], 1));
            data.pos.push_back({ row0, col0 });
            data.colors.push_back(color);
        }
        
        int idx[] = {
            0, 1, 2,
            0, 2, 3
        };
        
        for (int i = 0; i < 6; i++)
            data.indices.push_back(index + idx[i]);
        
        index += 4;
    }

    void initHorizontalLines(const Model& model, int& index)
    {
        for (int row = 0; row < model.rows + 1; row++)
        {
            vec4 color = model.getHorizontalLineColor(row);
            
            for (int col = 0; col < model.columns; col++)
                addLine(model, row, col, row, col + 1, 0, index, color);
        }
    }

    void initVerticalLines(const Model& model, int& index)
    {
        for (int col = 0; col < model.columns + 1; col++)
        {
            vec4 color = model.getVerticalLineColor(col);
            
            for (int row = 0; row < model.rows; row++)
                addLine(model, row, col, row + 1, col, 1, index, color);
        }
    }
    
    void initDiagonalLines(const Model& model, int& index)
    {
        for (int row = 0; row < model.rows + 1; row++)
        {
            vec4 color = model.getHorizontalLineColor(row);
            
            for (int col = 0; col < row; col++)
                addLine(model, row - col, col, row - col - 1, col + 1, 2, index, color);
        }
        
        for (int col = 1; col < model.columns; col++)
        {
            vec4 color = model.getDiagonalLineColor(model.rows - 1, col);
            
            for (int row = model.rows; row > 0; row--) {
                auto nextCol = col + (model.rows - 1 - row);
                if (nextCol >= model.columns)
                    break;
                
                addLine(model, row, nextCol, row - 1, nextCol + 1, 2, index, color);
            }
        }
    }
    
public:
    PointsLayer() : Layer(GL_LINES)
    {}
    
    virtual void initData(const Model& model)
    {
        clearData();
        
        int index = 0;
        initHorizontalLines(model, index);
        initVerticalLines(model, index);
        initDiagonalLines(model, index);
    }
};
