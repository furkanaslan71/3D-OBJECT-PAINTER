#include "MeshGraph.h"
#include "BinaryHeap.h"

// For printing
#include <fstream>
#include <iostream>
#include <sstream>

MeshGraph::MeshGraph(const std::vector<Double3>& vertexPositions,
                     const std::vector<IdPair>& edges)
{
    // TODO:
    vertices.resize(vertexPositions.size());
    for(int i = 0; i<vertexPositions.size(); i++)
    {
        Vertex newOne;
        newOne.id = i;
        newOne.position3D = vertexPositions[i];
        vertices[i] = newOne;
    }
    adjList.resize(vertexPositions.size());
    for(int i = 0; i<edges.size(); i++)
    {
        adjList[edges[i].vertexId0].push_back(&vertices[edges[i].vertexId1]);
        adjList[edges[i].vertexId1].push_back(&vertices[edges[i].vertexId0]);
    }
}

double MeshGraph::AverageDistanceBetweenVertices() const
{
    // TODO:
    double result = 0;
    for(int i = 0; i<adjList.size(); i++)
    {
        std::list<Vertex*> theList = adjList[i];
        std::list<Vertex*>::const_iterator it;
        for(it = theList.begin(); it != theList.end(); it++)
        {
            result += Double3::Distance(vertices[i].position3D, (*it)->position3D);
        }
    }
    return (result / (2*(double) TotalEdgeCount()));
}

double MeshGraph::AverageEdgePerVertex() const
{
    // TODO:
    return (double)TotalEdgeCount()/TotalVertexCount();
}

int MeshGraph::TotalVertexCount() const
{
    // TODO:
    return vertices.size();
}

int MeshGraph::TotalEdgeCount() const
{
    // TODO:
    int result = 0;
    for(int i = 0; i<adjList.size(); i++)
    {
        result += adjList[i].size();
    }
    return result/2;
}

int MeshGraph::VertexEdgeCount(int vertexId) const
{
    // TODO:
    if(!contains(vertexId)) return -1;
    return adjList[vertexId].size();
}

void MeshGraph::ImmediateNeighbours(std::vector<int>& outVertexIds,
                                    int vertexId) const
{
    // TODO:
    outVertexIds.resize(0);
    if(!contains(vertexId)) return;
    std::list<Vertex*> theList = adjList[vertexId];
    std::list<Vertex*>::const_iterator it;
    for(it = theList.begin(); it != theList.end(); it++)
    {
        outVertexIds.push_back((*it)->id);
    }
}

void MeshGraph::PaintInBetweenVertex(std::vector<Color>& outputColorAllVertex,
                                     int vertexIdFrom, int vertexIdTo,
                                     const Color& color) const
{
    // TODO:
    outputColorAllVertex.resize(0);
    if(!(contains(vertexIdFrom) && contains(vertexIdTo))) return ;

    outputColorAllVertex.resize(vertices.size());
    for(int i = 0; i<outputColorAllVertex.size(); i++)
    {
        outputColorAllVertex[i].b = 0;
        outputColorAllVertex[i].g = 0;
        outputColorAllVertex[i].r = 0;
    }
    int theSize = TotalVertexCount();

    std::vector<int> previous;
    std::vector<double> distance;
    BinaryHeap pq;
    previous.resize(theSize);
    distance.resize(theSize);
    std::vector<HeapElement> temp;
    temp.resize(theSize);
    for(int i = 0; i<theSize; i++)
    {
        distance[i] = pow(10, 308);
        previous[i] = -42;
    }
    distance[vertexIdFrom] = 0;
    for(int i = 0; i<theSize; i++)  pq.Add(i, distance[i]);
    while(pq.HeapSize() != 0)
    {
        int outUID;
        double outW;
        pq.PopHeap(outUID, outW);
        std::list<Vertex*>::const_iterator it;
        for(it = adjList[outUID].begin(); it != adjList[outUID].end(); it++)
        {
            double new_dist = distance[outUID] + Double3::Distance(vertices[outUID].position3D, (*it)->position3D);
            if(new_dist < distance[(*it)->id])
            {
                distance[(*it)->id] = new_dist;
                previous[(*it)->id] = outUID;
                pq.ChangePriority((*it)->id, new_dist);
            }
        }
    }
    int tmp = vertexIdTo;
    while(tmp != -42)
    {
        if(tmp != vertexIdFrom && previous[tmp] == -42) break;
        outputColorAllVertex[tmp] = color;
        tmp = previous[tmp];
    }
}

void MeshGraph::PaintInRangeGeodesic(std::vector<Color>& outputColorAllVertex,
                                    int vertexId, const Color& color,
                                    int maxDepth, FilterType type,
                                    double alpha) const
{
    // TODO:
    //checking edge-case
    outputColorAllVertex.resize(0);
    if(!contains(vertexId)) return;
    //initializing the output array to black
    int theSize = TotalVertexCount();
    Color black; black.b = 0; black.g = 0; black.r = 0;
    outputColorAllVertex.resize(theSize, black);
    //declarations
    std::vector<bool> mark(theSize, false);
    std::vector<double> distance(theSize, 0);
    std::vector<int> previous(theSize, -42);
    std::vector<int> depth(theSize, 0);
    std::vector<int> outVertexIds;
    BinaryHeap pq;
    //adding the first vertex to the pq
    pq.Add(vertexId, 0);
    mark[vertexId] = true;
    int zart = 0;
    while(pq.HeapSize() != 0)
    {
        int outUID;
        double outW;        
        pq.PopHeap(outUID, outW);
        //maxDepth terminating condition check
        if(depth[outUID] <= maxDepth) outVertexIds.push_back(outUID);
        if(depth[outUID] >= maxDepth) continue;
        //looking for neighbours
        std::list<Vertex*>::const_iterator it;
        std::list<Vertex*> theList = adjList[outUID];
        theList.sort(compare);
        for(it = theList.begin(); it != theList.end(); it++)
        {
            if(mark[(*it)->id] != true)
            {
                mark[(*it)->id] = true;
                previous[(*it)->id] = outUID;
                distance[(*it)->id] += distance[outUID];
                distance[(*it)->id] += Double3::Distance(vertices[outUID].position3D, (*it)->position3D);
                depth[(*it)->id] = depth[outUID] + 1;
                pq.Add((*it)->id, ++zart);
            }
        }
    }
    
    for(int i = 0; i<outVertexIds.size(); i++)
    {
        Color colour = filtered(color, type, alpha, distance[outVertexIds[i]]);
        outputColorAllVertex[outVertexIds[i]] = colour;
    }
}

void MeshGraph::PaintInRangeEuclidian(std::vector<Color>& outputColorAllVertex,
                                      int vertexId, const Color& color,
                                      int maxDepth, FilterType type,
                                      double alpha) const
{
    // TODO:
    //checking edge-case
    outputColorAllVertex.resize(0);
    if(!contains(vertexId)) return;
    //initializing the output array to black
    int theSize = TotalVertexCount();
    Color black; black.b = 0; black.g = 0; black.r = 0;
    outputColorAllVertex.resize(theSize, black);
    std::vector<bool> mark(theSize, false);
    std::vector<int> outVertexIds;
    BinaryHeap pq;
    pq.Add(vertexId, 0);
    mark[vertexId] = true;
    while(pq.HeapSize() != 0)
    {
        int outUID;
        double outW;
        pq.PopHeap(outUID, outW);
        if(outW > maxDepth) break;
        outVertexIds.push_back(outUID);
        std::list<Vertex*>::const_iterator it;
        std::list<Vertex*> theList = adjList[outUID];
        for(it = theList.begin(); it != theList.end(); it++)
        {
            if(mark[(*it)->id] != true)
            {
                mark[(*it)->id] = true;
                pq.Add((*it)->id, outW + 1);
            }
        }
    }
    for(int i = 0; i<outVertexIds.size(); i++)
    {
        double distance = Double3::Distance(vertices[vertexId].position3D, vertices[outVertexIds[i]].position3D);
        Color colour = filtered(color, type, alpha, distance);
        outputColorAllVertex[outVertexIds[i]] = colour;
    }
}

void MeshGraph::WriteColorToFile(const std::vector<Color>& colors,
                                 const std::string& fileName)
{
    // IMPLEMENTED
    std::stringstream s;
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        int r = static_cast<int>(colors[i].r);
        int g = static_cast<int>(colors[i].g);
        int b = static_cast<int>(colors[i].b);

        s << r << ", " << g << ", " << b << "\n";
    }
    std::ofstream f(fileName.c_str());
    f << s.str();
}

void MeshGraph::PrintColorToStdOut(const std::vector<Color>& colors)
{
    // IMPLEMENTED
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        std::cout << static_cast<int>(colors[i].r) << ", "
                  << static_cast<int>(colors[i].g) << ", "
                  << static_cast<int>(colors[i].b) << "\n";
    }
}


bool MeshGraph::contains(int vertexID) const
{
    for(int i = 0; i<vertices.size(); i++)
    {
        if(vertices[i].id == vertexID) return true;
    }
    return false;
}

Color MeshGraph::filtered(Color color, FilterType type, double alpha, double distance) const
{
    
    double k = (type == FILTER_BOX ? box(alpha, distance) : gaussian(alpha, distance));
    double newB = (double)color.b*k;
    double newG = (double)color.g*k;
    double newR = (double)color.r*k;
    color.b = (unsigned char)newB;
    color.g = (unsigned char)newG;
    color.r = (unsigned char)newR;
    return color;
}

double MeshGraph::gaussian(double alpha, double distance) const
{
    return std::exp(-(distance*distance)/(alpha*alpha));
}
double MeshGraph::box(double alpha, double distance) const
{
    if(-alpha <= distance && distance <= alpha) return 1;
    else return 0;
}

bool MeshGraph::compare(const Vertex* first, const Vertex* second)
{
    return first->id < second->id;
}