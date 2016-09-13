//
//  Graph.cpp
//  Wallpaper
//
//  Created by mutexre on 10/09/16.
//
//

#include <stdlib.h>
#include "Model.hpp"

float getRandomNumber() {
    return float(arc4random()) / 0xffffffff;
}

float getRandomNumber(float min, float max) {
    return min + (max - min) * getRandomNumber();
}

void Graph::createVertices()
{
    Vertex::FluxSpectrum spectrum;
    
    for (int row = 0; row < rows; row++)
    {
        rowsVertexIndices.push_back(vertices.size());
        
        for (int col = 0; col < columns - row % 2; col++)
        {
            spectrum.clear();
            for (int i = 0; i < 8; i++)
            {
                float amp = getRandomNumber();
                float phase = 2 * M_PI * getRandomNumber();
                float phaseVelocity = 1.f * getRandomNumber();
                spectrum.push_back({ amp, phase, phaseVelocity });
            };

            auto type = getVertexType(row, col);
            ivec2 pos(row, col);
            
            Vertex* v = new Vertex(type, pos, spectrum, getRandomNumber(0.5, 1.0));
            vertices.push_back(v);
            generators.push_back(v);
        }
    }
}

void Graph::makeEdges()
{
    for (int row = 0; row < rows; row++)
    {
        auto rowColumns = columns - row % 2;
        for (int col = 0; col < rowColumns; col++)
        {
            Vertex* v[4] = {
                nullptr, nullptr, nullptr, nullptr
            };
            
            v[0] = getVertex(row, col);
            
            if (col < rowColumns - 1)
                v[1] = getVertex(row, col + 1);
            
            if (row < rows - 1)
            {
                if (row % 2 == 0)
                {
                    if (col < rowColumns - 1)
                        v[2] = getVertex(row + 1, col);
                    
                    if (col > 0)
                        v[3] = getVertex(row + 1, col - 1);
                }
                else
                {
                    if (col < rowColumns - 1)
                        v[2] = getVertex(row + 1, col + 1);
                    
                    v[3] = getVertex(row + 1, col);
                }
            }
            
            Edge* newEdges[3] = {
                nullptr, nullptr, nullptr
            };
            
            for (int i = 0; i < 3; i++)
            {
                Vertex* vertex = v[i + 1];
                if (vertex) {
                    auto e = new Edge(v[0], vertex);
                    Vertex::connect(v[0], i, vertex, i + 3, e);
                    edges.push_back(e);
                    newEdges[i] = e;
                }
            }
        }
    }
}

void Graph::makeTriangles()
{
    for (int row = 0; row < rows; row++)
    {
        auto rowColumns = columns - row % 2;
        for (int col = 0; col < rowColumns; col++)
        {
            auto v = getVertex(row, col);
            Edge** edges = v->getEdges();
            
            if (edges[0] && edges[1])
            {
                auto triangle = new Triangle(v, v->getAdjacentVertex(0), v->getAdjacentVertex(1));
                triangles.push_back(triangle);
                
                edges[0]->setTriangle(0, triangle);
                edges[1]->setTriangle(0, triangle);
            }
            
            if (edges[1] && edges[2])
            {
                auto triangle = new Triangle(v, v->getAdjacentVertex(1), v->getAdjacentVertex(2));
                triangles.push_back(triangle);
                
                edges[1]->setTriangle(1, triangle);
                edges[2]->setTriangle(1, triangle);
            }
            
            if (edges[2] && edges[3])
            {
                auto triangle = edges[3]->getTriangle(0);
                edges[2]->setTriangle(0, triangle);
            }
            
            if (edges[0] && edges[5])
            {
                auto triangle = edges[5]->getTriangle(1);
                edges[0]->setTriangle(1, triangle);
            }
        }
    }
}

void Graph::makeConnections()
{
    makeEdges();
    makeTriangles();
}
