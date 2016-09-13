//
//  Edge.h
//  Wallpaper
//
//  Created by mutexre on 10/09/16.
//
//

class Edge
{
private:
    Vertex* v[2] = {
        nullptr, nullptr
    };
    
    Triangle* triangles[2] = {
        nullptr, nullptr
    };
    
    Log<float> energy{0.0f};
    float accumulatedEnergy = 0;
    bool highlighted = false;

public:
    Edge()
    {}
    
    Edge(Vertex* a, Vertex* b) {
        v[0] = a;
        v[1] = b;
    }
    
    Vertex* getVertex(int i) const {
        return v[i];
    }
    
    void setVertex(int i, Vertex* vertex) {
        v[i] = vertex;
    }
    
    Triangle* getTriangle(int i) const {
        return triangles[i];
    }
    
    void setTriangle(int i, Triangle* triangle) {
        triangles[i] = triangle;
    }
    
    const Log<float>& getEnergyLog() const {
        return energy;
    }
    
    Log<float>& getEnergyLog() {
        return energy;
    }
    
    bool isHighlighted() const {
        return highlighted;
    }
    
    void setHighlighted(bool value) {
        highlighted = value;
    }
    
    void addEnergy(float dE)
    {
        float e = energy.get(0);
        float newE = e + dE;
        energy.add(newE);
        accumulatedEnergy += newE - e;
    }
    
    void radiateAccumulatedEnergy(float t) {
        radiateEnergy(accumulatedEnergy, t);
        //accumulatedEnergy = 0;
    }
    
    void resetAccumulatedEnergy() {
        accumulatedEnergy = 0;
    }
    
    void radiateEnergy(float e, float t);
};
