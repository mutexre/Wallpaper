//
//  Triangle.hpp
//  Wallpaper
//
//  Created by mutexre on 12/09/16.
//
//

class Triangle
{
private:
    Vertex* v[3];
    Edge* e[3];
    Log<float> energy{0.0f};

public:
    Triangle(Vertex* a, Vertex* b, Vertex* c) {
        v[0] = a;
        v[1] = b;
        v[2] = c;
    }
    
    Vertex* getVertex(int i) const {
        return v[i];
    }
    
    void setVertex(int i, Vertex* vertex) {
        v[i] = vertex;
    }
    
    const Log<float>& getEnergyLog() const {
        return energy;
    }
    
    Log<float>& getEnergyLog() {
        return energy;
    }
    
    void addEnergy(float dE)
    {
        float e = energy.get(0);
        float newE = e + dE;
        energy.add(newE);
    }
};
