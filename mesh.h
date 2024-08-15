#ifndef MESH_H

#define MESH_H

#include "point.h"
#include "triangle.h"

#include <vector>
#include <queue>

// Class representing a 2D mesh composed of points and triangles
class Mesh {

private:

    std::vector<Point> vecPtShape;  // Vector of points defining the shape
    std::vector<Triangle> vecTriangles;  // Vector of triangles defining the shape

public:

    // Constructor to initialize mesh with a set of points
    Mesh(const std::vector<Point>& vecPt);

    // Getters and setters for the shape
    std::vector<Point> getShape() const;
    void setShape(const std::vector<Point>& vecPt);

    // Getters and setters for the vector of Triangles (for testing purposes only)
    void setTriVector(const std::vector<Triangle>& vecTri);
    std::vector<Triangle> getTriVector() const;

    // Function to build the mesh from points and triangles
    void buildMesh();

    // Function to finds the index the triangle that contains a specific point
    int findContainingTriangle(const Point& ptTargetPoint) const;

    // Function to create a super triangle that encloses all points
    Triangle superTriangle();

    // Removes helper triangles used for intermediate computations
    void removeHelperTriangles();

    // Updates triangle indices after removing triangles
    void updateTriangleIndicesAfterRemoval();

    // Updates neighboring triangles when a triangle is removed
    void updateRemovedNeighbours(int iRemovedTriangleIndex);

    // Function to create new triangles
    void createTriangles(int iTriangleIndex, int iPointIndex);

    //Function to handle edge case
    void handleEdgeCase(int iTriangleIndex, int iPointIndex);

    // Creates new triangles on the opposite side of a specified triangle and point
    void createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1);

    // Updates the neighboring triangles' edge references after swapping edges
    void updateEdgeNeighbours(int iTriangleIndex, int iNewTriangleIndex, int iNeighbourIndex0, int iNeighbourIndex1);

    // Checks if two triangles are neighbors (share an edge)
    bool areNeighbours(int iTri1, int iTri2);

    // Checks if a point is inside the circumcircles of neighboring triangles
    std::queue<int> checkNeighboringCircumcircles(int iTriangleIndex, int iPointIndex, int iEdgeIndex);

    // Function to swap edge between neighbouring triangles
    void swapEdge(int iTri1, int iTri2);

    // Performs edge swaps on all neighboring triangles containing a specific point
    void swapAll(std::queue<int>& neighbourQueue, int iPointIndex);

    // Finds the shared edge between two triangles
    int findSharedEdge(const Triangle& tri, int iDiff1, int iDiff2) const;

    // Finds the new edge index after swapping edges
    int findNewEdge(const Triangle& tri, int i, int iSharedEdge) const;

    // Updates neighboring relationships between triangles
    void updateNeighboursAfterSwap(int oldNeighborIndex, int oldTriangleIndex, int newTriangleIndex);

    // Equilateralizes triangles by adding new points to improve the mesh quality
    void equilateralizeTriangles();

    // Locates the triangle with the smallest angle
    int locateSmallestAngle();

};

#endif // MESH_H
