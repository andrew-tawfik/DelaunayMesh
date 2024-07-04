
#ifndef MESH_H
#define MESH_H

#include "point.h"
#include "triangle.h"
#include <vector>

class Mesh {
private:
    std::vector<Point> vecPoints;
    std::vector<Triangle> vecTriangles;

public:
    Mesh();

    std::vector<Point> getPoints();
    void setPoints(std::vector<Point> vecPoints);
    void buildMesh();
    std::vector<Triangle> sortTriangles(std::vector<Triangle> vecTri);
    void flipEdge(Triangle triTriangle1, Triangle triTriangle2, Point p);
    void edgeFlipping(Point p);
};

#endif // MESH_H
