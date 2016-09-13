//
//  Model2.h
//  Wallpaper
//
//  Created by mutexre on 10/09/16.
//
//

class Vertex
{
public:
    enum class Type : unsigned {
        generator = 0,
        transmitter,
        acceptor
    };
    
    // vec3 = amp, phase, phase velocity
    using FluxSpectrum = vector<vec3>;

private:
    Type type;
    ivec2 position;
    float power;
    
    struct {
        Vertex* vertices[6];
        Edge* edges[6];
    }
    connections;
    
    FluxSpectrum fluxSpectrum;
    
    Log<float> E{0.0f};
    float accumulatedE = 0;

private:
    float computePower(float t);
    float computeFlux(float angle, float t);
    
    void radiateEnergy(float e, float t);

public:
    Vertex(Type type, ivec2 pos, const FluxSpectrum& spectrum, float power);
    
    Type getType() const;
    
    bool isGenerator() const;
    bool isTransmitter() const;
    bool isAcceptor() const;
    
    ivec2 getPosition() const;
    
    Vertex* getAdjacentVertex(int i) const;
    void setAdjacentVertex(int i, Vertex* v);
    
    Edge* getEdge(int i) const;
    void setEdge(int i, Edge* e);
    
    Vertex** getAdjacentVertices();
    Edge** getEdges();
    
    static void connect(Vertex* a, int slotA, Vertex* b, int slotB, Edge* edge);
    void disableConnection(int i);
    
    float getEnergy() const;
    const Log<float>& getEnergyLog() const;
    Log<float>& getEnergyLog();
    
    void generatePower(float t, float dt);
    
    void addEnergy(float dE);
    void radiateAccumulatedEnergy(float t);
    void resetAccumulatedEnergy();
};
