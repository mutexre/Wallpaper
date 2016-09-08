/*
FFTOcean - Copyright (C) 2016 - Olivier Deiss - olivier.deiss@gmail.com

FFTOcean is a C++ implementation of researcher J. Tessendorf's paper
"Simulating Ocean Water". It is a real-time simulation of ocean water
in a 3D world. The (reverse) FFT is used to compute the 2D wave height
field from the Philipps spectrum. It is possible to adjust parameters
such as wind speed, direction and strength, wave choppiness, and sea depth.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <cmath>
#include <iostream>

#include "Height.hpp"
#include "Ocean.hpp"

/*
Initializes the variables and allocates space for the vectors.
*/
Ocean::Ocean(const double p_lx, const double p_ly, const int p_nx, const int p_ny, const double p_motion_factor) :
    lx(p_lx),
    ly(p_ly),
    nx(p_nx),
    ny(p_ny),
    motion_factor(p_motion_factor) {
    height0I.resize(nx+1);
    height0R.resize(nx+1);
    HR.resize(nx+1);
    HI.resize(nx+1);
    hr.resize(ny+1);
    hi.resize(ny+1);
    for(vec_vec_d_it it=HR.begin() ; it!=HR.end() ; it++) it->resize(ny+1);
    for(vec_vec_d_it it=HI.begin() ; it!=HI.end() ; it++) it->resize(ny+1);
    for(vec_vec_d_it it=hr.begin() ; it!=hr.end() ; it++) it->resize(nx+1);
    for(vec_vec_d_it it=hi.begin() ; it!=hi.end() ; it++) it->resize(nx+1);
    ffty.reserve(nx);
    fftx.reserve(ny);
    for(int i=0 ; i<nx ; i++) ffty.push_back(new FFT(ny, &HR[i], &HI[i]));
    for(int i=0 ; i<ny ; i++) fftx.push_back(new FFT(nx, &hr[i], &hi[i]));
}


/*
Free memory.
*/
Ocean::~Ocean() {
    for(int i=0 ; i<nx ; i++) delete ffty[i];
    for(int i=0 ; i<ny ; i++) delete fftx[i];
}

/*
Computes the initial random height field.
*/
void Ocean::generate_height(Height* const height) {
    /* real part */
    for(vec_vec_d_it itx=height0R.begin() ; itx!=height0R.end() ; itx++) {
        itx->resize(ny+1);
        height->init_fonctor(std::distance(height0R.begin(), itx));
        std::generate(itx->begin(), itx->end(), *height);
    }
    /* imaginary part */
    for(vec_vec_d_it itx=height0I.begin() ; itx!=height0I.end() ; itx++) {
        itx->resize(ny+1);
        height->init_fonctor(std::distance(height0I.begin(), itx));
        std::generate(itx->begin(), itx->end(), *height);
    }
}

/*
Does all the calculus needed for the ocean. This basically means
updating the spectrum and computing the 2D reverse FFT to get the wave shape.
The wave shape is stored in the hr vector. hi is generated by the FFT but
is useless in our application.
*/
void Ocean::main_computation(float t) {
    for(int x=0 ; x<nx ; x++) {
        get_sine_amp(x, static_cast<double>(motion_factor * t)/1000, &HR[x], &HI[x]);
        ffty[x]->reverse();
    }
    for(int y=0 ; y<ny ; y++) {
        int      x;
        vec_d_it it;
        for(it=hr[y].begin(), x=0 ; it!=hr[y].end() ; it++, x++) *it = HR[x][y];
        for(it=hi[y].begin(), x=0 ; it!=hi[y].end() ; it++, x++) *it = HI[x][y];
        fftx[y]->reverse();
    }
}

/*
Updates the wave height field.
*/
void Ocean::get_sine_amp(const int x, const double time, std::vector<double>* const p_HR, std::vector<double>* const p_HI) const {
    double   A;
    double   L = 0.1;
    int      y;
    vec_d_it itR;
    vec_d_it itI;
    for(itR=p_HR->begin(), itI=p_HI->begin(), y=0 ; itR!=p_HR->end() ; itR++, itI++, y++) {
        A = time*sqrt(9.81 * sqrt(pow((2*M_PI*x)/lx, 2)+pow((2*M_PI*y)/ly, 2)) * (1+(pow((2*M_PI*x)/lx, 2)+pow((2*M_PI*y)/ly, 2))*pow(L, 2)));
        *itR = height0R[x][y]*cos(A) - height0I[x][y]*sin(A) + height0R[nx-x][ny-y]*cos(-A) + height0I[nx-x][ny-y]*sin(-A);
        *itI = height0I[x][y]*cos(A) + height0R[x][y]*sin(A) - height0I[nx-x][ny-y]*cos(-A) + height0R[nx-x][ny-y]*sin(-A);
    }
}

/*
Creates an array that OpenGL can directly use - X
*/
void Ocean::init_gl_vertex_array_x(const int y, double* const vertices) const {
    for(int x=0 ; x<nx ; x++) {
        vertices[3*x]   = (lx/nx)*x;
        vertices[3*x+2] = (ly/ny)*y;
    }
    vertices[3*nx]   = lx;
    vertices[3*nx+2] = (ly/ny)*y;
}

/*
Creates an array that OpenGL can directly use - Y
*/
void Ocean::init_gl_vertex_array_y(const int x, double* const vertices) const {
    for(int y=0 ; y<ny ; y++) {
        vertices[3*y]   = (lx/nx)*x;
        vertices[3*y+2] = (ly/ny)*y;
    }
    vertices[3*ny]   = (lx/nx)*x;
    vertices[3*ny+2] = ly;
}

/*
Creates an array that OpenGL can directly use - X
*/
void Ocean::gl_vertex_array_x(const int y, double* const vertices) const {
    for(int x=0 ; x<nx ; x++) {
        vertices[3*x+1] = pow(-1, x+y)*hr[y][x];
    }
    vertices[3*nx+1] = pow(-1, nx+y)*hr[y][0];
}

/*
Creates an array that OpenGL can directly use - Y
*/
void Ocean::gl_vertex_array_y(const int x, double* const vertices) const {
    for(int y=0 ; y<ny ; y++) {
        vertices[3*y+1] = pow(-1, x+y)*hr[y][x];
    }
    vertices[3*ny+1] = pow(-1, x+ny)*hr[0][x];
}

void Ocean::copyImageData(unsigned char* data, int component, float min, float max)
{
    for (int y = 0; y < ny; y++)
    {
        for (int x = 0; x < nx; x++)
        {
            float v = hr[y][x];
            if (v < min) v = min;
            if (v > max) v = max;
            
            unsigned char value = 255 * ((v - min) / (max - min));
            
            int i = 4 * (y * nx + x);
            data[i + component] = value;
        }
    }
}