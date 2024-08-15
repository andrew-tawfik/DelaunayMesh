#include "mesh.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <stack>
#include <queue>

// Constructor: Creates the mesh with a given set of points
Mesh::Mesh(const std::vector<Point>& vecPt)
{
    setShape(vecPt);
}

// Returns the shape of the mesh as a vector of points
std::vector<Point> Mesh::getShape() const
{
    return vecPtShape;
}

// Sets the shape of the mesh with a given vector of points
void Mesh::setShape(const std::vector<Point>& vecPt)
{
    vecPtShape = vecPt;
}

// Returns the triangle vector
std::vector<Triangle> Mesh::getTriVector() const
{
    return vecTriangles;
}

// Sets the triangle vector
void Mesh::setTriVector(const std::vector<Triangle>& vecTri)
{
    vecTriangles = vecTri;
}

// Builds the mesh by iterating through all points in vecPtShape.
void Mesh::buildMesh()
{
    int iPointIndex = 0;

    for (auto point : vecPtShape)
    {
        // Find the triangle that contains the current point
        int iTriIndex = findContainingTriangle(point);

        // Create new triangles from the found triangle and current point
        createTriangles(iTriIndex, iPointIndex);

        iPointIndex += 1;  // Move to the next point

        // Stop processing when the last three points (super triangle points) are reached
        if ((vecPtShape.size() - 3) == iPointIndex) { break; }
    }
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

    // Initialize stack for DFS
    std::stack<int> stackTriangles;
    stackTriangles.push(iRandomIndex);

    while (!stackTriangles.empty())
    {
        int iCurrentIndex = stackTriangles.top();
        stackTriangles.pop();

        const Triangle& triCurrent = vecTriangles[iCurrentIndex];
        int iResult = triCurrent.findPathToContainingTriangle(ptTargetPoint);

        if (iResult == -1) // -1 indicates that the currentTri contains ptTargetPoint
        {
            return triCurrent.getIndex();
        }
        else
        {
            stackTriangles.push(iResult);
        }
    }

    // If no contaiCurrentNeighbouring triangle is found, return -1 or handle error appropriately
    return -1;
}

// Creates a super triangle that encloses all points in the mesh
Triangle Mesh::superTriangle()
{
    Point p0(0, 0);
    Point p1(30, 0);
    Point p2(15, 25.981);

    Triangle triSuper = Triangle(p0, p1, p2);
    triSuper.setPointIndex(0, vecPtShape.size());
    vecPtShape.push_back(p0);
    triSuper.setPointIndex(1, vecPtShape.size());
    vecPtShape.push_back(p1);
    triSuper.setPointIndex(2, vecPtShape.size());
    vecPtShape.push_back(p2);

    // Append points to the end of vecPtShape

    triSuper.setIndex(0);

    return triSuper;
}

// Removes the helper triangles that include points of the super triangle.
void Mesh::removeHelperTriangles()
{
    std::vector<int> trianglesToRemove;  // Vector to store indices of triangles to be removed

    // Iterate over all triangles in the mesh
    for (int i = 0; i < vecTriangles.size(); ++i)
    {
        const Triangle& triangle = vecTriangles[i];

        // Check each point in the current triangle
        for (int j = 0; j < 3; ++j)
        {
            // If a point in the triangle belongs to the super triangle (helper points)
            if (triangle.getPointIndex(j) >= vecPtShape.size() - 3)
            {
                // Mark this triangle for removal
                trianglesToRemove.push_back(i);

                // Update the neighbors of the triangle being removed
                updateRemovedNeighbours(triangle.getIndex());
                break;  // Exit the loop as this triangle will be removed
            }
        }
    }

    // Remove the triangles marked for deletion, starting from the end to avoid index issues
    for (int i = trianglesToRemove.size() - 1; i >= 0; --i)
    {
        vecTriangles.erase(vecTriangles.begin() + trianglesToRemove[i]);
    }

    // Remove the last three points corresponding to the super triangle
    vecPtShape.resize(vecPtShape.size() - 3);

    // Update triangle indices to reflect the removal
    updateTriangleIndicesAfterRemoval();
}

// Updates triangle indices after some triangles have been removed.
void Mesh::updateTriangleIndicesAfterRemoval()
{
    // Iterate through all remaining triangles
    for (int iTriangleIndex = 0; iTriangleIndex < vecTriangles.size(); ++iTriangleIndex)
    {
        Triangle& currentTriangle = vecTriangles[iTriangleIndex];
        int iOldIndex = currentTriangle.getIndex();  // Get the current triangle index

        // Update the triangle's index if it has changed
        if (iOldIndex != iTriangleIndex)
        {
            currentTriangle.setIndex(iTriangleIndex);

            // Update the neighbors of the current triangle to reference the new index
            for (int i = 0; i < 3; ++i)
            {
                int iTriangleNeighbourIndex = currentTriangle.getNeighbourIndex(i);
                if (iTriangleNeighbourIndex == -1) continue;  // Skip if no neighbor

                // Iterate through all triangles to find and update the neighbor's references
                for (Triangle& triNeighbour : vecTriangles)
                {
                    if (triNeighbour.getIndex() == iTriangleNeighbourIndex)
                    {
                        // Update the specific neighbor reference to the new index
                        for (int j = 0; j < 3; ++j)
                        {
                            if (triNeighbour.getNeighbourIndex(j) == iOldIndex)
                            {
                                triNeighbour.setNeighbourIndex(j, iTriangleIndex);
                                break;  // Exit loop once the neighbor is updated
                            }
                        }
                    }
                }
            }
        }
    }
}

// Updates the neighbors of a triangle that has been removed.
void Mesh::updateRemovedNeighbours(int iRemovedTriangleIndex)
{
    const Triangle& triRemoved = vecTriangles[iRemovedTriangleIndex];  // Get the removed triangle

    // Iterate through each edge of the removed triangle
    for (int i = 0; i < 3; ++i)
    {
        int removedNeighborIndex = triRemoved.getNeighbourIndex(i);
        if (removedNeighborIndex == -1) continue;  // Skip if no neighbor

        Triangle& triNeighbour = vecTriangles[removedNeighborIndex];

        // Update the neighbor reference to indicate it no longer has a neighbor on this edge
        for (int j = 0; j < 3; ++j)
        {
            if (triNeighbour.getNeighbourIndex(j) == iRemovedTriangleIndex)
            {
                triNeighbour.setNeighbourIndex(j, -1);  // Mark as no neighbor
            }
        }
    }
}

// Creates new triangles by splitting an existing triangle based on the provided point index.
void Mesh::createTriangles(int iTriangleIndex, int iPointIndex)
{
    // Validate indices to ensure they are within bounds
    if (iTriangleIndex < 0 || iTriangleIndex >= vecTriangles.size() || iPointIndex < 0 || iPointIndex >= vecPtShape.size()) {
        std::cerr << "Invalid indices provided to createTriangles." << std::endl;
        return;
    }

    // Reference to the target point in vecPtShape
    const Point& ptTargetPoint = vecPtShape[iPointIndex];
    // Reference to the triangle being split
    Triangle& triCurrent = vecTriangles[iTriangleIndex];

    // Check if the target point lies inside the triangle but not on any edge
    if (triCurrent.onEdge(ptTargetPoint) < 0)
    {
        // Create two new triangles using two points from the current triangle and the target point
        Triangle triNewTriangle1(triCurrent.getPoint(0), triCurrent.getPoint(1), ptTargetPoint);
        Triangle triNewTriangle2(triCurrent.getPoint(1), triCurrent.getPoint(2), ptTargetPoint);

        // Set the indices for the new triangles
        int iNewIndex1 = vecTriangles.size();
        int iNewIndex2 = iNewIndex1 + 1;
        triNewTriangle1.setIndex(iNewIndex1);
        triNewTriangle2.setIndex(iNewIndex2);

        // Set the point indices for the new triangles
        triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(0));
        triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(1));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        triNewTriangle2.setPointIndex(0, triCurrent.getPointIndex(1));
        triNewTriangle2.setPointIndex(1, triCurrent.getPointIndex(2));
        triNewTriangle2.setPointIndex(2, iPointIndex);

        // Update the current triangle with the new point and its index
        triCurrent.setPointIndex(1, iPointIndex);
        triCurrent.setPoint(1, ptTargetPoint);

        // Update neighbors for the new triangles
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex); // Set the current triangle as a neighbor
        triNewTriangle2.setNeighbourIndex(1, iTriangleIndex);

        // Set the new triangles as neighbors of each other
        triNewTriangle1.setNeighbourIndex(1, iNewIndex2);
        triNewTriangle2.setNeighbourIndex(2, iNewIndex1);

        // Save the old neighbors of the current triangle
        int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(0);
        int iOldNeighbourIndex2 = triCurrent.getNeighbourIndex(1);

        // Update the current triangle's neighbors to point to the new triangles
        triCurrent.setNeighbourIndex(0, iNewIndex1);
        triCurrent.setNeighbourIndex(1, iNewIndex2);

        // Update the old neighbor of the current triangle to point to the new triangle
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, iNewIndex1);
                    break;
                }
            }
            triNewTriangle1.setNeighbourIndex(0, iOldNeighbourIndex1);
        }

        // Repeat the neighbor update process for the second neighbor
        if (iOldNeighbourIndex2 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex2];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, iNewIndex2);
                    break;
                }
            }
            triNewTriangle2.setNeighbourIndex(0, iOldNeighbourIndex2);
        }

        // Add the new triangles to the mesh's triangle list
        vecTriangles.push_back(triNewTriangle1);
        vecTriangles.push_back(triNewTriangle2);

        // Check the circumcircles of the new triangles and swap edges if necessary to maintain the Delaunay condition
        if (vecTriangles[iTriangleIndex].getNeighbourIndex(2) != -1 && vecTriangles[vecTriangles[iTriangleIndex].getNeighbourIndex(2)].isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);
            swapAll(neighbourQueue, iPointIndex);
        }

        if (vecTriangles[iNewIndex1].getNeighbourIndex(0) != -1 && vecTriangles[vecTriangles[iNewIndex1].getNeighbourIndex(0)].isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
            swapAll(neighbourQueue, iPointIndex);
        }

        if (vecTriangles[iNewIndex2].getNeighbourIndex(0) != -1 && vecTriangles[vecTriangles[iNewIndex2].getNeighbourIndex(0)].isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex2, iPointIndex, 0);
            swapAll(neighbourQueue, iPointIndex);
        }
    }
    else
    {
        // Handle cases where the target point lies on an edge of the current triangle
        handleEdgeCase(iTriangleIndex, iPointIndex);
    }
}

// Handles special edge cases when a triangle's edge is processed or modified.
void Mesh::handleEdgeCase(int iTriangleIndex, int iPointIndex)
{
    {
        // Reference to the point being processed and the triangle that is being split
        const Point& ptTargetPoint = vecPtShape[iPointIndex];
        Triangle& triCurrent = vecTriangles[iTriangleIndex];

        // Create a new triangle that will be used to split the current triangle
        Triangle triNewTriangle1;
        int iNewIndex1 = vecTriangles.size();
        triNewTriangle1.setIndex(iNewIndex1);

        // Handle case where the point is on Edge 0 (pt0 to pt1)
        if(triCurrent.onEdge(ptTargetPoint) == 0)
        {
            // Set points for the new triangle and update indices
            triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
            triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
            triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            // Set neighbor relationships for the new triangle
            triNewTriangle1.setNeighbourIndex(1, iTriangleIndex);
            triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));

            int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);

            // Update the old neighbor to reference the new triangle
            if (iOldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex1];
                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                    {
                        triOldNeighbour.setNeighbourIndex(i, iNewIndex1);
                        break;
                    }
                }
            }

            // Update the current triangle to include the new point
            triCurrent.setPointIndex(1, iPointIndex);
            triCurrent.setPoint(1, ptTargetPoint);
            triCurrent.setNeighbourIndex(1, iNewIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(0);
            triCurrent.setNeighbourIndex(0, -1);

            // Add the new triangle to the list of triangles
            vecTriangles.push_back(triNewTriangle1);

            // Update references to the current and new triangles
            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[iNewIndex1];

            // Check and handle circumcircles for possible swaps
            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);
                swapAll(neighbourQueue, iPointIndex);

                if (areNeighbours(iTriangleIndex, iNewIndex1))
                {
                    iTriangleIndex = triNeighbour0.getIndex();
                }
            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
                swapAll(neighbourQueue, iPointIndex);

                if (areNeighbours(iTriangleIndex, iNewIndex1))
                {
                    iNewIndex1 = triNeighbour1.getIndex();
                }
            }

            // Handle the opposite side of the triangle if necessary
            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }
        }

        // Handle case where the point is on Edge 1 (pt1 to pt2)
        else if (triCurrent.onEdge(ptTargetPoint) == 1)
        {
            // Set points for the new triangle and update indices
            triNewTriangle1.setPoint(0, triCurrent.getPoint(0));
            triNewTriangle1.setPoint(1, triCurrent.getPoint(1));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(0));
            triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(1));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            // Set neighbor relationships for the new triangle
            triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(0));
            triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

            int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(0);

            // Update the old neighbor to reference the new triangle
            if (iOldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex1];
                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                    {
                        triOldNeighbour.setNeighbourIndex(i, iNewIndex1);
                        break;
                    }
                }
            }

            // Update the current triangle to include the new point
            triCurrent.setPointIndex(1, iPointIndex);
            triCurrent.setPoint(1, ptTargetPoint);
            triCurrent.setNeighbourIndex(0, iNewIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(1);
            triCurrent.setNeighbourIndex(1, -1);

            // Add the new triangle to the list of triangles
            vecTriangles.push_back(triNewTriangle1);

            // Update references to the current and new triangles
            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[iNewIndex1];

            // Check and handle circumcircles for possible swaps
            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);
                swapAll(neighbourQueue, iPointIndex);

                if (areNeighbours(iTriangleIndex, iNewIndex1))
                {
                    iTriangleIndex = triNeighbour0.getIndex();
                }
            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
                swapAll(neighbourQueue, iPointIndex);

                if (areNeighbours(iTriangleIndex, iNewIndex1))
                {
                    iNewIndex1 = triNeighbour1.getIndex();
                }
            }

            // Handle the opposite side of the triangle if necessary
            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }
        }

        // Handle case where the point is on Edge 2 (pt2 to pt0)
        else
        {
            // Set points for the new triangle and update indices
            triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
            triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
            triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            // Set neighbor relationships for the new triangle
            triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));
            triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

            int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);

            // Update the old neighbor to reference the new triangle
            if (iOldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex1];
                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                    {
                        triOldNeighbour.setNeighbourIndex(i, iNewIndex1);
                        break;
                    }
                }
            }

            // Update the current triangle to include the new point
            triCurrent.setPointIndex(2, iPointIndex);
            triCurrent.setPoint(2, ptTargetPoint);
            triCurrent.setNeighbourIndex(1, iNewIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(2);
            triCurrent.setNeighbourIndex(2, -1);

            // Add the new triangle to the list of triangles
            vecTriangles.push_back(triNewTriangle1);

            // Update references to the current and new triangles
            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[iNewIndex1];

            // Check and handle circumcircles for possible swaps
            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(0)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 0);
                swapAll(neighbourQueue, iPointIndex);

                if (areNeighbours(iTriangleIndex, iNewIndex1))
                {
                    iTriangleIndex = triNeighbour0.getIndex();
                }
            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {
                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
                swapAll(neighbourQueue, iPointIndex);

                if (areNeighbours(iTriangleIndex, iNewIndex1))
                {
                    iNewIndex1 = triNeighbour1.getIndex();
                }
            }

            // Handle the opposite side of the triangle if necessary
            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }
        }
    }
}

// Creates new triangles on the opposite side of a specified triangle and point.
void Mesh::createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{
    // Retrieve the point that will be used to create new triangles
    const Point& ptTargetPoint = vecPtShape[iPointIndex];

    // Get the current triangle that will be modified
    Triangle& triCurrent = vecTriangles[iTriangleIndex];

    // Create a new triangle to be added to the mesh
    Triangle triNewTriangle1;
    int iNewIndex1 = vecTriangles.size();
    triNewTriangle1.setIndex(iNewIndex1);

    // Handle the case where the target point is on Edge 0 (between points 1 and 2)
    if (triCurrent.onEdge(ptTargetPoint) == 0)
    {
        // Define the points of the new triangle
        triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
        triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        // Set the point indices for the new triangle
        triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
        triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        // Set the neighbors of the new triangle
        triNewTriangle1.setNeighbourIndex(1, iTriangleIndex);
        triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));

        // Update the neighbor index of the old neighbor
        int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, iNewIndex1);
                    break;
                }
            }
        }

        // Update the current triangle with the new point and neighbor
        triCurrent.setPointIndex(1, iPointIndex);
        triCurrent.setPoint(1, ptTargetPoint);
        triCurrent.setNeighbourIndex(1, iNewIndex1);

        // Determine the appropriate neighbor indices for the new triangle
        if (areNeighbours(iTriangleIndex, iNeighbourIndex0))
        {
            triNewTriangle1.setNeighbourIndex(2, iNeighbourIndex1);
            triCurrent.setNeighbourIndex(0, iNeighbourIndex0);
        }
        else
        {
            triNewTriangle1.setNeighbourIndex(2, iNeighbourIndex0);
            triCurrent.setNeighbourIndex(0, iNeighbourIndex1);
        }

        // Add the new triangle to the list of triangles
        vecTriangles.push_back(triNewTriangle1);

        // Update the reference to the current triangle and perform edge neighbor updates
        triCurrent = vecTriangles[iTriangleIndex];
        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);

        // Check if the new triangles' neighbors are in the circumcircle of the target point
        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];
        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);
            swapAll(neighbourQueue, iPointIndex);
        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
            swapAll(neighbourQueue, iPointIndex);
        }
    }
    // Handle the case where the target point is on Edge 1 (between points 0 and 1)
    else if (triCurrent.onEdge(ptTargetPoint) == 1)
    {
        // Define the points of the new triangle
        triNewTriangle1.setPoint(0, triCurrent.getPoint(0));
        triNewTriangle1.setPoint(1, triCurrent.getPoint(1));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        // Set the point indices for the new triangle
        triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(0));
        triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(1));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        // Set the neighbors of the new triangle
        triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(0));
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

        // Update the neighbor index of the old neighbor
        int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(0);
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, iNewIndex1);
                    break;
                }
            }
        }

        // Update the current triangle with the new point and neighbor
        triCurrent.setPointIndex(1, iPointIndex);
        triCurrent.setPoint(1, ptTargetPoint);
        triCurrent.setNeighbourIndex(0, iNewIndex1);

        // Determine the appropriate neighbor indices for the new triangle
        if (areNeighbours(iTriangleIndex, iNeighbourIndex1))
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex0);
            triCurrent.setNeighbourIndex(1, iNeighbourIndex1);
        }
        else
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex1);
            triCurrent.setNeighbourIndex(1, iNeighbourIndex0);
        }

        // Add the new triangle to the list of triangles
        vecTriangles.push_back(triNewTriangle1);

        // Update the reference to the current triangle and perform edge neighbor updates
        triCurrent = vecTriangles[iTriangleIndex];
        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);

        // Check if the new triangles' neighbors are in the circumcircle of the target point
        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];
        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);
            swapAll(neighbourQueue, iPointIndex);
        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
            swapAll(neighbourQueue, iPointIndex);
        }
    }
    // Handle the case where the target point is on Edge 2 (between points 1 and 2)
    else
    {
        // Define the points of the new triangle
        triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
        triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        // Set the point indices for the new triangle
        triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
        triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        // Set the neighbors of the new triangle
        triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

        // Update the neighbor index of the old neighbor
        int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(1);
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.getNeighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, iNewIndex1);
                    break;
                }
            }
        }

        // Update the current triangle with the new point and neighbor
        triCurrent.setPointIndex(2, iPointIndex);
        triCurrent.setPoint(2, ptTargetPoint);
        triCurrent.setNeighbourIndex(1, iNewIndex1);

        // Determine the appropriate neighbor indices for the new triangle
        if (areNeighbours(iTriangleIndex, iNeighbourIndex0))
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex1);
            triCurrent.setNeighbourIndex(2, iNeighbourIndex0);
        }
        else
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex0);
            triCurrent.setNeighbourIndex(2, iNeighbourIndex1);
        }

        // Add the new triangle to the list of triangles
        vecTriangles.push_back(triNewTriangle1);

        // Update the reference to the current triangle and perform edge neighbor updates
        triCurrent = vecTriangles[iTriangleIndex];
        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);

        // Check if the new triangles' neighbors are in the circumcircle of the target point
        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(0)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];
        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 0);
            swapAll(neighbourQueue, iPointIndex);
        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
            swapAll(neighbourQueue, iPointIndex);
        }
    }
}

// Updates the neighboring triangles' edge references after swapping edges.
void Mesh::updateEdgeNeighbours(int iTriangleIndex, int iNewTriangleIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{
    // Swap neighbor indices if necessary to maintain consistency
    if (areNeighbours(iTriangleIndex, iNeighbourIndex0))
    {
        std::swap(iNeighbourIndex0, iNeighbourIndex1);
    }

    // Retrieve the triangles based on their indices
    Triangle& triCurrent = vecTriangles[iTriangleIndex];
    Triangle& triNewTriangle = vecTriangles[iNewTriangleIndex];

    Triangle& triOldNeighbour0 = vecTriangles[iNeighbourIndex0];
    Triangle& triOldNeighbour1 = vecTriangles[iNeighbourIndex1];

    int iEdgeIndex0, iEdgeIndex1;

    // Find the edge indices of the current and new triangles that correspond to the neighbor indices
    for (int i = 0; i < 3; ++i)
    {
        if (triNewTriangle.getNeighbourIndex(i) == iNeighbourIndex0)
        {
            iEdgeIndex0 = i;
        }

        if (triCurrent.getNeighbourIndex(i) == iNeighbourIndex1)
        {
            iEdgeIndex1 = i;
        }
    }

    // Calculate the midpoints of the edges that connect the new triangle and current triangle
    Point ptMid0 = triNewTriangle.getEdgeMidpoint(iEdgeIndex0);
    Point ptMid1 = triCurrent.getEdgeMidpoint(iEdgeIndex1);

    // Determine the target edge indices in the old neighbor triangles
    int iTargetEdgeIndex0 = triOldNeighbour0.onEdge(ptMid0);
    int iTargetEdgeIndex1 = triOldNeighbour1.onEdge(ptMid1);

    // Update the neighbor indices in the old neighbor triangles
    triOldNeighbour0.setNeighbourIndex(iTargetEdgeIndex0, iNewTriangleIndex);
    triOldNeighbour1.setNeighbourIndex(iTargetEdgeIndex1, iTriangleIndex);
}

// Determines if two triangles share an edge by checking their points.
bool Mesh::areNeighbours(int iTri1, int iTri2)
{
    // Retrieve the triangles based on their indices
    const Triangle& tri1 = vecTriangles[iTri1];
    const Triangle& tri2 = vecTriangles[iTri2];

    int iSharedCount = 0;  // Counter for shared points

    // Identify shared points between the two triangles
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int pointCurrent = tri1.getPointIndex(i);
            int pointNeighbour = tri2.getPointIndex(j);

            if (pointCurrent == pointNeighbour)
            {
                ++iSharedCount;  // Increment shared points count
            }
        }
    }

    // Two triangles are considered neighbors if they share exactly two points
    if (iSharedCount == 2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Checks neighboring triangles' circumcircles to find those containing the target point.
std::queue<int> Mesh::checkNeighboringCircumcircles(int iTriangleIndex, int iPointIndex, int iEdgeIndex)
{
    // Initialize a queue to keep track of triangles to check
    std::queue<int> neighbourQueue;
    neighbourQueue.push(iTriangleIndex);

    // Vector to track visited triangles to avoid processing them multiple times
    std::vector<bool> visited(vecTriangles.size(), false);

    const Point& ptTargetPoint = vecPtShape[iPointIndex];
    size_t iQueueIndex = 0;
    bool firstIteration = true;

    // Traverse the queue of triangles
    while (iQueueIndex < neighbourQueue.size())
    {
        int iCurrentIndex = neighbourQueue.back();

        // Check if the current triangle has not been visited
        // Add all triangles that contain the point in their circumcircle to the queue
        if (!visited[iCurrentIndex])
        {
            visited[iCurrentIndex] = true;
            Triangle& triCurrent = vecTriangles[iCurrentIndex];

            // Check only specified edge for the first iteration
            if (firstIteration)
            {
                int iNeighbourIndex = triCurrent.getNeighbourIndex(iEdgeIndex);

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

    // Return the queue containing all triangles whose circumcircles include the target point
    return neighbourQueue;
}

// Swaps the edge between two triangles and updates their neighboring relationships.
void Mesh::swapEdge(int iTri1, int iTri2)
{
    // Retrieve the triangles based on their indices
    Triangle& triCurrent = vecTriangles[iTri1];
    Triangle& triNeighbour = vecTriangles[iTri2];

    int diff1 = -1, diff2 = -1;
    std::vector<int> shared;

    // Identify shared points between the two triangles
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int pointCurrent = triCurrent.getPointIndex(i);
            int pointNeighbour = triNeighbour.getPointIndex(j);

            if (pointCurrent == pointNeighbour)
            {
                shared.push_back(pointCurrent); // Store shared points
            }
        }
    }

    // Identify the different points in each triangle
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

    // Retrieve neighbor indices and point indices for both triangles
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
            int iSharedEdge = findSharedEdge(triCurrent, diff1, diff2);
            int iNewEdge = findNewEdge(triCurrent, i, iSharedEdge);

            int iStolenEdge = -1;
            if ((triNeighbour.getPointIndex(0) == shared[1] && triNeighbour.getPointIndex(1) == diff2) ||
                (triNeighbour.getPointIndex(1) == shared[1] && triNeighbour.getPointIndex(0) == diff2))
            {
                iStolenEdge = iNeighbourN0;
            }
            else if ((triNeighbour.getPointIndex(1) == shared[1] && triNeighbour.getPointIndex(2) == diff2) ||
                     (triNeighbour.getPointIndex(2) == shared[1] && triNeighbour.getPointIndex(1) == diff2))
            {
                iStolenEdge = iNeighbourN1;
            }
            else if ((triNeighbour.getPointIndex(2) == shared[1] && triNeighbour.getPointIndex(0) == diff2) ||
                     (triNeighbour.getPointIndex(0) == shared[1] && triNeighbour.getPointIndex(2) == diff2))
            {
                iStolenEdge = iNeighbourN2;
            }

            // Update the neighbor indices in triCurrent
            triCurrent.setNeighbourIndex(iSharedEdge, iTri2);
            triCurrent.setNeighbourIndex(iNewEdge, iStolenEdge);
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

            // Determine the shared edge and new edge after swap
            int iSharedEdge = findSharedEdge(triNeighbour, diff1, diff2);
            int iNewEdge = findNewEdge(triNeighbour, i, iSharedEdge);

            int iStolenEdge = -1;
            if ((iCurrentPt0 == diff1 && iCurrentPt1 == shared[0]) ||
                (iCurrentPt1 == diff1 && iCurrentPt0 == shared[0]))
            {
                iStolenEdge = iCurrentN0;
            }
            else if ((iCurrentPt1 == diff1 && iCurrentPt2 == shared[0]) ||
                     (iCurrentPt2 == diff1 && iCurrentPt1 == shared[0]))
            {
                iStolenEdge = iCurrentN1;
            }
            else if ((iCurrentPt2 == diff1 && iCurrentPt0 == shared[0]) ||
                     (iCurrentPt0 == diff1 && iCurrentPt2 == shared[0]))
            {
                iStolenEdge = iCurrentN2;
            }

            // Update the neighbor indices in triNeighbour
            triNeighbour.setNeighbourIndex(iSharedEdge, iTri1);
            triNeighbour.setNeighbourIndex(iNewEdge, iStolenEdge);
            break;
        }
    }

    // Update neighbor triangles to reflect changes
    updateNeighboursAfterSwap(iCurrentN0, iTri1, iTri2);
    updateNeighboursAfterSwap(iCurrentN1, iTri1, iTri2);
    updateNeighboursAfterSwap(iCurrentN2, iTri1, iTri2);
    updateNeighboursAfterSwap(iNeighbourN0, iTri2, iTri1);
    updateNeighboursAfterSwap(iNeighbourN1, iTri2, iTri1);
    updateNeighboursAfterSwap(iNeighbourN2, iTri2, iTri1);
}

// Swaps edges in all triangles that have the target point within their circumcircles.
void Mesh::swapAll(std::queue<int>& neighbourQueue, int iPointIndex)
{
    Point& ptTargetPoint = vecPtShape[iPointIndex];

    while (neighbourQueue.size() > 1)
    {
        // Remove the oldest triangle from the queue
        neighbourQueue.pop();

        // Get the index of the next triangle to process
        int iNeighbourIndex = neighbourQueue.front();
        Triangle& triNeighbour = vecTriangles[iNeighbourIndex];

        // Find the triangle that contains the target point
        int iTriangleIndex = triNeighbour.findPathToContainingTriangle(ptTargetPoint);

        // If a valid triangle index is found, perform an edge swap
        if (iNeighbourIndex >= 0)
        {
            swapEdge(iTriangleIndex, iNeighbourIndex);
        }
    }
}

// Finds the index of the shared edge between two triangles based on their differing points.
int Mesh::findSharedEdge(const Triangle& tri, int iDiff1, int iDiff2) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (((tri.getPointIndex(i) == iDiff1 && tri.getPointIndex((i + 1) % 3) == iDiff2)) ||
            ((tri.getPointIndex(i) == iDiff2 && tri.getPointIndex((i + 1) % 3) == iDiff1)))
        {
            return i;
        }
    }
    return -1; // Return -1 if the edge is not found
}

// Determines the new edge index for a triangle after an edge swap.
int Mesh::findNewEdge(const Triangle& tri, int i, int iSharedEdge) const
{
    if (i == 0)
    {
        if (iSharedEdge == 0)
        {
            return 2;
        }
        else if (iSharedEdge == 2)
        {
            return 0;
        }
    }
    else if (i == 1)
    {
        if (iSharedEdge == 0)
        {
            return 1;
        }
        else if (iSharedEdge == 1)
        {
            return 0;
        }
    }
    else if (i == 2)
    {
        if (iSharedEdge == 1)
        {
            return 2;
        }
        else if (iSharedEdge == 2)
        {
            return 1;
        }
    }
    return -1; // Return -1 if the new edge index cannot be determined
}

// Updates the neighbor indices of a triangle when one of its neighbors changes.
void Mesh::updateNeighboursAfterSwap(int oldNeighborIndex, int oldTriangleIndex, int newTriangleIndex)
{
    // Update the neighbor index in the old neighbor triangle if necessary
    if (oldNeighborIndex != -1 && oldNeighborIndex != oldTriangleIndex && oldNeighborIndex != newTriangleIndex)
    {
        Triangle& oldNeighbor = vecTriangles[oldNeighborIndex];
        const Triangle& triCurrent = vecTriangles[oldTriangleIndex];

        // Check if the old neighbor triangle needs to be updated
        bool needsUpdate = true;
        for (int i = 0; i < 3; ++i)
        {
            if (triCurrent.getNeighbourIndex(i) == oldNeighborIndex)
            {
                needsUpdate = false;
                break;
            }
        }

        // Update the neighbor index in the old neighbor triangle
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

// Adjusts triangles to make them more equilateral by adding new points and creating new triangles.
void Mesh::equilateralizeTriangles()
{
    int smallestAngleTriangleIndex;
    int iterationCount = 0;

    // Continuously process triangles until no triangles with small angles are found
    while ((smallestAngleTriangleIndex = locateSmallestAngle()) >= 0)
    {
        // Retrieve the triangle with the smallest angle
        auto& triangle = vecTriangles[smallestAngleTriangleIndex];

        Point circumcenter = triangle.getCircumcenter();

        vecPtShape.push_back(circumcenter);

        // Find the triangle that contains the circumcenter
        int containingTriangleIndex = findContainingTriangle(circumcenter);

        int newPointIndex = vecPtShape.size() - 1;

        // Create new triangles by connecting the new point with the containing triangle
        createTriangles(containingTriangleIndex, newPointIndex);

        // For debugging purposes,
        //if (++iterationCount == 3){break;}
    }
}

// Finds the index of the triangle with the smallest angle to be processed for equilateralization.
int Mesh::locateSmallestAngle()
{
    double minAngle = 180.0; // Initialize with the largest possible angle
    int minAngleTriangleIndex = -1; // Index of the triangle with the smallest angle

    // Update the minimum angle and corresponding triangle index
    auto updateMinAngle = [&](double angle, int triangleIndex) {
        if (angle < minAngle)
        {
            minAngle = angle;
            minAngleTriangleIndex = triangleIndex;
        }
    };

    // Iterate over all triangles in the mesh
    for (const auto& triangle : vecTriangles)
    {
        // Check each angle of the triangle
        for (int j = 0; j < 3; ++j)
        {
            int neighborIndex = triangle.getNeighbourIndex(j);

            // Check if the edge is a border case (i.e., has no neighbor)
            if (neighborIndex == -1)
            {
                double innerAngle = triangle.getAng(j);
                updateMinAngle(innerAngle, triangle.getIndex());
                break;  // No need to check further angles for this triangle
            }
            else
            {
                updateMinAngle(triangle.getAng(j), triangle.getIndex());
            }
        }
    }

    // Return the index of the triangle with the smallest angle if it's below threshold, else return -1
    if (minAngle < 40.0)
    {
        return minAngleTriangleIndex;
    }
    else
    {
        return -1;
    }
}
