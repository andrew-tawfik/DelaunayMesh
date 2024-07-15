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

    return triSuper;
}

// Builds mesh from points and triangles
void Mesh::buildMesh()
{
    Triangle triSuper = superTriangle();
    vecTriangles.push_back(triSuper);


    for (const auto& p : vecPtShape)
    {

        int iTriIndex = findContainingTriangle(p);
        createTriangles(iTriIndex);
    }
}

// Create new triangles
void Mesh::createTriangles(int iIndex)
{}

int Mesh::findContainingTriangle(const Point& ptTargetPoint) const
{
    // Initialize random number generator
    static std::random_device rd;  // Seed
    static std::mt19937 gen(rd()); // Mersenne Twister RNG

    int iMaxIndex = vecTriangles.size() - 1;
    // Size of vecTriangles should change everytime method is called
    std::uniform_int_distribution<> dis(0, iMaxIndex);

    // Get a randomized triangle from vecTriangles
    int randomIndex = dis(gen);

    std::stack<int> stack;
    stack.push(randomIndex);

    while (!stack.empty())
    {
        int currentIndex = stack.top();
        stack.pop();

        const Triangle& currentTri = vecTriangles[currentIndex];
        int result = currentTri.findPathToContainingTriangle(ptTargetPoint);

        if (result == -1) // -1 indicates that the currentTri contains ptTargetPoint
        {
            return currentTri.getIndex();
        }
        else
        {
            stack.push(result);
        }
    }

    // If no containing triangle is found, return -1 or handle error appropriately
    return -1;
}


