
#ifndef MESH_H
#define MESH_H

#include "point.h"
#include "triangle.h"
#include <vector>

class Mesh {
private:
    std::vector<Point> vecPoints;
    std::vector<Triangle> vecTriangles;

    std::vector <int> triangleIndices;
    std::vector <int> pointIndices;

public:
    Mesh(std::vector<Point> points);

    std::vector<Point> getPoints();
    void setPoints(std::vector<Point> vecPoints);

    std::vector <int> getPointIndices();
    void setPointIndices(int numPoints);

    std::vector <int> getTriangleIndices();
    void setTriangleIndices(int numPoints);


    Triangle superTriangle();


    void buildMesh();

    void createTriangles(int index, Triangle containingTriangle);  // Create new triangles

    Triangle findContainingTriangle(Point& p);  // Find the triangle that contains the point
    void checkNeighboringCircumcircles(Point& p);  // Check if the point is in neighbors' circumcircles
    void updateNeighbors();  // Update neighbors between triangles
    void printMesh();  // Debugging function to print the mesh



};

#endif // MESH_H
