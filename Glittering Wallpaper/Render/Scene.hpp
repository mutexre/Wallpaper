//
//  Scene.hpp
//  Wallpaper
//
//  Created by mutexre on 05/09/16.
//
//

#include <vector>
#include <array>
#include <math.h>
#include <glm/glm.hpp>
#include "Program.hpp"
#include "Helper.hpp"
#include "Model.hpp"
#include "Layer.hpp"
#include "PointsLayer.hpp"
#include "LinesLayer.hpp"
#include "TrianglesLayer.hpp"

#pragma once

using namespace std;
using namespace glm;

class Scene
{
private:
    shared_ptr<Grid> grid;
    
    struct {
        array<vec4, 3> points;
        array<vec4, 2> lines;
        vec4 triangles;
    }
    colors;
    
    struct {
        shared_ptr<Program> points, lines, triangles, texturedTriangles;
    }
    programs;
    
    struct {
        unique_ptr<PointsLayer> points;
        unique_ptr<LinesLayer> lines;
        unique_ptr<TrianglesLayer> triangles;
        unique_ptr<TrianglesLayer> texturedTriangles;
    }
    layers;
    
    struct {
        unique_ptr<Texture> picture;
    }
    textures;
    
    vec2 scale{ 1, 1 };
    vec2 translation{ 0, 0 };
    
    int frame = 0;

private:
    void initPrograms()
    {
        programs = {
            loadProgram("Vertex", "Fragment"),
            loadProgram("Vertex", "Fragment"),
            loadProgram("Vertex", "Fragment"),
            loadProgram("TexturedTriangles", "TexturedTriangles")
        };
    }
    
    void initPoints()
    {
        layers.points = unique_ptr<PointsLayer>(new PointsLayer(grid, [&](Vertex* v) -> vec4
        {
            float energy = v->getEnergy();
            Vertex::Type type = v->getType();
            auto c = colors.points[int(type)];
            c.a = 20 * pow(tanh(energy), 3);
            return c;
        }));
        
        layers.points->setProgram(programs.points);
        layers.points->copyCoords();
        layers.points->allocBuffers(grid->getVertexCount(), 0);
        layers.points->syncCoords();
    }
    
    void initLines()
    {
        layers.lines = unique_ptr<LinesLayer>(new LinesLayer(grid, [](Edge* e, Vertex* v) {
            auto t0 = e->getTriangle(0);
            auto t1 = e->getTriangle(1);
            
            //if ((t0 && t0->getEnergyLog()[0] > 0.5) || (t1 && t1->getEnergyLog()[0] > 0.5))
            //    return vec4(0, 0, 0, 0);
            //else
            {
                float vertexEnergy = v->getEnergyLog().getMeanValue();
                float c = (e->isHighlighted() ? 0.7 : 0.35);
                return vec4(c, c, c, 10 * pow(tanh(vertexEnergy), 3));
            }
        }));
        
        layers.lines->setProgram(programs.lines);
        layers.lines->copyCoords();
        layers.lines->allocBuffers(2 * grid->getEdgesCount(), 0);
        layers.lines->syncCoords();
    }
    
    void initTriangles()
    {
        layers.triangles = unique_ptr<TrianglesLayer>(new TrianglesLayer(grid, [](Triangle* triangle) {
            float c = 0.25;
            float energy = triangle->getEnergyLog()[15];//.getMeanValue();
            return vec4(c, c, c, 20 * pow(energy, 3));
        }));
        
        layers.triangles->setProgram(programs.triangles);
        layers.triangles->copyCoords();
        layers.triangles->allocBuffers(3 * grid->getTrianglesCount(), 0);
        layers.triangles->syncCoords();

        layers.texturedTriangles = unique_ptr<TrianglesLayer>(new TrianglesLayer(grid, [](Triangle* triangle) {
            float energy = triangle->getEnergyLog()[49];//.getMeanValue();
            float e = tanh(pow(energy, 1));
            return vec4(0, 0, 0, e > 0.2 ? 30 * pow(e, 3) : 0);
        }));
        
        layers.texturedTriangles->setProgram(programs.texturedTriangles);
        layers.texturedTriangles->copyCoords();
        layers.texturedTriangles->allocBuffers(3 * grid->getTrianglesCount(), 0);
        layers.texturedTriangles->syncCoords();
    }
    
    void initGeometryData()
    {
        initPoints();
        initLines();
        initTriangles();
    }
    
    void initTextures() {
        textures.picture = unique_ptr<Texture>(new Texture("hieroglyphs.jpg"));
    }

public:
    Scene(const shared_ptr<Grid>& grid,
          const array<vec4, 3>& pointColors,
          const array<vec4, 2>& lineColors,
          const vec4& triangleColor)
    {
        this->grid = grid;
        
        colors.points = pointColors;
        colors.lines = lineColors;
        colors.triangles = triangleColor;
        
        initPrograms();
        initGeometryData();
        initTextures();
    }
    
    void setScale(vec2 s) {
        scale = s;
    }
    
    void setTranslate(vec2 t) {
        translation = t;
    }
    
    void setAspectRatio(float aspect)
    {
        mat3 m(1);
        m[0][0] = scale.x;
        m[1][1] = scale.y * aspect;
        m[2][0] = translation.x;
        m[2][1] = translation.y * aspect;
        
        layers.points->setTransform(m);
        layers.lines->setTransform(m);
        layers.triangles->setTransform(m);
        layers.texturedTriangles->setTransform(m);
        
        float w = textures.picture->getW();
        float h = textures.picture->getH();
        
        m = mat3(1);
        m[0][0] = 0.5;
        m[1][1] = -0.5 * ((w / h) / aspect);
        m[2][0] = -0.5;
        m[2][1] = -0.5;
        layers.texturedTriangles->setUVTransform(m);
    }
    
    void render(double t)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        textures.picture->bind();
        
        grid->step(t, 1.f / 60.f);
        
        layers.triangles->clearColorData();
        layers.triangles->copyColors(colors.triangles);
        layers.triangles->syncColors();
        
        layers.points->clearColorData();
        layers.points->copyColors(colors.points);
        layers.points->syncColors();

        layers.lines->clearColorData();
        layers.lines->copyColors(colors.lines);
        layers.lines->syncColors();
        
        layers.texturedTriangles->clearColorData();
        layers.texturedTriangles->copyColors(colors.triangles);
        layers.texturedTriangles->syncColors();
        
        layers.triangles->render();
        layers.lines->render();
        layers.points->render();
        layers.texturedTriangles->render();
    }
};
