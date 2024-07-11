#include "mesh.h"
#include <iostream>

Mesh::Mesh(std::vector<Point> points): vecPoints(points)
{
    // Create the super triangle and initialize the triangles list
    Triangle initialTriangle = superTriangle();
    int numPoints = vecPoints.size() + 3;
    setPointIndices(numPoints);
    setTriangleIndices(numPoints);
}



std::vector<int> Mesh::getPointIndices() {
    return pointIndices;
}

void Mesh::setPointIndices(int numPoints) {
    pointIndices.resize(numPoints);
    for (int i = 3; i < numPoints; ++i) {
        pointIndices[i] = i;
    }
}

std::vector<int> Mesh::getTriangleIndices() {
    return triangleIndices;
}

void Mesh::setTriangleIndices(int numPoints) {

    int numTriangles = 2 * numPoints + 1;
    triangleIndices.resize(numTriangles);
    for (int i = 0; i < numTriangles; ++i) {
        triangleIndices[i] = i;
    }
}

Triangle Mesh::superTriangle() {
    Point p0(0, 0);
    Point p1(30, 0);
    Point p2(15, 25.981);

    Triangle triSuper = Triangle(p0, p1, p2);
    triSuper.setPointIndex(0, 0);
    triSuper.setPointIndex(1, 1);
    triSuper.setPointIndex(2, 2);

    return triSuper;
}

/*
void Mesh::buildMesh() {
    std::vector<Triangle> vecNewTriangles;

    for (auto& p : vecPoints) {
        vecNewTriangles.clear();
        Triangle containingTriangle = findContainingTriangle(p);
        //if (containingTriangle)

        createTriangles(p, containingTriangle);
        checkNeighboringCircumcircles(p);

    }
}

Triangle Mesh::findContainingTriangle(Point& p) {
    // Implement
}


void Mesh::createTriangles(int pIndex, Triangle containingTriangle) {


    // Create new triangles by connecting the point to the vertices of the containing triangle

    Triangle t2(containingTriangle.getPoint(1), containingTriangle.getPoint(3), p);
    Triangle t3(containingTriangle.getPoint(2), containingTriangle.getPoint(3), p);

    // Update the containing triangles points
    containingTriangle.setPointIndex(2, pIndex);

    vecTriangles.push_back(t1);
    vecTriangles.push_back(t2);
    vecTriangles.push_back(t3);

    // Update neighbors for new triangles

    updateNeighbors();

    // Remove the old containing triangle

    // Implement

}

void Mesh::checkNeighboringCircumcircles( Point& p) {
    // Implement
}

void Mesh::updateNeighbors() {
    // Implement
}

void Mesh::printMesh()  {
    for (auto& triangle : vecTriangles) {
        triangle.printPoints();
    }
}
*/
