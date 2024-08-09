#include "mesh.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <stack>
#include <queue>

// Constructor: iCurrentNeighbourtializes the mesh with a given set of points
Mesh::Mesh(const std::vector<Point>& vecPt)
{
    setShape(vecPt);
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

// Builds mesh from points and triangles
void Mesh::buildMesh()
{
    int iPointIndex = 0;
    for (auto point: vecPtShape)
    {
        int iTriIndex = findContainingTriangle(point);
        createTriangles(iTriIndex, iPointIndex);

        iPointIndex += 1;

        if ((vecPtShape.size() - 3) == iPointIndex){ break;}
    }
}

void Mesh::removeHelperTriangles()
{
    // Vector to hold indices of triangles to be removed
    std::vector<int> trianglesToRemove;

    // Iterate through all triangles
    for (int i = 0; i < vecTriangles.size(); ++i)
    {
        const Triangle& triangle = vecTriangles[i];

        // Check each point in the triangle
        for (int j = 0; j < 3; ++j)
        {
            // If the point index is within the range of helper points
            if (triangle.getPointIndex(j) >= vecPtShape.size() - 3)
            {
                // Record the triangle's index for removal
                trianglesToRemove.push_back(i);
                updateRemovedNeighbours(triangle.getIndex());
                break;  // Exit loop early since this triangle will be removed
            }
        }
    }

    // Remove the triangles in reverse order to avoid invalidating indices
    for (int i = trianglesToRemove.size() - 1; i >= 0; --i)
    {
        vecTriangles.erase(vecTriangles.begin() + trianglesToRemove[i]);
    }

    // Remove the last three points added for the super triangle
    vecPtShape.resize(vecPtShape.size() - 3);
    updateTriangleIndicesAfterRemoval();
}

void Mesh::updateTriangleIndicesAfterRemoval()
{
    for (int iTriangleIndex = 0; iTriangleIndex < vecTriangles.size(); ++iTriangleIndex)
    {
        Triangle& currentTriangle = vecTriangles[iTriangleIndex];
        int iOldIndex = currentTriangle.getIndex();

        // Only update if the index has actually changed
        if (iOldIndex != iTriangleIndex)
        {
            currentTriangle.setIndex(iTriangleIndex);

            // Update the neighbors of the current triangle
            for (int i = 0; i < 3; ++i)
            {
                int iTriangleNeighbourIndex = currentTriangle.getNeighbourIndex(i);
                if (iTriangleNeighbourIndex == -1) continue; // Skip if no neighbor

                for (Triangle& triNeighbour : vecTriangles) // Iterate by reference
                {
                    if (triNeighbour.getIndex() == iTriangleNeighbourIndex)
                    {
                        // Update the neighbor's reference to the old index
                        for (int j = 0; j < 3; ++j)
                        {
                            if (triNeighbour.getNeighbourIndex(j) == iOldIndex)
                            {
                                triNeighbour.setNeighbourIndex(j, iTriangleIndex);
                                break; // No need to check other neighbors once updated
                            }
                        }
                    }
                }
            }
        }
    }
}

void Mesh::updateRemovedNeighbours(int iRemovedTriangleIndex)
{
    const Triangle& triRemoved = vecTriangles[iRemovedTriangleIndex];

    for (int i = 0; i < 3; ++i)
    {
        int removedNeighborIndex = triRemoved.getNeighbourIndex(i);
        if (removedNeighborIndex == -1) continue; // Skip if no neighbor

        Triangle& triNeighbour = vecTriangles[removedNeighborIndex];
        for (int j = 0; j < 3; ++j)
        {
            if (triNeighbour.getNeighbourIndex(j) == iRemovedTriangleIndex)
            {
                triNeighbour.setNeighbourIndex(j, -1);
            }
        }
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

    // Case when the point is inside the triangle but not on an edge
    if (triCurrent.onEdge(ptTargetPoint) < 0)
    {
        // Create two new triangles of different points from the current triangle and the target point
        Triangle triNewTriangle1(triCurrent.getPoint(0), triCurrent.getPoint(1), ptTargetPoint);
        Triangle triNewTriangle2(triCurrent.getPoint(1), triCurrent.getPoint(2), ptTargetPoint);

        // Set indices for the new triangles
        int iNewIndex1 = vecTriangles.size();
        int iNewIndex2 = iNewIndex1 + 1;
        triNewTriangle1.setIndex(iNewIndex1);
        triNewTriangle2.setIndex(iNewIndex2);

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
        triNewTriangle1.setNeighbourIndex(1, iNewIndex2);
        triNewTriangle2.setNeighbourIndex(2, iNewIndex1);

        int iOldNeighbourIndex1 = triCurrent.getNeighbourIndex(0);
        int iOldNeighbourIndex2 = triCurrent.getNeighbourIndex(1);

        triCurrent.setNeighbourIndex(0, iNewIndex1);
        triCurrent.setNeighbourIndex(1, iNewIndex2);

        // Update the neighboring relationships with the old neighbor
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


        // Update the neighboring relationships with the old neighbor
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

        // Add the new triangles to the vector
        vecTriangles.push_back(triNewTriangle1);
        vecTriangles.push_back(triNewTriangle2);


        // Check circumcircles of the new triangles and swap edges if necessary
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
        handleEdgeCase(iTriangleIndex, iPointIndex);
    }
}

void Mesh::handleEdgeCase(int iTriangleIndex, int iPointIndex)
{
    {
        const Point& ptTargetPoint = vecPtShape[iPointIndex];
        Triangle& triCurrent = vecTriangles[iTriangleIndex];

        // Triangle gets split in half
        Triangle triNewTriangle1;
        int iNewIndex1 = vecTriangles.size();
        triNewTriangle1.setIndex(iNewIndex1);

        // Case where the point is on Edge 0 (pt0 to pt1)
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

            triCurrent.setPointIndex(1, iPointIndex);
            triCurrent.setPoint(1, ptTargetPoint);
            triCurrent.setNeighbourIndex(1, iNewIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(0);
            triCurrent.setNeighbourIndex(0, -1);

            // Set indices for the new triangle
            vecTriangles.push_back(triNewTriangle1);

            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[iNewIndex1];


            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);
                swapAll(neighbourQueue, iPointIndex);
                iTriangleIndex = triNeighbour0.getIndex();

            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
                swapAll(neighbourQueue, iPointIndex);
                iNewIndex1 = triNeighbour1.getIndex();

            }

            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }
        }

        // Case where the point is on Edge 1 (pt1 to pt2)
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


            triCurrent.setPointIndex(1, iPointIndex);
            triCurrent.setPoint(1, ptTargetPoint);

            triCurrent.setNeighbourIndex(0, iNewIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(1);
            triCurrent.setNeighbourIndex(1, -1);

            // Set indices for the new triangle
            vecTriangles.push_back(triNewTriangle1);

            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[iNewIndex1];

            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];


            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);
                swapAll(neighbourQueue, iPointIndex);
                iTriangleIndex = triNeighbour0.getIndex();

            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 0);
                swapAll(neighbourQueue, iPointIndex);
                iNewIndex1 = triNeighbour1.getIndex();

            }

            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }

        }

        // Case where the point is on Edge 2 (pt2 to pt0)
        else
        {
            triNewTriangle1.setPoint(0, triCurrent.getPoint(1));
            triNewTriangle1.setPoint(1, triCurrent.getPoint(2));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.getPointIndex(1));
            triNewTriangle1.setPointIndex(1, triCurrent.getPointIndex(2));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));
            triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

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

            triCurrent.setPointIndex(2, iPointIndex);
            triCurrent.setPoint(2, ptTargetPoint);

            triCurrent.setNeighbourIndex(1, iNewIndex1);

            const int iOldNeighbour = triCurrent.getNeighbourIndex(2);
            triCurrent.setNeighbourIndex(2, -1);

            // Set indices for the new triangle
            vecTriangles.push_back(triNewTriangle1);



            triCurrent = vecTriangles[iTriangleIndex];
            triNewTriangle1 = vecTriangles[iNewIndex1];

            const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(0)];
            const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(1)];

            if (triNeighbour0.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 0);
                swapAll(neighbourQueue, iPointIndex);
                iTriangleIndex = triNeighbour0.getIndex();

            }

            if (triNeighbour1.isInCircumcircle(ptTargetPoint))
            {

                std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1, iPointIndex, 1);
                swapAll(neighbourQueue, iPointIndex);
                iNewIndex1 = triNeighbour1.getIndex();

            }

            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }
        }
    }
}

void Mesh::createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{

    const Point& ptTargetPoint = vecPtShape[iPointIndex];
    Triangle& triCurrent = vecTriangles[iTriangleIndex];

    Triangle triNewTriangle1;
    int iNewIndex1 = vecTriangles.size();
    triNewTriangle1.setIndex(iNewIndex1);

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

        triCurrent.setPointIndex(1, iPointIndex);
        triCurrent.setPoint(1, ptTargetPoint);
        triCurrent.setNeighbourIndex(1, iNewIndex1);

        if (bAreNeighbours(iTriangleIndex, iNeighbourIndex0))
        {
            triNewTriangle1.setNeighbourIndex(2, iNeighbourIndex1);
            triCurrent.setNeighbourIndex(0, iNeighbourIndex0);
        }
        else
        {
            triNewTriangle1.setNeighbourIndex(2, iNeighbourIndex0);
            triCurrent.setNeighbourIndex(0, iNeighbourIndex1);
        }


        //Add new triangle to vecTriangles
        vecTriangles.push_back(triNewTriangle1);

        triCurrent = vecTriangles[iTriangleIndex];

        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);

        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

            swapAll(neighbourQueue, iPointIndex);

        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {

            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1,iPointIndex, 0);

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
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

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

        triCurrent.setPointIndex(1, iPointIndex);
        triCurrent.setPoint(1, ptTargetPoint);

        triCurrent.setNeighbourIndex(0, iNewIndex1);

        if (bAreNeighbours(iTriangleIndex, iNeighbourIndex1))
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex0);
            triCurrent.setNeighbourIndex(1, iNeighbourIndex1);
        }
        else
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex1);
            triCurrent.setNeighbourIndex(1, iNeighbourIndex0);
        }

        //Add new triangle to vecTriangles
        vecTriangles.push_back(triNewTriangle1);

        triCurrent = vecTriangles[iTriangleIndex];

        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);


        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(2)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 2);

            swapAll(neighbourQueue, iPointIndex);

        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {

            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1,iPointIndex, 0);

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

        triNewTriangle1.setNeighbourIndex(0, triCurrent.getNeighbourIndex(1));
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

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


        triCurrent.setPointIndex(2, iPointIndex);
        triCurrent.setPoint(2, ptTargetPoint);

        triCurrent.setNeighbourIndex(1, iNewIndex1);

        // Must determine which triangles are neighbours
        if (bAreNeighbours(iTriangleIndex, iNeighbourIndex0))
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex1);
            triCurrent.setNeighbourIndex(2, iNeighbourIndex0);
        }
        else
        {
            triNewTriangle1.setNeighbourIndex(1, iNeighbourIndex0);
            triCurrent.setNeighbourIndex(2, iNeighbourIndex1);
        }

        if (triCurrent.getIndex() == 13)
        {
            triCurrent.printPoints();
        }

        //Add new triangle to vecTriangles
        vecTriangles.push_back(triNewTriangle1);

        if (triCurrent.getIndex() == 13)
        {
            triCurrent.printPoints();
        }

        triCurrent = vecTriangles[iTriangleIndex];

        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);


        const Triangle& triNeighbour0 = vecTriangles[triCurrent.getNeighbourIndex(0)];
        const Triangle& triNeighbour1 = vecTriangles[triNewTriangle1.getNeighbourIndex(0)];

        if (triNeighbour0.isInCircumcircle(ptTargetPoint))
        {
            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iTriangleIndex, iPointIndex, 0);

            swapAll(neighbourQueue, iPointIndex);

        }

        if (triNeighbour1.isInCircumcircle(ptTargetPoint))
        {

            std::queue<int> neighbourQueue = checkNeighboringCircumcircles(iNewIndex1,iPointIndex, 0);

            swapAll(neighbourQueue, iPointIndex);
        }
    }
}

bool Mesh::bAreNeighbours(int iTri1, int iTri2)
{
    const Triangle& tri1 = vecTriangles[iTri1];
    const Triangle& tri2 = vecTriangles[iTri2];

    int iSharedCount = 0;
    // Identify shared points
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int pointCurrent = tri1.getPointIndex(i);
            int pointNeighbour = tri2.getPointIndex(j);

            if (pointCurrent == pointNeighbour)
            {
                ++iSharedCount;
            }
        }
    }

    if (iSharedCount == 2)
    {
        return true;
    }

    else
    {
        return false;
    }

}

void Mesh::updateEdgeNeighbours(int iTriangleIndex, int iNewTriangleIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{
    Triangle& triCurrent = vecTriangles[iTriangleIndex];
    Triangle& triNewTriangle1 = vecTriangles[iNewTriangleIndex];

    Triangle& triOldNeighbour0 = vecTriangles[iNeighbourIndex0];
    Triangle& triOldNeighbour1 = vecTriangles[iNeighbourIndex1];

    for (int i = 0; i < 3; ++i)
    {
        if (triOldNeighbour0.getNeighbourIndex(i) == -1)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (triCurrent.getNeighbourIndex(j) == triOldNeighbour0.getIndex())
                {
                    triOldNeighbour0.setNeighbourIndex(i, iTriangleIndex);
                }
                else if (triNewTriangle1.getNeighbourIndex(j) == triOldNeighbour0.getIndex())
                {
                    triOldNeighbour0.setNeighbourIndex(i, iNewTriangleIndex);
                }
            }
        }

        if (triOldNeighbour1.getNeighbourIndex(i) == -1)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (triCurrent.getNeighbourIndex(j) == triOldNeighbour1.getIndex())
                {
                    triOldNeighbour1.setNeighbourIndex(i, iTriangleIndex);
                }
                else if (triNewTriangle1.getNeighbourIndex(j) == triOldNeighbour1.getIndex())
                {
                    triOldNeighbour1.setNeighbourIndex(i, iNewTriangleIndex);
                }
            }
        }
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
            int iSharedEdge = findSharedEdge(triCurrent, diff1, diff2);
            int iNewEdge = findNewEdge(triCurrent, i, iSharedEdge);

            int iStolenEdge = -1;
            if ((triNeighbour.getPointIndex(0) == shared[1] && triNeighbour.getPointIndex(1) == diff2) || (triNeighbour.getPointIndex(1) == shared[1] && triNeighbour.getPointIndex(0) == diff2))
            {
                iStolenEdge = iNeighbourN0;
            }
            else if ((triNeighbour.getPointIndex(1) == shared[1] && triNeighbour.getPointIndex(2) == diff2) || (triNeighbour.getPointIndex(2) == shared[1] && triNeighbour.getPointIndex(1) == diff2))
            {
                iStolenEdge = iNeighbourN1;
            }
            else if ((triNeighbour.getPointIndex(2) == shared[1] && triNeighbour.getPointIndex(0) == diff2) || (triNeighbour.getPointIndex(0) == shared[1] && triNeighbour.getPointIndex(2) == diff2) )
            {
                iStolenEdge = iNeighbourN2;
            }


            // Update the neighbor indices
            triCurrent.setNeighbourIndex(iSharedEdge, iTri2);
            triCurrent.setNeighbourIndex(iNewEdge, iStolenEdge);
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
            int iSharedEdge = findSharedEdge(triNeighbour, diff1, diff2);
            int iNewEdge = findNewEdge(triNeighbour, i, iSharedEdge);

            int iStolenEdge = -1;
            if ((iCurrentPt0 == diff1 && iCurrentPt1 == shared[0]) || (iCurrentPt1 == diff1 && iCurrentPt0 == shared[0]))
            {
                iStolenEdge = iCurrentN0;
            }
            else if ((iCurrentPt1 == diff1 && iCurrentPt2 == shared[0]) || (iCurrentPt2 == diff1 && iCurrentPt1 == shared[0]))
            {
                iStolenEdge = iCurrentN1;
            }
            else if ((iCurrentPt2 == diff1 && iCurrentPt0 == shared[0]) || (iCurrentPt0 == diff1 && iCurrentPt2 == shared[0]))
            {
                iStolenEdge = iCurrentN2;
            }

            // Update the neighbor indices
            triNeighbour.setNeighbourIndex(iSharedEdge, iTri1);
            triNeighbour.setNeighbourIndex(iNewEdge, iStolenEdge);
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

int Mesh::findSharedEdge(const Triangle& tri, int iDiff1, int iDiff2) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (((tri.getPointIndex(i) == iDiff1 && tri.getPointIndex((i + 1) % 3) == iDiff2)) || ((tri.getPointIndex(i) == iDiff2 && tri.getPointIndex((i + 1) % 3) == iDiff1)))
        { return i; }
    }
    return -1;
}

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

bool Mesh::isNearEquilateral(int iTriangleIndex)
{
    const Triangle& currentTriangle = vecTriangles[iTriangleIndex];

    for (int i = 0; i < 3; ++i)
    {
        if (currentTriangle.getAng(i) < 40) { return false; }
    }

    return true;
}

void Mesh::equilateralizeTriangles()
{
    int iMinimimumAngleTriangleIndex;
    int count = 0; // for testing purposes
    // Continuously process triangles until locateSmallestAngle returns -1
    while ((iMinimimumAngleTriangleIndex = locateSmallestAngle()) >= 0)
    {
        auto& triangle = vecTriangles[iMinimimumAngleTriangleIndex];
        Point centroid = triangle.getCentroid();
        vecPtShape.push_back(centroid);
        int iPointIndex = vecPtShape.size() - 1;
        std::cout << "Creating triangles with index: " << iPointIndex << std::endl;
        createTriangles(iMinimimumAngleTriangleIndex, iPointIndex);
        if (count == 2){break;}
        ++count;
    }

    std::cout << "All Triangles are near equilateral" << std::endl;
}

int Mesh::locateSmallestAngle()
{
    double dMinAngle = 180;
    int iMinAngleTriangleIndex = -1;

    for (int i = 0; i < vecTriangles.size(); ++i)
    {
        const Triangle& triangle = vecTriangles[i];

        for (int j = 0; j < 3; ++j)
        {
            if (triangle.getAng(j) < dMinAngle)
            {
                dMinAngle = triangle.getAng(j);
                iMinAngleTriangleIndex = triangle.getIndex();
            }
        }
    }



    if (dMinAngle < 40)
    {
        std::cout << "The smallest angle in the mesh of triangles is: " << dMinAngle << std::endl;
        std::cout << "The index of that triangle is: " << iMinAngleTriangleIndex << std::endl;
        std::cout << std::endl;
        return iMinAngleTriangleIndex;
    }
    else
    {
        std::cout << "Delauny Triangulation satisfied"<< std::endl;
        return -1;
    }
}
