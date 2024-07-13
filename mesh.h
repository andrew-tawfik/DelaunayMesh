#ifndef MESH_H
#define MESH_H

#include "point.h"
#include "triangle.h"
#include <vector>

// Class representing a 2D mesh composed of points and triangles
class Mesh {
private:
    std::vector<Point> vecPtShape;  // Vector of points defining the shape
    std::vector<Triangle> vecTriShape;  // Vector of triangles defining the shape

    std::vector<int> viTriangleIndices;  // Indices of triangles in the mesh
    std::vector<int> viPointIndices;  // Indices of points in the mesh

public:
    // Constructor to initialize mesh with a set of points
    Mesh(const std::vector<Point>& vecPt);

    // Getters and setters for the shape
    std::vector<Point> getShape() const;
    void setShape(const std::vector<Point>& vecPt);

    // Getters and setters for point indices
    std::vector<int> getPointIndices() const;
    void setPointIndices(int iNumPoints);

    // Getters and setters for triangle indices
    std::vector<int> getTriangleIndices() const;
    void setTriangleIndices(int iNumPoints);

    // Function to create a super triangle that encloses all points
    Triangle superTriangle();

    // Function to build the mesh from points and triangles
    void buildMesh();

    // Function to create new triangles
    void createTriangles(int iIndex, Triangle triContainingTriangle);

    // Function to find the triangle that contains a specific point
    Triangle findContainingTriangle(Point& ptTargetPoint);

    // Function to check if a point is inside neighboring triangles' circumcircles
    void checkNeighboringCircumcircles(Point& ptTargetPoint);

    // Function to update neighboring relationships between triangles
    void updateNeighbors();

    // Function to print the mesh for debugging purposes
    void printMesh();
};
#endif // MESH_H
