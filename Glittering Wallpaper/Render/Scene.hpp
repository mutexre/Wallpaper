//
//  Scene.hpp
//  Glittering Wallpaper
//
//  Created by mutexre on 05/09/16.
//
//

#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include "Program.hpp"
#include "Helper.hpp"
#include "Model.hpp"
#include "Layer.hpp"
#include "PointsLayer.hpp"
#include "LinesLayer.hpp"
#include "TrianglesLayer.hpp"
//#include "OceanFFT.h"
//#include "Ocean.hpp"
//#include "Height.hpp"
//#include "Philipps.hpp"

#pragma once

using namespace std;
using namespace glm;

class Scene
{
private:
    Model model;
    
    PointsLayer points;
    LinesLayer lines;
    TrianglesLayer triangles;
    
    vec2 scale{ 1, 1 };
    vec2 translation{ 0, 0 };
    
    //OceanFFT ocean{ 7, 7, 10, 0 };
    
    /*struct {
        unique_ptr<Ocean> points, lines, triangles;
    }
    ocean;*/
    
    unique_ptr<Texture> texture;
    unsigned char* imageData;
    
    //int nx, ny;
    
    int frame = 0;

private:
    void initGeometryData()
    {
        struct {
            shared_ptr<Program> points, lines, triangles;
        }
        program = {
            loadProgram("Points", "Fragment"),
            loadProgram("Lines", "Fragment"),
            loadProgram("Triangles", "Triangles")
        };

        points.setProgram(program.points);
        points.initData(model);
        points.initBuffers();
        
        lines.setProgram(program.lines);
        lines.initData(model);
        lines.initBuffers();
        
        triangles.setProgram(program.triangles);
        triangles.initData(model);
        triangles.initBuffers();
    }
    
    void initTextures()
    {
        texture = unique_ptr<Texture>(new Texture("galaxy.jpg"));
        //imageData = new float[4 * fft.m * fft.n];
    }
    
    /*void initOcean()
    {
        int lx, ly;
        lx = ly = 2000;
        
        nx = ny = 64;
        
        float windSpeed = 100;
        int windAlignment = 2;
        float minWaveSize = 0.1;
        
        float amp = 1e-6;
        float motionFactor = 0.5;
        
        Philipps philipps(lx, ly, nx, ny, windSpeed, windAlignment, minWaveSize, amp);
        
        Height height0(nx, ny);
        ocean.points = unique_ptr<Ocean>(new Ocean(lx, ly, nx, ny, motionFactor));
        height0.generate_philipps(&philipps);
        ocean.points->generate_height(&height0);
        
        Height height1(nx, ny);
        ocean.lines = unique_ptr<Ocean>(new Ocean(lx, ly, nx, ny, motionFactor));
        height1.generate_philipps(&philipps);
        ocean.lines->generate_height(&height1);
        
        Height height2(nx, ny);
        ocean.triangles = unique_ptr<Ocean>(new Ocean(lx, ly, nx, ny, motionFactor));
        height2.generate_philipps(&philipps);
        ocean.triangles->generate_height(&height2);
    }*/

public:
    Scene(const Model& model) {
        this->model = model;
        initGeometryData();
        initTextures();
        //initOcean();
        imageData = new unsigned char[4 * 256 * 256];
    }
    
    virtual ~Scene()
    {
        delete[] imageData;
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
        
        points.setTransform(m);
        lines.setTransform(m);
        triangles.setTransform(m);
    }
    
    void render(double t)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //texture->bind();
        points.render(t, frame);
        //lines.render(t, frame);
        //triangles.render(t, frame);
        
        frame++;
        if (frame > 32767)
            frame = 0;
    }
};
