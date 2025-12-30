#include "mesh.h"
#include <iostream>
#include <random>
#include <stack>
#include <vector>
#include <queue>
#include <algorithm>

// Constructor: Creates empty mesh object
Mesh::Mesh() {
    setPtVector({});
    setTriVector({ superTriangle() });
}

// Constructor: Creates the mesh with a given set of points
Mesh::Mesh(const std::vector<Point>& vecPt)
{
    setPtVector(vecPt);
    setTriVector({ superTriangle()});
}

// Returns the shape of the mesh as a vector of points
std::vector<Point> Mesh::getPtVector() const
{
    return m_vecPoints;
}

// Sets the shape of the mesh with a given vector of points
void Mesh::setPtVector(const std::vector<Point>& vecPt)
{
    m_vecPoints = vecPt;
}

void Mesh::addPoint(const Point &pt)
{
    m_vecPoints.push_back(pt);
}

// Returns the triangle vector
std::vector<Triangle> Mesh::getTriVector() const
{
    return m_vecTriangles;
}

// Sets the triangle vector
void Mesh::setTriVector(const std::vector<Triangle>& vecTri)
{
    m_vecTriangles = vecTri;
}

void Mesh::triangulatePoint(double fx, double fy)
{
    int iPointIndex = m_vecPoints.size();
    Point target_point = {fx, fy};
    addPoint(target_point);
    int iTriIndex = findContainingTriangle(target_point);
    createTriangles(iTriIndex, iPointIndex);
}

// Builds the mesh by iterating through all points in vecPtShape.
void Mesh::buildMesh()
{
    int iPointIndex = 0;

    for (auto point : m_vecPoints)
    {
        // Find the triangle that contains the current point
        int iTriIndex = findContainingTriangle(point);

        // Create new triangles from the found triangle and current point
        createTriangles(iTriIndex, iPointIndex);

        iPointIndex += 1;  // Move to the next point
    }
}

// Finds the index of the triangle containing the target point
int Mesh::findContainingTriangle(const Point& ptTargetPoint) const
{
    // random number generator
    static std::random_device rd;  // Seed
    static std::mt19937 gen(rd()); // Mersenne Twister RNG


    // Should adapt to size of updated vecTriangles
    std::uniform_int_distribution<> dis(0, m_vecTriangles.size() - 1);

    // Get a randomized triangle from vecTriangles
    int iRandomIndex = dis(gen);

    // Initialize stack for DFS
    std::stack<int> stackTriangles;
    stackTriangles.push(iRandomIndex);

    while (!stackTriangles.empty())
    {
        int iCurrentIndex = stackTriangles.top();
        stackTriangles.pop();

        const Triangle& triCurrent = m_vecTriangles[iCurrentIndex];
        int iResult = triCurrent.findPathToContainingTriangle(ptTargetPoint);

        if (iResult == -1) break; // indicates triangle not found
        if (iResult == -2) // -2 indicates that the currentTri contains ptTargetPoint
        {
            return triCurrent.index();
        }
        else
        {
            stackTriangles.push(iResult);
        }
    }

    return -1;
}

// Creates a super triangle that encloses all points in the mesh
Triangle Mesh::superTriangle()
{
    Point p0(-1000000.0f, -1000000.0f);
    Point p1( 2000000.0f, -1000000.0f);  
    Point p2( 500000.0f,  3000000.0f);

    Triangle triSuper {p0, p1, p2};
    triSuper.setPointIndex(0, -10);
    triSuper.setPointIndex(1, -11);
    triSuper.setPointIndex(2, -12);

    triSuper.setIndex(0);

    return triSuper;
}

// Creates new triangles by splitting an existing triangle based on the provided point index.
void Mesh::createTriangles(int iTriangleIndex, int iPointIndex)
{
    // Validate indices to ensure they are within bounds
    if (iTriangleIndex < 0 || iTriangleIndex >= m_vecTriangles.size() || iPointIndex < 0 || iPointIndex >= m_vecPoints.size()) {
        std::cerr << "Invalid indices provided to createTriangles." << std::endl;
        return;
    }

    // Reference to the target point in vecPtShape
    const Point& ptTargetPoint = m_vecPoints[iPointIndex];
    // Reference to the triangle being split
    Triangle& triCurrent = m_vecTriangles[iTriangleIndex];

    if(!triCurrent.contains(ptTargetPoint)) {
        std::cerr << "Target point provided is outside the bounds of the triangle. " << std::endl;
        return;
    }
    // Check if the target point lies inside the triangle but not on any edge
    if (triCurrent.onEdge(ptTargetPoint) < 0)
    {
        // Create two new triangles using two points from the current triangle and the target point
        Triangle triNewTriangle1(triCurrent.point(0), triCurrent.point(1), ptTargetPoint);
        Triangle triNewTriangle2(triCurrent.point(1), triCurrent.point(2), ptTargetPoint);

        // Set the indices for the new triangles
        int iNewIndex1 = m_vecTriangles.size();
        int iNewIndex2 = iNewIndex1 + 1;
        triNewTriangle1.setIndex(iNewIndex1);
        triNewTriangle2.setIndex(iNewIndex2);

        // Set the point indices for the new triangles
        triNewTriangle1.setPointIndex(0, triCurrent.pointIndex(0));
        triNewTriangle1.setPointIndex(1, triCurrent.pointIndex(1));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        triNewTriangle2.setPointIndex(0, triCurrent.pointIndex(1));
        triNewTriangle2.setPointIndex(1, triCurrent.pointIndex(2));
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
        int iOldNeighbourIndex1 = triCurrent.neighbourIndex(0);
        int iOldNeighbourIndex2 = triCurrent.neighbourIndex(1);

        // Update the current triangle's neighbors to point to the new triangles
        triCurrent.setNeighbourIndex(0, iNewIndex1);
        triCurrent.setNeighbourIndex(1, iNewIndex2);

        // Update the old neighbor of the current triangle to point to the new triangle
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
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
            Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex2];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
                {
                    triOldNeighbour.setNeighbourIndex(i, iNewIndex2);
                    break;
                }
            }
            triNewTriangle2.setNeighbourIndex(0, iOldNeighbourIndex2);
        }

        // Add the new triangles to the mesh's triangle list
        m_vecTriangles.push_back(triNewTriangle1);
        m_vecTriangles.push_back(triNewTriangle2);

        restoreDelaunay(iPointIndex);
    }
    else
    {
        // Handle cases where the target point lies on an edge of the current triangle
        handleEdgeCase(iTriangleIndex, iPointIndex);
    }
}

// Handle cases where the target point lies on an edge of the current triangle
void Mesh::handleEdgeCase(int iTriangleIndex, int iPointIndex)
{
    {
        // Reference to the point being processed and the triangle that is being split
        const Point& ptTargetPoint = m_vecPoints[iPointIndex];
        Triangle& triCurrent = m_vecTriangles[iTriangleIndex];

        // Create a new triangle that will be used to split the current triangle
        Triangle triNewTriangle1;
        int iNewIndex1 = m_vecTriangles.size();
        triNewTriangle1.setIndex(iNewIndex1);

        // Handle case where the point is on Edge 0 (pt0 to pt1)
        if(triCurrent.onEdge(ptTargetPoint) == 0)
        {
            // Set points for the new triangle and update indices
            triNewTriangle1.setPoint(0, triCurrent.point(1));
            triNewTriangle1.setPoint(1, triCurrent.point(2));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.pointIndex(1));
            triNewTriangle1.setPointIndex(1, triCurrent.pointIndex(2));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            // Set neighbor relationships for the new triangle
            triNewTriangle1.setNeighbourIndex(1, iTriangleIndex);
            triNewTriangle1.setNeighbourIndex(0, triCurrent.neighbourIndex(1));

            int iOldNeighbourIndex1 = triCurrent.neighbourIndex(1);

            // Update the old neighbor to reference the new triangle
            if (iOldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex1];
                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
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

            const int iOldNeighbour = triCurrent.neighbourIndex(0);
            triCurrent.setNeighbourIndex(0, -1);

            // Add the new triangle to the list of triangles
            m_vecTriangles.push_back(triNewTriangle1);

            // Handle the opposite side of the triangle if necessary
            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }

            restoreDelaunay(iPointIndex);
        }

        // Handle case where the point is on Edge 1 (pt1 to pt2)
        else if (triCurrent.onEdge(ptTargetPoint) == 1)
        {
            // Set points for the new triangle and update indices
            triNewTriangle1.setPoint(0, triCurrent.point(0));
            triNewTriangle1.setPoint(1, triCurrent.point(1));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.pointIndex(0));
            triNewTriangle1.setPointIndex(1, triCurrent.pointIndex(1));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            // Set neighbor relationships for the new triangle
            triNewTriangle1.setNeighbourIndex(0, triCurrent.neighbourIndex(0));
            triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

            int iOldNeighbourIndex1 = triCurrent.neighbourIndex(0);

            // Update the old neighbor to reference the new triangle
            if (iOldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex1];
                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
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

            const int iOldNeighbour = triCurrent.neighbourIndex(1);
            triCurrent.setNeighbourIndex(1, -1);

            // Add the new triangle to the list of triangles
            m_vecTriangles.push_back(triNewTriangle1);

            // Handle the opposite side of the triangle if necessary
            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }

            restoreDelaunay(iPointIndex);
        }

        // Handle case where the point is on Edge 2 (pt2 to pt0)
        else
        {
            // Set points for the new triangle and update indices
            triNewTriangle1.setPoint(0, triCurrent.point(1));
            triNewTriangle1.setPoint(1, triCurrent.point(2));
            triNewTriangle1.setPoint(2, ptTargetPoint);

            triNewTriangle1.setPointIndex(0, triCurrent.pointIndex(1));
            triNewTriangle1.setPointIndex(1, triCurrent.pointIndex(2));
            triNewTriangle1.setPointIndex(2, iPointIndex);

            // Set neighbor relationships for the new triangle
            triNewTriangle1.setNeighbourIndex(0, triCurrent.neighbourIndex(1));
            triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

            int iOldNeighbourIndex1 = triCurrent.neighbourIndex(1);

            // Update the old neighbor to reference the new triangle
            if (iOldNeighbourIndex1 != -1)
            {
                Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex1];
                for (int i = 0; i < 3; ++i)
                {
                    if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
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

            const int iOldNeighbour = triCurrent.neighbourIndex(2);
            triCurrent.setNeighbourIndex(2, -1);

            // Add the new triangle to the list of triangles
            m_vecTriangles.push_back(triNewTriangle1);

            // Handle the opposite side of the triangle if necessary
            if (iOldNeighbour > -1)
            {
                createTrianglesOppositeSide(iOldNeighbour, iPointIndex, iTriangleIndex, iNewIndex1);
            }

            restoreDelaunay(iPointIndex);
        }
    }
}

// Creates new triangles on the opposite side of a specified triangle and point.
void Mesh::createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{
    // Retrieve the point that will be used to create new triangles
    const Point& ptTargetPoint = m_vecPoints[iPointIndex];

    // Get the current triangle that will be modified
    Triangle& triCurrent = m_vecTriangles[iTriangleIndex];

    // Create a new triangle to be added to the mesh
    Triangle triNewTriangle1;
    int iNewIndex1 = m_vecTriangles.size();
    triNewTriangle1.setIndex(iNewIndex1);

    // Handle the case where the target point is on Edge 0 (between points 1 and 2)
    if (triCurrent.onEdge(ptTargetPoint) == 0)
    {
        // Define the points of the new triangle
        triNewTriangle1.setPoint(0, triCurrent.point(1));
        triNewTriangle1.setPoint(1, triCurrent.point(2));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        // Set the point indices for the new triangle
        triNewTriangle1.setPointIndex(0, triCurrent.pointIndex(1));
        triNewTriangle1.setPointIndex(1, triCurrent.pointIndex(2));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        // Set the neighbors of the new triangle
        triNewTriangle1.setNeighbourIndex(1, iTriangleIndex);
        triNewTriangle1.setNeighbourIndex(0, triCurrent.neighbourIndex(1));

        // Update the neighbor index of the old neighbor
        int iOldNeighbourIndex1 = triCurrent.neighbourIndex(1);
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
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
        m_vecTriangles.push_back(triNewTriangle1);

        // Update the reference to the current triangle and perform edge neighbor updates
        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);

    }
    // Handle the case where the target point is on Edge 1 (between points 0 and 1)
    else if (triCurrent.onEdge(ptTargetPoint) == 1)
    {
        // Define the points of the new triangle
        triNewTriangle1.setPoint(0, triCurrent.point(0));
        triNewTriangle1.setPoint(1, triCurrent.point(1));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        // Set the point indices for the new triangle
        triNewTriangle1.setPointIndex(0, triCurrent.pointIndex(0));
        triNewTriangle1.setPointIndex(1, triCurrent.pointIndex(1));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        // Set the neighbors of the new triangle
        triNewTriangle1.setNeighbourIndex(0, triCurrent.neighbourIndex(0));
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

        // Update the neighbor index of the old neighbor
        int iOldNeighbourIndex1 = triCurrent.neighbourIndex(0);
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
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
        m_vecTriangles.push_back(triNewTriangle1);

        // Update the reference to the current triangle and perform edge neighbor updates
        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);
    }
    // Handle the case where the target point is on Edge 2 (between points 1 and 2)
    else
    {
        // Define the points of the new triangle
        triNewTriangle1.setPoint(0, triCurrent.point(1));
        triNewTriangle1.setPoint(1, triCurrent.point(2));
        triNewTriangle1.setPoint(2, ptTargetPoint);

        // Set the point indices for the new triangle
        triNewTriangle1.setPointIndex(0, triCurrent.pointIndex(1));
        triNewTriangle1.setPointIndex(1, triCurrent.pointIndex(2));
        triNewTriangle1.setPointIndex(2, iPointIndex);

        // Set the neighbors of the new triangle
        triNewTriangle1.setNeighbourIndex(0, triCurrent.neighbourIndex(1));
        triNewTriangle1.setNeighbourIndex(2, iTriangleIndex);

        // Update the neighbor index of the old neighbor
        int iOldNeighbourIndex1 = triCurrent.neighbourIndex(1);
        if (iOldNeighbourIndex1 != -1)
        {
            Triangle& triOldNeighbour = m_vecTriangles[iOldNeighbourIndex1];
            for (int i = 0; i < 3; ++i)
            {
                if (triOldNeighbour.neighbourIndex(i) == iTriangleIndex)
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
        m_vecTriangles.push_back(triNewTriangle1);

        // Update the reference to the current triangle and perform edge neighbor updates
        updateEdgeNeighbours(iTriangleIndex, iNewIndex1, iNeighbourIndex0, iNeighbourIndex1);
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
    Triangle& triCurrent = m_vecTriangles[iTriangleIndex];
    Triangle& triNewTriangle = m_vecTriangles[iNewTriangleIndex];

    Triangle& triOldNeighbour0 = m_vecTriangles[iNeighbourIndex0];
    Triangle& triOldNeighbour1 = m_vecTriangles[iNeighbourIndex1];

    int iEdgeIndex0, iEdgeIndex1;

    // Find the edge indices of the current and new triangles that correspond to the neighbor indices
    for (int i = 0; i < 3; ++i)
    {
        if (triNewTriangle.neighbourIndex(i) == iNeighbourIndex0)
        {
            iEdgeIndex0 = i;
        }

        if (triCurrent.neighbourIndex(i) == iNeighbourIndex1)
        {
            iEdgeIndex1 = i;
        }
        else
        {
            continue;
        }
    }

    // Calculate the midpoints of the edges that connect the new triangle and current triangle
    Point ptMid0 = triNewTriangle.edgeMidpoint(iEdgeIndex0);
    Point ptMid1 = triCurrent.edgeMidpoint(iEdgeIndex1);

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
    const Triangle& tri1 = m_vecTriangles[iTri1];
    const Triangle& tri2 = m_vecTriangles[iTri2];

    int iSharedCount = 0;  // Counter for shared points

    // Identify shared points between the two triangles
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int pointCurrent = tri1.pointIndex(i);
            int pointNeighbour = tri2.pointIndex(j);

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
    std::vector<bool> visited(m_vecTriangles.size(), false);

    const Point& ptTargetPoint = m_vecPoints[iPointIndex];
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
            Triangle& triCurrent = m_vecTriangles[iCurrentIndex];

            // Check only specified edge for the first iteration
            if (firstIteration)
            {
                int iNeighbourIndex = triCurrent.neighbourIndex(iEdgeIndex);

                if (iNeighbourIndex >= 0 && !visited[iNeighbourIndex])
                {
                    Triangle& triNeighbour = m_vecTriangles[iNeighbourIndex];

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
                    int iNeighbourIndex = triCurrent.neighbourIndex(i);

                    if (iNeighbourIndex >= 0 && !visited[iNeighbourIndex])
                    {
                        Triangle& triNeighbour = m_vecTriangles[iNeighbourIndex];

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
    Triangle super = superTriangle();
    // Retrieve the triangles based on their indices
    Triangle& triCurrent = m_vecTriangles[iTri1];
    Triangle& triNeighbour = m_vecTriangles[iTri2];

    int diff1 = -1, diff2 = -1;
    std::vector<int> shared;

    // Identify shared points between the two triangles
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int pointCurrent = triCurrent.pointIndex(i);
            int pointNeighbour = triNeighbour.pointIndex(j);

            if (pointCurrent == pointNeighbour)
            {
                shared.push_back(pointCurrent); // Store shared points
            }
        }
    }

    // Identify the different points in each triangle
    for (int i = 0; i < 3; ++i)
    {
        int pointCurrent = triCurrent.pointIndex(i);
        int pointNeighbour = triNeighbour.pointIndex(i);

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
    const int iCurrentN0 = triCurrent.neighbourIndex(0);
    const int iCurrentN1 = triCurrent.neighbourIndex(1);
    const int iCurrentN2 = triCurrent.neighbourIndex(2);

    const int iNeighbourN0 = triNeighbour.neighbourIndex(0);
    const int iNeighbourN1 = triNeighbour.neighbourIndex(1);
    const int iNeighbourN2 = triNeighbour.neighbourIndex(2);

    const int iCurrentPt0 = triCurrent.pointIndex(0);
    const int iCurrentPt1 = triCurrent.pointIndex(1);
    const int iCurrentPt2 = triCurrent.pointIndex(2);

    // Replace one of the shared points in triCurrent with diff2
    for (int i = 0; i < 3; ++i)
    {
        if (triCurrent.pointIndex(i) == shared[0])
        {
            triCurrent.setPointIndex(i, diff2);

            if(diff2 > -1) {
                triCurrent.setPoint(i, m_vecPoints[diff2]);
            } else {
                triCurrent.setPoint(i, super.point(abs(diff2) - 10));
            }

            // Determine the shared edge and new edge after swap
            int iSharedEdge = findSharedEdge(triCurrent, diff1, diff2);
            int iNewEdge = findNewEdge(i, iSharedEdge);

            int iStolenEdge = -1;
            if ((triNeighbour.pointIndex(0) == shared[1] && triNeighbour.pointIndex(1) == diff2) ||
                (triNeighbour.pointIndex(1) == shared[1] && triNeighbour.pointIndex(0) == diff2))
            {
                iStolenEdge = iNeighbourN0;
            }
            else if ((triNeighbour.pointIndex(1) == shared[1] && triNeighbour.pointIndex(2) == diff2) ||
                     (triNeighbour.pointIndex(2) == shared[1] && triNeighbour.pointIndex(1) == diff2))
            {
                iStolenEdge = iNeighbourN1;
            }
            else if ((triNeighbour.pointIndex(2) == shared[1] && triNeighbour.pointIndex(0) == diff2) ||
                     (triNeighbour.pointIndex(0) == shared[1] && triNeighbour.pointIndex(2) == diff2))
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
        if (triNeighbour.pointIndex(i) == shared[1])
        {
            triNeighbour.setPointIndex(i, diff1);
            if(diff1 > -1)
                triNeighbour.setPoint(i, m_vecPoints[diff1]);
            else {
                triNeighbour.setPoint(i, super.point(abs(diff1) - 10));
            }

            // Determine the shared edge and new edge after swap
            int iSharedEdge = findSharedEdge(triNeighbour, diff1, diff2);
            int iNewEdge = findNewEdge(i, iSharedEdge);

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
    Point& ptTargetPoint = m_vecPoints[iPointIndex];

    while (neighbourQueue.size() > 1)
    {
        // Remove the oldest triangle from the queue
        neighbourQueue.pop();

        // Get the index of the next triangle to process
        int iNeighbourIndex = neighbourQueue.front();
        Triangle& triNeighbour = m_vecTriangles[iNeighbourIndex];

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
        if (((tri.pointIndex(i) == iDiff1 && tri.pointIndex((i + 1) % 3) == iDiff2)) ||
            ((tri.pointIndex(i) == iDiff2 && tri.pointIndex((i + 1) % 3) == iDiff1)))
        {
            return i;
        }
    }
    return -1; // Return -1 if the edge is not found
}

// Determines the new edge index for a triangle after an edge swap.
int Mesh::findNewEdge(int i, int iSharedEdge) const
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
        Triangle& oldNeighbor = m_vecTriangles[oldNeighborIndex];
        const Triangle& triCurrent = m_vecTriangles[oldTriangleIndex];

        // Check if the old neighbor triangle needs to be updated
        bool needsUpdate = true;
        for (int i = 0; i < 3; ++i)
        {
            if (triCurrent.neighbourIndex(i) == oldNeighborIndex)
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
                if (oldNeighbor.neighbourIndex(i) == oldTriangleIndex)
                {
                    oldNeighbor.setNeighbourIndex(i, newTriangleIndex);
                    break;
                }
            }
        }
    }
}

void to_json(nlohmann::json &j, const Mesh &m)
{
    j = nlohmann::json {
        {"triangles", m.getTriVector()},
        {"points", m.getPtVector()}
    };
}

void Mesh::removeHelperTriangles()
{
    std::vector<int> trianglesToRemove;

    for (int i = 0; i < m_vecTriangles.size(); ++i)
    {
        const Triangle& triangle = m_vecTriangles[i];

        for (int j = 0; j < 3; ++j)
        {
            // Check for super triangle sentinel values
            if (triangle.pointIndex(j) <= -10)
            {
                trianglesToRemove.push_back(i);
                updateRemovedNeighbours(triangle.index());
                break;
            }
        }
    }

    for (int i = trianglesToRemove.size() - 1; i >= 0; --i)
    {
        m_vecTriangles.erase(m_vecTriangles.begin() + trianglesToRemove[i]);
    }

    // Remove the buggy resize line entirely
    
    updateTriangleIndicesAfterRemoval();
}

// Updates triangle indices after some triangles have been removed.
void Mesh::updateTriangleIndicesAfterRemoval()
{
    // Iterate through all remaining triangles
    for (int iTriangleIndex = 0; iTriangleIndex < m_vecTriangles.size(); ++iTriangleIndex)
    {
        Triangle& currentTriangle = m_vecTriangles[iTriangleIndex];
        int iOldIndex = currentTriangle.index();  // Get the current triangle index

        // Update the triangle's index if it has changed
        if (iOldIndex != iTriangleIndex)
        {
            currentTriangle.setIndex(iTriangleIndex);

            // Update the neighbors of the current triangle to reference the new index
            for (int i = 0; i < 3; ++i)
            {
                int iTriangleNeighbourIndex = currentTriangle.neighbourIndex(i);
                if (iTriangleNeighbourIndex == -1) continue;  // Skip if no neighbor

                // Iterate through all triangles to find and update the neighbor's references
                for (Triangle& triNeighbour : m_vecTriangles)
                {
                    if (triNeighbour.index() == iTriangleNeighbourIndex)
                    {
                        // Update the specific neighbor reference to the new index
                        for (int j = 0; j < 3; ++j)
                        {
                            if (triNeighbour.neighbourIndex(j) == iOldIndex)
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
    const Triangle& triRemoved = m_vecTriangles[iRemovedTriangleIndex];  // Get the removed triangle

    // Iterate through each edge of the removed triangle
    for (int i = 0; i < 3; ++i)
    {
        int removedNeighborIndex = triRemoved.neighbourIndex(i);
        if (removedNeighborIndex == -1) continue;  // Skip if no neighbor

        Triangle& triNeighbour = m_vecTriangles[removedNeighborIndex];

        // Update the neighbor reference to indicate it no longer has a neighbor on this edge
        for (int j = 0; j < 3; ++j)
        {
            if (triNeighbour.neighbourIndex(j) == iRemovedTriangleIndex)
            {
                triNeighbour.setNeighbourIndex(j, -1);  // Mark as no neighbor
            }
        }
    }
}

void Mesh::restoreDelaunay(int iPointIndex)
{
    const Point& pt = m_vecPoints[iPointIndex];
    
    bool changed = true;
    int maxIterations = m_vecTriangles.size() * 3;
    int iterations = 0;
    
    while (changed && iterations < maxIterations) {
        changed = false;
        iterations++;
        
        for (size_t t = 0; t < m_vecTriangles.size() && !changed; t++) {
            Triangle& tri = m_vecTriangles[t];
            
            bool hasNewPoint = (tri.pointIndex(0) == iPointIndex ||
                               tri.pointIndex(1) == iPointIndex ||
                               tri.pointIndex(2) == iPointIndex);
            if (!hasNewPoint) continue;
            
            for (int edge = 0; edge < 3 && !changed; edge++) {
                // Find the ring edge - the edge OPPOSITE to point P
                // Skip spoke edges (edges that connect to P)
                int p0 = tri.pointIndex(edge);
                int p1 = tri.pointIndex((edge + 1) % 3);
                
                if (p0 == iPointIndex || p1 == iPointIndex) {
                    continue;  // This is a spoke edge, skip it
                }
                
                // This is the ring edge
                int neighborIdx = tri.neighbourIndex(edge);
                if (neighborIdx < 0) continue;
                
                if (m_vecTriangles[neighborIdx].isInCircumcircle(pt)) {
                    swapEdge(t, neighborIdx);
                    changed = true;
                    break;
                }
            }
        }
    }
}