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

    // Vector of points that will aid maintain the triangles to adhere to the Delaunay Criterion
    std::vector<Point> vecPtInternal;

public:
    // Constructor to initialize mesh with a set of points
    Mesh(const std::vector<Point>& vecPt);

    // Getters and setters for the shape
    std::vector<Point> getShape() const;
    void setShape(const std::vector<Point>& vecPt);

    // Getters and setters for the vector of Triangles (for testing purposes only)
    void setTriVector(const std::vector<Triangle>& vecTri);
    std::vector<Triangle> getTriVector() const;

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

    void removeHelperTriangles();

    void updateTriangleIndicesAfterRemoval();

    void updateRemovedNeighbours(int iRemovedTriangleIndex);

    // Function to create new triangles
    void createTriangles(int iTriangleIndex, int iPointIndex);

    //Function to handle edge case
    void handleEdgeCase(int iTriangleIndex, int iPointIndex);

    void updateEdgeNeighbours(int iTriangleIndex, int iNewTriangleIndex, int iNeighbourIndex0, int iNeighbourIndex1);

    // Function to finds the index the triangle that contains a specific point
    int findContainingTriangle(const Point& ptTargetPoint) const;

    // Function to swap edge between neighbouring triangles
    void swapEdge(int iTri1, int iTri2);

    bool bAreNeighbours(int iTri1, int iTri2);

    int findSharedEdge(const Triangle& tri, int iDiff1, int iDiff2) const;

    int findNewEdge(const Triangle& tri, int i, int iSharedEdge) const;

    int findStolenEdge(const Triangle& tri, int iSharedPoint, int iDiff1) const;

    void swapAll(std::queue<int>& neighbourQueue, int iPointIndex);

    // Function to update neighboring relationships between triangles
    void updateNeighbours(int oldNeighborIndex, int oldTriangleIndex, int newTriangleIndex);

    // Function to check if a point is inside neighboring triangles' circumcircles
    std::queue<int> checkNeighboringCircumcircles(int iTriangleIndex, int iPointIndex, int iEdgeIndex);

    // Function to print the mesh for debugging purposes
    void printMesh() const;

    void createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1);

    bool isNearEquilateral(int iTriangleIndex);

    void equilateralizeTriangles();

    int locateSmallestAngle();

};
#endif // MESH_H
