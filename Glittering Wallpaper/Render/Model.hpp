//
//  Model.h
//  Glittering Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

#define kSinPi3 0.866025403784439

struct Model
{
public:
    int rows, columns;
    int period;
    float cellSize;
    float thickness;
    vec4 colors[3];
    vec4 triangleColor;

private:
    vec2 directions[6];
    
    void calcDirVectors() {
        float angle = -M_PI / 6;
        for (int i = 0; i < 6; i++) {
            directions[i].x = cos(angle);
            directions[i].y = sin(angle);
            angle += M_PI / 3;
        }
    }

public:
    Model()
    {}
    
    Model(int rows, int columns, int period,
          float cellSize, float thickness,
          const vec4& color0,
          const vec4& color1,
          const vec4& color2,
          const vec4& triangleColor)
    {
        this->period = period;
        this->rows = rows;
        this->columns = columns;
        this->cellSize = cellSize;
        this->thickness = thickness;
        
        colors[0] = color0;
        colors[1] = color1;
        colors[2] = color2;
        this->triangleColor = triangleColor;
        
        calcDirVectors();
    }
    
    vec2 getCoord(int row, int col) const {
        return vec2(col * cellSize + row * cellSize * 0.5, row * cellSize * kSinPi3);
    }
    
    vec4 getColor(int row, int col) const
    {
        bool a = (row % period == 0);
        bool b = (col % period == 0);
        bool c = ((row + col) % period == 0);
        
        if ((a && b) || (a && c) || (b && c))
            return colors[0];
        
        if (a || b || c)
            return colors[1];
        
        return colors[2];
    }
    
    enum class LineDirection
    {
        horizontal,
        vertical,
        diagonal
    };
    
    vec4 getHorizontalLineColor(int row) const
    {
        if (row % period == 0)
            return colors[1];
        return colors[2];
    }
    
    vec4 getVerticalLineColor(int col) const
    {
        if (col % period == 0)
            return colors[1];
        return colors[2];
    }

    vec4 getDiagonalLineColor(int row, int col) const
    {
        if ((row + col) % period == 0)
            return colors[1];
        return colors[2];
    }
    
    vec4 getTriangleColor() const {
        return triangleColor;
    }
    
    const vec2& getDirection(int i) const {
        return directions[i];
    }
};
