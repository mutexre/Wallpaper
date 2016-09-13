//
//  Model.h
//  Wallpaper
//
//  Created by mutexre on 06/09/16.
//
//

#pragma once

#define kSinPi3 0.866025403784439

class Grid : public Graph
{
private:
    int period;
    vec2 cellSize;

public:
    Grid(int rows, int columns, int period, float cellWidth) : Graph(rows, columns)
    {
        this->period = period;
        this->cellSize = vec2(cellWidth, cellWidth * kSinPi3);
    }
    
    virtual void init()
    {
        Graph::init();
        
        for (Edge* e : edges)
        {
            Vertex* v[2] = {
                e->getVertex(0),
                e->getVertex(1)
            };
            
            ivec2 pos[2] = {
                v[0]->getPosition(),
                v[1]->getPosition()
            };
            
            if (pos[0].x == pos[1].x && pos[0].x % period == 0) {
                e->setHighlighted(true);
            }
            else
            {
                if ((pos[0].y - pos[0].x % 2 + pos[0].x) % period == 0 &&
                    (pos[1].y - pos[1].x % 2 + pos[1].x) % period == 0)
                {
                    e->setHighlighted(true);
                }
                else if ((pos[0].y - pos[0].x % 2 - pos[0].x) % period == 0 &&
                         (pos[1].y - pos[1].x % 2 - pos[1].x) % period == 0)
                {
                    e->setHighlighted(true);
                }
            }
        }
    }
    
    vec2 getVertexCoord(int row, int col) const
    {
        float x = (col + 0.5f * (row % 2)) * cellSize.x;
        float y = row * cellSize.y;
        return vec2(x, y);
    }
    
    virtual Vertex::Type getVertexType(int row, int col) const
    {
        int colMinusRowMod2 = col - row % 2;
        bool b = ((colMinusRowMod2 + row) % period == 0);
        bool c = ((colMinusRowMod2 - row) % period == 0);
        
        if (b && c)
            return Vertex::Type::generator;
        
        bool a = (row % period == 0);
        
        if (a || b || c)
            return Vertex::Type::transmitter;
        
        return Vertex::Type::acceptor;
    }
};
