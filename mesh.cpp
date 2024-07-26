#include "mesh.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <stack>
#include <queue>

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

    int iPointIndex = 3;
    for (auto it = vecPtShape.begin() + 3; it != vecPtShape.end(); ++it)
    {
        const auto& p = *it;
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
    Triangle& triCurrent = vecTriangles[iTriangleIndex]; // 2

    if (triCurrent.onEdge(ptTargetPoint) < 0)
    {

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

        int oldNeighbourIndex1 = triCurrent.getNeighbourIndex(0);
        int oldNeighbourIndex2 = triCurrent.getNeighbourIndex(1);

        triCurrent.setNeighbourIndex(0, newIndex1);
        triCurrent.setNeighbourIndex(1, newIndex2);

        // Update the neighboring relationships with the old neighbor
        if (oldNeighbourIndex1 != -1)
        {
            Triangle& oldNeighbour = vecTriangles[oldNeighbourIndex1];

            for (int i = 0; i < 3; ++i)
            {
                if (oldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    oldNeighbour.setNeighbourIndex(i, newIndex1);
                    break;
                }
            }

            triNewTriangle1.setNeighbourIndex(0, oldNeighbourIndex1);
        }


        // Update the neighboring relationships with the old neighbor
        if (oldNeighbourIndex2 != -1)
        {
            Triangle& oldNeighbour = vecTriangles[oldNeighbourIndex2];

            for (int i = 0; i < 3; ++i)
            {
                if (oldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    oldNeighbour.setNeighbourIndex(i, newIndex2);
                    break;
                }
            }

            triNewTriangle2.setNeighbourIndex(0, oldNeighbourIndex2);
        }

        // Add the new triangles to the vector
        vecTriangles.push_back(triNewTriangle1);
        vecTriangles.push_back(triNewTriangle2);

        // Keep track of old neighbours checkNeighbourCircumcircles function

        const Triangle& triFirst = vecTriangles[iTriangleIndex];
        const Triangle& triNeighbour0 = vecTriangles[triFirst.getNeighbourIndex(2)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];
        const Triangle& triNeighbour2 = vecTriangles[triNewTriangle2.getNeighbourIndex(0)];

        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

            swapAll(neighbourQueue, iPointIndex);

        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {

            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex1,iPointIndex, 0);

            swapAll(neighbourQueue, iPointIndex);
        }

        if (triNeighbour2.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex2, iPointIndex, 0);
            swapAll(neighbourQueue, iPointIndex);
        }
    }

    else
    {
        // Triangle gets split in half
        Triangle triNewTriangle1;
        int newIndex1 = vecTriangles.size();
        triNewTriangle1.setIndex(newIndex1);

        if(triCurrent.onEdge(ptTargetPoint) == 0)
        {
            triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
            triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
            triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            triNewTriangle1.setNeighbourIndex(1, iTriangleIndex);
            triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));


            int oldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);

            if (oldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = vecTriangles[oldNeighbourIndex1];

                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                    {
                        triOldNeighbour.setNeighbourIndex(i, newIndex1);
                        break;
                    }
                }
            }

            triCurrent.setPointIndex(1, iPointIndex);
            triCurrent.setPoint(1, ptTargetPoint);
            triCurrent.setNeighbourIndex(1, newIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(0);
            triCurrent.setNeighbourIndex(0, -1);

            // Set indices for the new triangle
            vecTriangles.push_back(triNewTriangle1);

            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[newIndex1];


            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

                swapAll(neighbourQueue, iPointIndex);

            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex1,iPointIndex, 0);

                swapAll(neighbourQueue, iPointIndex);
            }

            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, newIndex1);
            }
        }

        else if (triCurrent.onEdge(ptTargetPoint) == 1)
        {
            triNewTriangle1.setPoint(0, triCurrent.getPoint(0));
            triNewTriangle1.setPoint(1, triCurrent.getPoint(1));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(0));
            triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(1));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(0));
            triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

            int oldNeighbourIndex1 = triCurrent.getNeighbourIndex(0);

            if (oldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = vecTriangles[oldNeighbourIndex1];

                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                    {
                        triOldNeighbour.setNeighbourIndex(i, newIndex1);
                        break;
                    }
                }
            }


            triCurrent.setPointIndex(1, iPointIndex);
            triCurrent.setPoint(1, ptTargetPoint);

            triCurrent.setNeighbourIndex(0, newIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(1);
            triCurrent.setNeighbourIndex(1, -1);

            // Set indices for the new triangle
            vecTriangles.push_back(triNewTriangle1);

            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, newIndex1);
            }

            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[newIndex1];

            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

                swapAll(neighbourQueue, iPointIndex);

            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex1,iPointIndex, 0);

                swapAll(neighbourQueue, iPointIndex);
            }

        }

        else
        {
            triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
            triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
            triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            triNewTriangle1.setNeighbourIndex(0, iTriangleIndex);
            triNewTriangle1.setNeighbourIndex(1, triCurrent.getNeighbourIndex(1));

            int oldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);

            if (oldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = vecTriangles[oldNeighbourIndex1];

                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                    {
                        triOldNeighbour.setNeighbourIndex(i, newIndex1);
                        break;
                    }
                }
            }

            triCurrent.setPointIndex(2, iPointIndex);
            triCurrent.setPoint(2, ptTargetPoint);

            triCurrent.setNeighbourIndex(1, newIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(2);
            triCurrent.setNeighbourIndex(2, -1);

            // Set indices for the new triangle
            vecTriangles.push_back(triNewTriangle1);

            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, newIndex1);
            }

            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[newIndex1];

            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(0)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(1)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 0);

                swapAll(neighbourQueue, iPointIndex);

            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex1,iPointIndex, 1);

                swapAll(neighbourQueue, iPointIndex);
            }

        }
    }
}

void Mesh::createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{

    const Point& ptTargetPoint = vecPtShape[iPointIndex];
    Triangle& triCurrent = vecTriangles[iTriangleIndex];

    Triangle triNewTriangle1;
    int newIndex1 = vecTriangles.size();
    triNewTriangle1.setIndex(newIndex1);

    if(triCurrent.onEdge(ptTargetPoint) == 0)
    {
        triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
        triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
        triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        triNewTriangle1.setNeighbourIndex(1, iTriangleIndex);
        triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));
        triNewTriangle1.setNeighbourIndex(2, iNeighbourIndex1);

        int oldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);

        if (oldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[oldNeighbourIndex1];

            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, newIndex1);
                    break;
                }
            }
        }

        triCurrent.setPointIndex(1, iPointIndex);
        triCurrent.setPoint(1, ptTargetPoint);
        triCurrent.setNeighbourIndex(1, newIndex1);

        const int iOldNeighbour = triCurrent.getNeighbourIndex(0);
        triCurrent.setNeighbourIndex(0, -1);

        Triangle& triOldNeighbour0 = vecTriangles[iNeighbourIndex0];
        Triangle& triOldNeighbour1 = vecTriangles[iNeighbourIndex1];

        for (int i = 0; i < 3; ++i)
        {
            if (triOldNeighbour0.getNeighbourIndex(i) == -1)
                triOldNeighbour0.setNeighbourIndex(i, newIndex1);

            if (triOldNeighbour1.getNeighbourIndex(i) == -1)
                triOldNeighbour1.setNeighbourIndex(i, iTriangleIndex);
        }

        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

            swapAll(neighbourQueue, iPointIndex);

        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {

            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex1,iPointIndex, 0);

            swapAll(neighbourQueue, iPointIndex);
        }

    }

    else if (triCurrent.onEdge(ptTargetPoint) == 1)
    {
        triNewTriangle1.setPoint(0, triCurrent.getPoint(0));
        triNewTriangle1.setPoint(1, triCurrent.getPoint(1));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(0));
        triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(1));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(0));
        triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex0);
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

        int oldNeighbourIndex1 = triCurrent.getNeighbourIndex(0);

        if (oldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[oldNeighbourIndex1];

            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, newIndex1);
                    break;
                }
            }
        }

        triCurrent.setPointIndex(1, iPointIndex);
        triCurrent.setPoint(1, ptTargetPoint);

        triCurrent.setNeighbourIndex(0, newIndex1);

        triCurrent.setNeighbourIndex(1, iNeighbourIndex1);


        Triangle& triOldNeighbour0 = vecTriangles[iNeighbourIndex0];
        Triangle& triOldNeighbour1 = vecTriangles[iNeighbourIndex1];

        for (int i = 0; i < 3; ++i)
        {
            if (triOldNeighbour0.getNeighbourIndex(i) == -1)
                triOldNeighbour0.setNeighbourIndex(i, newIndex1);

            if (triOldNeighbour1.getNeighbourIndex(i) == -1)
                triOldNeighbour1.setNeighbourIndex(i, iTriangleIndex);
        }


        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

            swapAll(neighbourQueue, iPointIndex);

        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {

            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex1,iPointIndex, 0);

            swapAll(neighbourQueue, iPointIndex);
        }
    }

    else
    {
        triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
        triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
        triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        triNewTriangle1.setNeighbourIndex(0, iTriangleIndex);
        triNewTriangle1.setNeighbourIndex(1, triCurrent.getNeighbourIndex(1));
        triNewTriangle1.setNeighbourIndex(2, iNeighbourIndex1);

        int oldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);

        if (oldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[oldNeighbourIndex1];

            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, newIndex1);
                    break;
                }
            }
        }

        triCurrent.setPointIndex(2, iPointIndex);
        triCurrent.setPoint(2, ptTargetPoint);

        triCurrent.setNeighbourIndex(1, newIndex1);
        triCurrent.setNeighbourIndex(2, iNeighbourIndex0);


        Triangle& triOldNeighbour0 = vecTriangles[iNeighbourIndex0];
        Triangle& triOldNeighbour1 = vecTriangles[iNeighbourIndex1];

        for (int i = 0; i < 3; ++i)
        {
            if (triOldNeighbour0.getNeighbourIndex(i) == -1)
                triOldNeighbour0.setNeighbourIndex(i, newIndex1);

            if (triOldNeighbour1.getNeighbourIndex(i) == -1)
                triOldNeighbour1.setNeighbourIndex(i, iTriangleIndex);
        }

        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(0)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(1)];

        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 0);

            swapAll(neighbourQueue, iPointIndex);

        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {

            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(newIndex1,iPointIndex, 1);

            swapAll(neighbourQueue, iPointIndex);
        }
    }

    // Set indices for the new triangle
    vecTriangles.push_back(triNewTriangle1);
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

    // If no contaiCurrentNeighbouring triangle is found, return -1 or handle error appropriately
    return -1;
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


void Mesh::swapAll(std::queue<int>& neighbourQueue, int iPointIndex)
{
    Point& ptTargetPoint = vecPtShape[iPointIndex];
    // Stack contains all triangles that have ptTargetPoint within their circumcircles
    while (neighbourQueue.size() > 1)
    {
        neighbourQueue.pop();
        int iNeighbourIndex = neighbourQueue.front();
        Triangle& triNeighbour = vecTriangles[iNeighbourIndex];

        int iTriangleIndex = triNeighbour.findPathToContainingTriangle(ptTargetPoint);

        if (iNeighbourIndex >= 0)
        {
            swapEdge(iTriangleIndex, iNeighbourIndex);
        }
    }
}

std::queue<int> Mesh::checkNeighboringCircumcircles(int iTriangleIndex, int iPointIndex, int iEdgeIndex)
{
    std::queue<int> neighbourQueue;
    neighbourQueue.push(iTriangleIndex);


    std::vector<bool> visited(vecTriangles.size(), false);

    const Point& ptTargetPoint = vecPtShape[iPointIndex];
    size_t iQueueIndex = 0;
    bool firstIteration = true;

    // Traverse the triangles
    while (iQueueIndex < neighbourQueue.size())
    {
        // Get the index of the current triangle
        int iCurrentIndex = neighbourQueue.back();

        if (!visited[iCurrentIndex])
        {
            visited[iCurrentIndex] = true; // Mark as visited
            Triangle& triCurrent = vecTriangles[iCurrentIndex];

            if (firstIteration)
            {
                // For the very first triangle, check the specified edge
                int iNeighbourIndex = triCurrent.getNeighbourIndex(iEdgeIndex);

                // Ensure the neighbor index is valid
                if (iNeighbourIndex >= 0 && !visited[iNeighbourIndex])
                {
                    Triangle& triNeighbour = vecTriangles[iNeighbourIndex];

                    if (triNeighbour.isInCircumcircle(ptTargetPoint))
                    {
                        neighbourQueue.push(iNeighbourIndex);
                    }
                }
                firstIteration = false;
            }
            else
            {
                // For subsequent triangles, check all three edges
                for (int i = 0; i < 3; ++i)
                {
                    int iNeighbourIndex = triCurrent.getNeighbourIndex(i);

                    // Ensure the neighbor index is valid
                    if (iNeighbourIndex >= 0 && !visited[iNeighbourIndex])
                    {
                        Triangle& triNeighbour = vecTriangles[iNeighbourIndex];

                        if (triNeighbour.isInCircumcircle(ptTargetPoint))
                        {
                            neighbourQueue.push(iNeighbourIndex);
                        }
                    }
                }
            }
        }
        iQueueIndex++;
    }

    return neighbourQueue; // Return the queue containing all neighbors to be processed
}
