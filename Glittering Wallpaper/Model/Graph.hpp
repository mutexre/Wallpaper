//
//  Grid.h
//  Wallpaper
//
//  Created by mutexre on 10/09/16.
//
//

class Graph
{
protected:
    int rows, columns;
    vector<Vertex*> vertices;
    vector<size_t> rowsVertexIndices;
    vector<Vertex*> generators;
    vector<Edge*> edges;
    vector<Triangle*> triangles;

private:
    void createVertices();
    
    size_t getVertexIndex(int row, int col) const {
        return rowsVertexIndices[row] + col;
    }
    
    void makeEdges();
    void makeTriangles();
    void makeConnections();
    
private:
    void destroyVertices()
    {
        for (Vertex* v : vertices)
            delete v;
    }
    
    void destroyEdges()
    {
        for (Edge* e : edges)
            delete e;
    }
    
    void destroyTriangles()
    {
        for (Triangle* t : triangles)
            delete t;
    }
    
    void destroyItems()
    {
        destroyVertices();
        destroyEdges();
        destroyTriangles();
    }

private:
    void generatePower(float t, float dt)
    {
        for (Vertex* v : generators)
            v->generatePower(t, dt);
    }
    
    void radiateAccumulatedEnergy(float t)
    {
        for (Vertex* v : vertices)
            v->radiateAccumulatedEnergy(t);

        for (Edge* e : edges)
            e->radiateAccumulatedEnergy(t);
    }
    
    void resetAccumulatedEnergy()
    {
        for (Vertex* v : vertices)
            v->resetAccumulatedEnergy();
        
        for (Edge* e : edges)
            e->resetAccumulatedEnergy();
    }
    
public:
    Graph(int rows, int columns)
    {
        this->rows = rows;
        this->columns = columns;
    }
    
    virtual void init() {
        createVertices();
        makeConnections();
    }
    
    virtual ~Graph() {
        destroyItems();
    }
    
    int getRowsCount() const {
        return rows;
    }
    
    int getColumnsCount() const {
        return columns;
    }
    
    const Vertex* getVertex(int row, int col) const {
        return vertices[getVertexIndex(row, col)];
    }
    
    Vertex* getVertex(int row, int col) {
        return vertices[getVertexIndex(row, col)];
    }
    
    size_t getVertexCount() const {
        return vertices.size();
    }
    
    size_t getEdgesCount() const {
        return edges.size();
    }
    
    size_t getTrianglesCount() const {
        return triangles.size();
    }
    
    const vector<Vertex*>& getVertices() const {
        return vertices;
    }
    
    const vector<Edge*>& getEdges() const {
        return edges;
    }
    
    const vector<Triangle*>& getTriangles() const {
        return triangles;
    }
    
    virtual Vertex::Type getVertexType(int row, int col) const = 0;
    
    void step(float t, float dt)
    {
        generatePower(t, dt);
        
        for (int i = 0; i < 1; i++)
            radiateAccumulatedEnergy(t);
        
        resetAccumulatedEnergy();
    }
};

