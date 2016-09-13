//
//  Edge.cpp
//  Wallpaper
//
//  Created by mutexre on 12/09/16.
//
//

#include "Model.hpp"

void Edge::radiateEnergy(float e, float t) {
    for (Triangle* triangle : triangles) {
        if (triangle)
            triangle->addEnergy(0.5 * e);
    }
}
