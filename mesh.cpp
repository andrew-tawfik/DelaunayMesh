#include "mesh.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <stack>

// Constructor: iCurrentNeighbourtializes the mesh with a given set of points
Mesh::Mesh(const std::vector<Point>& vecPt)
{
    setTriVector({});
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

    // Insert points at the beginning of vecPtShape
    vecPtShape.insert(vecPtShape.begin(), {p0, p1, p2});

    triSuper.setIndex(0);

    return triSuper;
}

// Builds mesh from points and triangles
void Mesh::buildMesh()
{

    // Triangle triSuper = superTriangle();
    // vecTriangles.push_back(triSuper);

    std::cout << "Size of triVector: " << vecTriangles.size() <<  std::endl;
    std::cout << "Size of ptVector : " << vecPtShape.size() <<  std::endl;

    int iPointIndex = 0;
    for (const auto& p : vecPtShape)
    {

        int iTriIndex = findContainingTriangle(p);
        createTriangles(iTriIndex, iPointIndex);

        iPointIndex += 1;
        std::cout << "Size of triVector after loop: " << iPointIndex << ": " << vecTriangles.size() << std::endl;
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
    Triangle& triCurrent = vecTriangles[iTriangleIndex]; // 2

    triCurrent.printPoints();
    // Create two new triangles
    Triangle triNewTriangle1(triCurrent.getPoint(0), triCurrent.getPoint(1), ptTargetPoint); // 3
    Triangle triNewTriangle2(triCurrent.getPoint(1), triCurrent.getPoint(2), ptTargetPoint); // 4

    // Set indices for the new triangles
    int newIndex1 = vecTriangles.size();
    int newIndex2 = newIndex1 + 1;
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

        triNewTriangle2.setNeighbourIndex(0, oldNeighbourIndex);
        triNewTriangle2.setNeighbourIndex(1, iTriangleIndex);
    }

    // Add the new triangles to the vector
    vecTriangles.push_back(triNewTriangle1);
    vecTriangles.push_back(triNewTriangle2);

    // Keep track of old neighbours checkNeighbourCircumcircles function
/*
    const Triangle& triFirst = vecTriangles[iTriangleIndex];
    const Triangle& triNeighbour0 = vecTriangles[triFirst.getNeighbourIndex(2)];
    const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];
    const Triangle& triNeighbour2 = vecTriangles[triNewTriangle2.getNeighbourIndex(0)];


    if (triNeighbour0.isInCircumcircle(ptTargetPoint))
    {
        checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

    }

    if (triNeighbour1.isInCircumcircle(ptTargetPoint))
    {
        checkNeighboringCircumcircles(newIndex1,iPointIndex, 0);
    }

    if (triNeighbour2.isInCircumcircle(ptTargetPoint))
        checkNeighboringCircumcircles(newIndex2, iPointIndex, 0);

*/
}

// Finds the index of the triangle contaiCurrentNeighbourng the target point
int Mesh::findContainingTriangle(const Point& ptTargetPoint) const
{
    // iCurrentNeighbourtialize random number generator
    static std::random_device rd;  // Seed
    static std::mt19937 gen(rd()); // Mersenne Twister RNG


    // Size of vecTriangles should change everytime method is called
    std::uniform_int_distribution<> dis(0, vecTriangles.size() - 1);

    // Get a randomized triangle from vecTriangles
    int iRandomIndex = dis(gen);

    // iCurrentNeighbourtialize stack for dfs
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

    // If no contaiCurrentNeighbourng triangle is found, return -1 or handle error appropriately
    return -1;
}


void Mesh::checkNeighboringCircumcircles(int iTriangleIndex, int iPointIndex, int iSide)
{
    // Ensure indices are valid
    if (iTriangleIndex < 0 || iTriangleIndex >= vecTriangles.size() || iPointIndex < 0 || iPointIndex >= vecPtShape.size()) {
        std::cerr << "Invalid indices provided to checkNeighboringCircumcircles." << std::endl;
        return;
    }

    // Get references to the target point and current triangle
    const Point& ptTargetPoint = vecPtShape[iPointIndex];
    Triangle& triCurrent = vecTriangles[iTriangleIndex];

    int iNeighbourIndex = triCurrent.getNeighbourIndex(iSide);

    // Ensure the neighbor index is valid
    if (iNeighbourIndex < 0 || iNeighbourIndex >= vecTriangles.size())
    {
        std::cerr << "Invalid neighbor index." << std::endl;
        return;
    }

    Triangle& triNeighbour = vecTriangles[iNeighbourIndex];

    if (triNeighbour.isInCircumcircle(ptTargetPoint))
    {
        swapEdge(iTriangleIndex, iNeighbourIndex);
    }
}


void Mesh::swapEdge(int iTri1, int iTri2)
{
    Triangle& triCurrent = vecTriangles[iTri1];
    Triangle& triNeighbour = vecTriangles[iTri2];

    int diff1 = -1, diff2 = -1;
    std::vector<int> shared;

    // Identify shared points
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int pointCurrent = triCurrent.getPointIndex(i);
            int pointNeighbour = triNeighbour.getPointIndex(j);

            if (pointCurrent == pointNeighbour)
            {
                shared.push_back(pointCurrent);
            }
        }
    }

    // Identify different points
    for (int i = 0; i < 3; ++i)
    {
        int pointCurrent = triCurrent.getPointIndex(i);
        int pointNeighbour = triNeighbour.getPointIndex(i);

        if (std::find(shared.begin(), shared.end(), pointCurrent) == shared.end())
        {
            diff1 = pointCurrent;
        }

        if (std::find(shared.begin(), shared.end(), pointNeighbour) == shared.end())
        {
            diff2 = pointNeighbour;
        }
    }

    std::cout << "Shared points: ";
    for (int sp : shared)
    {
        std::cout << sp << " ";
    }
    std::cout << std::endl;

    std::cout << "Diff points: " << diff1 << " " << diff2 << std::endl;

    // Log before changing
    std::cout << "\nBefore swap:" << std::endl;
    triCurrent.printPoints();
    triNeighbour.printPoints();

    // Replace one of the shared points in triCurrent with diff2
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "the triCurrent at index " << i << " is: " << triCurrent.getPointIndex(i) << std::endl;
        if (triCurrent.getPointIndex(i) == shared[0])
        {
            triCurrent.setPointIndex(i, diff2);
            triCurrent.setPoint(i, vecPtShape[diff2]);
            break;
        }
    }

    // Replace one of the shared points in triNeighbour with diff1
    for (int i = 0; i < 3; ++i)
    {
        if (triNeighbour.getPointIndex(i) == shared[1])
        {
            triNeighbour.setPointIndex(i, diff1);
            triNeighbour.setPoint(i, vecPtShape[diff1]);
            break;
        }
    }

    // Log after changing
    std::cout << "\nAfter swap:" << std::endl;
    triCurrent.printPoints();
    triNeighbour.printPoints();

    updateNeighbours(iTri1, iTri2);
}

void Mesh::updateNeighbours(int iTri1, int iTri2)
{
    Triangle& triCurrent = vecTriangles[iTri1];
    Triangle& triNeighbour = vecTriangles[iTri2];

    int oldNeighboursCurrent[3] = { triCurrent.getNeighbourIndex(0), triCurrent.getNeighbourIndex(1), triCurrent.getNeighbourIndex(2) };
    int oldNeighboursNeighbour[3] = { triNeighbour.getNeighbourIndex(0), triNeighbour.getNeighbourIndex(1), triNeighbour.getNeighbourIndex(2) };

    // Update neighbors for triCurrent
    for (int i = 0; i < 3; ++i)
    {
        if (oldNeighboursCurrent[i] == iTri2)
        {
            triCurrent.setNeighbourIndex(i, -1);
        }
    }

    // Update neighbors for triNeighbour
    for (int i = 0; i < 3; ++i)
    {
        if (oldNeighboursNeighbour[i] == iTri1)
        {
            triNeighbour.setNeighbourIndex(i, -1);
        }
    }

    // Update neighboring relationships
    for (int i = 0; i < 3; ++i)
    {
        if (oldNeighboursCurrent[i] != -1 && oldNeighboursCurrent[i] != iTri2)
        {
            Triangle& oldNeighbour = vecTriangles[oldNeighboursCurrent[i]];
            for (int j = 0; j < 3; ++j)
            {
                if (oldNeighbour.getNeighbourIndex(j) == iTri1)
                {
                    oldNeighbour.setNeighbourIndex(j, iTri2);
                    break;
                }
            }
        }

        if (oldNeighboursNeighbour[i] != -1 && oldNeighboursNeighbour[i] != iTri1)
        {
            Triangle& oldNeighbour = vecTriangles[oldNeighboursNeighbour[i]];
            for (int j = 0; j < 3; ++j)
            {
                if (oldNeighbour.getNeighbourIndex(j) == iTri2)
                {
                    oldNeighbour.setNeighbourIndex(j, iTri1);
                    break;
                }
            }
        }
    }

    // Set new neighbors
    triCurrent.setNeighbourIndex(std::find(std::begin(oldNeighboursCurrent), std::end(oldNeighboursCurrent), iTri2) - std::begin(oldNeighboursCurrent), iTri2);
    triNeighbour.setNeighbourIndex(std::find(std::begin(oldNeighboursNeighbour), std::end(oldNeighboursNeighbour), iTri1) - std::begin(oldNeighboursNeighbour), iTri1);
}
