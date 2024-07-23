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

    const int iCurrentN0 = triCurrent.getNeighbourIndex(0);
    const int iCurrentN1 = triCurrent.getNeighbourIndex(1);
    const int iCurrentN2 = triCurrent.getNeighbourIndex(2);

    const int iNeighbourN0 = triNeighbour.getNeighbourIndex(0);
    const int iNeighbourN1 = triNeighbour.getNeighbourIndex(1);
    const int iNeighbourN2 = triNeighbour.getNeighbourIndex(2);

    const int iCurrentPt0 = triCurrent.getPointIndex(0);
    const int iCurrentPt1 = triCurrent.getPointIndex(1);
    const int iCurrentPt2 = triCurrent.getPointIndex(2);

    // Replace one of the shared points in triCurrent with diff2
    for (int i = 0; i < 3; ++i)
    {
        if (triCurrent.getPointIndex(i) == shared[0])
        {
            triCurrent.setPointIndex(i, diff2);
            triCurrent.setPoint(i, vecPtShape[diff2]);

            // Determine the shared edge and new edge after swap
            int sharedEdge = -1, newEdge = -1;
            for (int j = 0; j < 3; ++j)
            {
                if (((triCurrent.getPointIndex(j) == diff1 && triCurrent.getPointIndex((j + 1) % 3) == diff2)) || ((triCurrent.getPointIndex(j) == diff2 && triCurrent.getPointIndex((j + 1) % 3) == diff1)))
                {
                    sharedEdge = j;
                }
            }

            if (i == 0)
            {
                if (sharedEdge == 0)
                {
                    newEdge = 2;
                }
                else if (sharedEdge == 2)
                {
                    newEdge = 0;
                }
            }

            else if (i == 1)
            {
                if (sharedEdge == 0)
                {
                    newEdge = 1;
                }
                else if (sharedEdge == 1)
                {
                    newEdge = 0;
                }
            }

            else if (i == 2)
            {
                if (sharedEdge == 1)
                {
                    newEdge = 2;
                }
                else if (sharedEdge == 2)
                {
                    newEdge = 1;
                }
            }

            // Find which neighbor from triNeighbour was stolen
            int stolenEdge = -1;
            if ((triNeighbour.getPointIndex(0) == shared[1] && triNeighbour.getPointIndex(1) == diff2) || (triNeighbour.getPointIndex(1) == shared[1] && triNeighbour.getPointIndex(0) == diff2))
            {
                stolenEdge = iNeighbourN0;
            }
            else if ((triNeighbour.getPointIndex(1) == shared[1] && triNeighbour.getPointIndex(2) == diff2) || (triNeighbour.getPointIndex(2) == shared[1] && triNeighbour.getPointIndex(1) == diff2))
            {
                stolenEdge = iNeighbourN1;
            }
            else if ((triNeighbour.getPointIndex(2) == shared[1] && triNeighbour.getPointIndex(0) == diff2) || (triNeighbour.getPointIndex(0) == shared[1] && triNeighbour.getPointIndex(2) == diff2) )
            {
                stolenEdge = iNeighbourN2;
            }

            // Update the neighbor indices
            triCurrent.setNeighbourIndex(sharedEdge, iTri2);
            triCurrent.setNeighbourIndex(newEdge, stolenEdge);
            break;

            \
        }
    }

    // Replace one of the shared points in triNeighbour with diff1
    for (int i = 0; i < 3; ++i)
    {
        if (triNeighbour.getPointIndex(i) == shared[1])
        {
            triNeighbour.setPointIndex(i, diff1);
            triNeighbour.setPoint(i, vecPtShape[diff1]);

            // Determine the shared edge and new edge after swap
            int sharedEdge = -1, newEdge = -1;
            for (int j = 0; j < 3; ++j)
            {
                if ((triNeighbour.getPointIndex(j) == diff1 && triNeighbour.getPointIndex((j + 1) % 3) == diff2) || (triNeighbour.getPointIndex(j) == diff2 && triNeighbour.getPointIndex((j + 1) % 3) == diff1))
                {
                    sharedEdge = j;
                }
            }



            if (i == 0)
            {
                if (sharedEdge == 0)
                {
                    newEdge = 2;
                }
                else if (sharedEdge == 2)
                {
                    newEdge = 0;
                }
            }

            else if (i == 1)
            {
                if (sharedEdge == 0)
                {
                    newEdge = 1;
                }
                else if (sharedEdge == 1)
                {
                    newEdge = 0;
                }
            }

            else if (i == 2)
            {
                if (sharedEdge == 1)
                {
                    newEdge = 2;
                }
                else if (sharedEdge == 2)
                {
                    newEdge = 1;
                }
            }

            // Find which neighbor from triCurrent was stolen
            int stolenEdge = -1;
            if ((iCurrentPt0 == diff1 && iCurrentPt1 == shared[0]) || (iCurrentPt1 == diff1 && iCurrentPt0 == shared[0]))
            {
                stolenEdge = iCurrentN0;
            }
            else if ((iCurrentPt1 == diff1 && iCurrentPt2 == shared[0]) || (iCurrentPt2 == diff1 && iCurrentPt1 == shared[0]))
            {
                stolenEdge = iCurrentN1;
            }
            else if ((iCurrentPt2 == diff1 && iCurrentPt0 == shared[0]) || (iCurrentPt0 == diff1 && iCurrentPt2 == shared[0]))
            {
                stolenEdge = iCurrentN2;
            }

            // Update the neighbor indices
            triNeighbour.setNeighbourIndex(sharedEdge, iTri1);
            triNeighbour.setNeighbourIndex(newEdge, stolenEdge);
            break;
        }
    }


    // Update old neighbors
    updateNeighbours(iCurrentN0, iTri1, iTri2);
    updateNeighbours(iCurrentN1, iTri1, iTri2);
    updateNeighbours(iCurrentN2, iTri1, iTri2);
    updateNeighbours(iNeighbourN0, iTri2, iTri1);
    updateNeighbours(iNeighbourN1, iTri2, iTri1);
    updateNeighbours(iNeighbourN2, iTri2, iTri1);
}


void Mesh::updateNeighbours(int oldNeighborIndex, int oldTriangleIndex, int newTriangleIndex)
{
    if (oldNeighborIndex != -1 && oldNeighborIndex != oldTriangleIndex && oldNeighborIndex != newTriangleIndex)
    {
        Triangle& oldNeighbor = vecTriangles[oldNeighborIndex];
        const Triangle& triCurrent = vecTriangles[oldTriangleIndex];

        // Determine if the neighbor needs to be updated
        bool needsUpdate = true;
        for (int i = 0; i < 3; ++i)
        {
            if (triCurrent.getNeighbourIndex(i) == oldNeighborIndex)
            {
                needsUpdate = false;
                break;
            }
        }

        // Update the neighbor if needed
        if (needsUpdate)
        {
            for (int i = 0; i < 3; ++i)
            {
                if (oldNeighbor.getNeighbourIndex(i) == oldTriangleIndex)
                {
                    oldNeighbor.setNeighbourIndex(i, newTriangleIndex);
                    break;
                }
            }
        }
    }
}

