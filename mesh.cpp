#include "mesh.h"
#include <iostream>
#include <random>
#include <stack>

// Constructor: initializes the mesh with a given set of points
Mesh::Mesh(const std::vector<Point>& vecPt)
{
    setShape(vecPt);
    setPointIndices(vecPt.size());
    setTriangleIndices(vecPt.size());
}

// Sets the shape of the mesh with a given vector of points
void Mesh::setShape(const std::vector<Point>& vecPt)
{
    vecPtShape = vecPt;
}

// Returns the shape of the mesh as a vector of points
std::vector<Point> Mesh::getShape() const
{
    return vecPtShape;
}

// Sets the shape of the mesh with a given vector of points
void Mesh::setTriVector(const std::vector<Triangle>& vecTri)
{
    vecTriangles = vecTri;
}

// Returns the shape of the mesh as a vector of points
std::vector<Triangle> Mesh::getTriVector() const
{
    return vecTriangles;
}

// Returns the indices of points in the mesh
std::vector<int> Mesh::getPointIndices() const
{
    return viPointIndices;
}

// Sets the indices of points in the mesh, adding 3 for the super triangle
void Mesh::setPointIndices(int iNumPoints)
{
    iNumPoints += 3;
    viPointIndices.resize(iNumPoints);
    for (int i = 0; i < iNumPoints; ++i)
    {
        viPointIndices[i] = i;
    }
}

// Returns the indices of triangles in the mesh
std::vector<int> Mesh::getTriangleIndices() const
{
    return viTriangleIndices;
}

// Sets the indices of triangles in the mesh, calculating the number based on points
void Mesh::setTriangleIndices(int numPoints)
{
    int iNumTriangles = 2 * numPoints + 1;
    viTriangleIndices.resize(iNumTriangles);
    for (int i = 0; i < iNumTriangles; ++i)
    {
        viTriangleIndices[i] = i;
    }
}

// Creates a super triangle that encloses all points in the mesh
Triangle Mesh::superTriangle()
{
    Point p0(0, 0);
    Point p1(30, 0);
    Point p2(15, 25.981);

    Triangle triSuper = Triangle(p0, p1, p2);
    triSuper.setPointIndex(0, 0);
    triSuper.setPointIndex(1, 1);
    triSuper.setPointIndex(2, 2);

    for (int i = 0; i < 3; ++i)
    {
        viPointIndices[i] = i;
    }

    triSuper.setIndex(0);

    return triSuper;
}

// Builds mesh from points and triangles
void Mesh::buildMesh()
{
    Triangle triSuper = superTriangle();
    vecTriangles.push_back(triSuper);


    int iPointIndex = 0;
    for (const auto& p : vecPtShape)
    {

        int iTriIndex = findContainingTriangle(p);
        createTriangles(iTriIndex, iPointIndex);

        iPointIndex += 1;
    }
}

void Mesh::createTriangles(int iTriangleIndex, int iPointIndex)
{
    // Ensure indices are valid
    if (iTriangleIndex < 0 || iTriangleIndex >= vecTriangles.size() || iPointIndex < 0 || iPointIndex >= vecPtShape.size()) {
        std::cerr << "Invalid indices provided to createTriangles." << std::endl;
        return;
    }

    // Get references to the target point and current triangle
    const Point& ptTargetPoint = vecPtShape[iPointIndex];
    Triangle& triCurrent = vecTriangles[iTriangleIndex];

    // Create two new triangles
    Triangle triNewTriangle1(triCurrent.getPoint(0), triCurrent.getPoint(1), ptTargetPoint);
    Triangle triNewTriangle2(triCurrent.getPoint(1), triCurrent.getPoint(2), ptTargetPoint);

    // Set indices for the new triangles
    int newIndex1 = vecTriangles.size(); //  3
    int newIndex2 = newIndex1 + 1;       //  4
    triNewTriangle1.setIndex(newIndex1);
    triNewTriangle2.setIndex(newIndex2);

    // Set point indices for the new triangles
    triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(0));
    triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(1));
    triNewTriangle1.setPointIndex(2, iPointIndex);

    triNewTriangle2.setPointIndex(0, triCurrent.getPointIndex(1));
    triNewTriangle2.setPointIndex(1, triCurrent.getPointIndex(2));
    triNewTriangle2.setPointIndex(2, iPointIndex);

    // Update the current triangle with the new point and point index
    triCurrent.setPointIndex(1, iPointIndex);
    triCurrent.setPoint(1, ptTargetPoint);

    // Update neighbors for the new triangles
    triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);
    triNewTriangle2.setNeighbourIndex(1, iTriangleIndex);

    // Update neighbors for the current triangle

    triNewTriangle1.setNeighbourIndex(1, newIndex2);
    triNewTriangle2.setNeighbourIndex(2, newIndex1);

    int oldNeighbourIndex = triCurrent.getNeighbourIndex(1);

    triCurrent.setNeighbourIndex(0, newIndex1);
    triCurrent.setNeighbourIndex(1, newIndex2);


    // Update the neighboring relationships with the old neighbor
    if (oldNeighbourIndex != -1)
    {
        Triangle& oldNeighbour = vecTriangles[oldNeighbourIndex];

        for (int i = 0; i < 3; ++i)
        {
            if (oldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
            {
                oldNeighbour.setNeighbourIndex(i, newIndex2);
                break;
            }
        }

        triNewTriangle2.setNeighbourIndex(1, oldNeighbourIndex);
        triNewTriangle2.setNeighbourIndex(0, iTriangleIndex);
    }

    // Add the new triangles to the vector
    vecTriangles.push_back(triNewTriangle1);
    vecTriangles.push_back(triNewTriangle2);
}

// Finds the index of the triangle containing the target point
int Mesh::findContainingTriangle(const Point& ptTargetPoint) const
{
    // Initialize random number generator
    static std::random_device rd;  // Seed
    static std::mt19937 gen(rd()); // Mersenne Twister RNG


    // Size of vecTriangles should change everytime method is called
    std::uniform_int_distribution<> dis(0, vecTriangles.size() - 1);

    // Get a randomized triangle from vecTriangles
    int iRandomIndex = dis(gen);

    // Initialize stack for dfs
    std::stack<int> stack;
    stack.push(iRandomIndex);

    while (!stack.empty())
    {
        int iCurrentIndex = stack.top();
        stack.pop();

        const Triangle& triCurrent = vecTriangles[iCurrentIndex];
        int iResult = triCurrent.findPathToContainingTriangle(ptTargetPoint);

        if (iResult == -1) // -1 indicates that the currentTri contains ptTargetPoint
        {
            return triCurrent.getIndex();
        }
        else
        {
            stack.push(iResult);
        }
    }

    // If no containing triangle is found, return -1 or handle error appropriately
    return -1;
}


