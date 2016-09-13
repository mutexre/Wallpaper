//
//  Vertex.cpp
//  Wallpaper
//
//  Created by mutexre on 10/09/16.
//
//

#include "Model.hpp"

float Vertex::computePower(float t) {
    //if (type == Type::generator) {
        float p = 3;
        return power * 50 * tanh(50 * pow(t, p) / pow(position.x + 0.1, p));// * (0.5 + 0.5 * abs(sin(0.1 * position.y)));
    //}
    return 0;
}

float Vertex::computeFlux(float angle, float t)
{
    float f = 0;
    
    for (int i = 0; i < fluxSpectrum.size(); i++)
    {
        const vec3& band = fluxSpectrum[i];
        
        float amp = band.x;
        float phase = band.y;
        float phaseVelocity = band.z;
        
        f += amp * cos((i + 1) * angle + phase + phaseVelocity * t);
    }
    
    return f;
}

void Vertex::radiateEnergy(float energy, float t)
{
    array<float, 6> flux;
    float minFlux = 0;
    
    for (int i = 0; i < 6; i++)
    {
        float angle = i * M_PI / 3.0f;
        float f = computeFlux(angle, t);
        
        if (i > 0) {
            if (f < minFlux)
                minFlux = f;
        }
        else
            minFlux = f;
        
        flux[i] = f;
    }
    
    float totalFlux = 0;
    for (int i = 0; i < 6; i++)
    {
        float f = flux[i] - minFlux;
        totalFlux += f;
        flux[i] = f;
    }
    
    if (totalFlux > 0) {
        for (int i = 0; i < 6; i++)
            flux[i] /= totalFlux;
    }
    else
        fill(flux.begin(), flux.end(), 1.f / 6.f);
    
    for (int i = 0; i < 6; i++) {
        float radiatedEnergy = energy * flux[i];
        
        Vertex* v = connections.vertices[i];
        if (v)
            v->addEnergy(radiatedEnergy);
        
        Edge* e = connections.edges[i];
        if (e)
            e->addEnergy(radiatedEnergy);
    }
}

Vertex::Vertex(Type type, ivec2 pos, const FluxSpectrum& spectrum, float power) {
    this->type = type;
    this->position = pos;
    this->fluxSpectrum = spectrum;
    this->power = power;
    
    for (int i = 0; i < 6; i++) {
        connections.vertices[i] = nullptr;
        connections.edges[i] = nullptr;
    }
}

Vertex::Type Vertex::getType() const {
    return type;
}

bool Vertex::isGenerator() const {
    return type == Vertex::Type::generator;
}

bool Vertex::isTransmitter() const {
    return type == Vertex::Type::transmitter;
}

bool Vertex::isAcceptor() const {
    return type == Vertex::Type::acceptor;
}

ivec2 Vertex::getPosition() const {
    return position;
}

Vertex* Vertex::getAdjacentVertex(int i) const {
    return connections.vertices[i];
}

void Vertex::setAdjacentVertex(int i, Vertex* v) {
    connections.vertices[i] = v;
}

Edge* Vertex::getEdge(int i) const {
    return connections.edges[i];
}

void Vertex::setEdge(int i, Edge* e) {
    connections.edges[i] = e;
}

Vertex** Vertex::getAdjacentVertices() {
    return &connections.vertices[0];
}

Edge** Vertex::getEdges() {
    return &connections.edges[0];
}

void Vertex::connect(Vertex* a, int slotA, Vertex* b, int slotB, Edge* edge)
{
    a->setAdjacentVertex(slotA, b);
    a->setEdge(slotA, edge);
    
    b->setAdjacentVertex(slotB, a);
    b->setEdge(slotB, edge);
}

void Vertex::disableConnection(int i) {
    setAdjacentVertex(i, nullptr);
    setEdge(i, nullptr);
}

float Vertex::getEnergy() const {
    return E.get(0);
}

Log<float>& Vertex::getEnergyLog() {
    return E;
}

void Vertex::generatePower(float t, float dt) {
    addEnergy(computePower(t) * dt);
}

void Vertex::addEnergy(float dE)
{
    float energy = E.get(0);
    float newE = energy + dE;
    E.add(newE);
    accumulatedE += newE - energy;
}

const Log<float>& Vertex::getEnergyLog() const {
    return E;
}

void Vertex::radiateAccumulatedEnergy(float t) {
    radiateEnergy(accumulatedE, t);
    accumulatedE = 0;
}

void Vertex::resetAccumulatedEnergy() {
    accumulatedE = 0;
}
