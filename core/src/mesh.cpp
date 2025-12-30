#include "mesh.h"
#include <iostream>
#include <random>
#include <stack>
#include <vector>
#include <queue>
#include <algorithm>
#include "constants.h"

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

// Sets the shape of the mesh with a given vector of points
void Mesh::setPtVector(const std::vector<Point>& vecPt)
{
    m_vecPoints = vecPt;
}

void Mesh::addPoint(const Point &pt)
{
    m_vecPoints.push_back(pt);
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

    for (const auto& point : m_vecPoints)
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
    using namespace delaunay;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, m_vecTriangles.size() - 1);

    int iCurrentIndex = dis(gen);

    while (true)
    {
        const Triangle& triCurrent = m_vecTriangles[iCurrentIndex];
        
        // Check if this triangle contains the point
        if (triCurrent.contains(ptTargetPoint))
        {
            return triCurrent.index();
        }
        
        // Get neighbor toward the point
        auto next = triCurrent.neighborToward(ptTargetPoint);
        if (!next || *next == NO_NEIGHBOR)
        {
            break;  // Dead end
        }
        
        iCurrentIndex = *next;
    }

    return NO_NEIGHBOR;
}


// Creates a super triangle that encloses all points in the mesh
Triangle Mesh::superTriangle()
{
    using namespace delaunay;
    
    Point p0(-1000000.0, -1000000.0);
    Point p1( 2000000.0, -1000000.0);  
    Point p2( 500000.0,  3000000.0);

    Triangle triSuper{p0, p1, p2};
    triSuper.setPointIndex(0, SUPER_VERTEX_0);
    triSuper.setPointIndex(1, SUPER_VERTEX_1);
    triSuper.setPointIndex(2, SUPER_VERTEX_2);
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
    using namespace delaunay;
    
    const Point& pt = m_vecPoints[iPointIndex];
    Triangle& tri = m_vecTriangles[iTriangleIndex];
    
    int edge = tri.onEdge(pt);
    if (edge < 0) return;  // Not on any edge
    
    // Compute indices using modular arithmetic
    int i0 = edge;
    int i1 = (edge + 1) % 3;
    int i2 = (edge + 2) % 3;
    
    // Create new triangle from the two points NOT on the split edge, plus new point
    Triangle triNew(tri.point(i1), tri.point(i2), pt);
    
    int iNewIndex = m_vecTriangles.size();
    triNew.setIndex(iNewIndex);
    
    // Set point indices
    triNew.setPointIndex(0, tri.pointIndex(i1));
    triNew.setPointIndex(1, tri.pointIndex(i2));
    triNew.setPointIndex(2, iPointIndex);
    
    // Set neighbor relationships
    triNew.setNeighbourIndex(0, tri.neighbourIndex(i1));
    triNew.setNeighbourIndex(1, iTriangleIndex);  // Points back to original
    
    // Update old neighbor to point to new triangle
    int oldNeighbour = tri.neighbourIndex(i1);
    if (oldNeighbour != NO_NEIGHBOR)
    {
        updateNeighbourReference(oldNeighbour, iTriangleIndex, iNewIndex);
    }
    
    // Update original triangle
    int oppositeNeighbour = tri.neighbourIndex(i0);
    tri.setPointIndex(i1, iPointIndex);
    tri.setPoint(i1, pt);
    tri.setNeighbourIndex(i1, iNewIndex);
    tri.setNeighbourIndex(i0, NO_NEIGHBOR);
    
    m_vecTriangles.push_back(triNew);
    
    // Handle opposite side if there's a neighbor
    if (oppositeNeighbour != NO_NEIGHBOR)
    {
        createTrianglesOppositeSide(oppositeNeighbour, iPointIndex, iTriangleIndex, iNewIndex);
    }
    
    restoreDelaunay(iPointIndex);
}

// Creates new triangles on the opposite side of a specified triangle and point.
void Mesh::createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{
    using namespace delaunay;
    
    const Point& pt = m_vecPoints[iPointIndex];
    Triangle& tri = m_vecTriangles[iTriangleIndex];
    
    int edge = tri.onEdge(pt);
    if (edge < 0) return;
    
    // Compute indices using modular arithmetic
    int i0 = edge;
    int i1 = (edge + 1) % 3;
    int i2 = (edge + 2) % 3;
    
    // Create new triangle
    Triangle triNew(tri.point(i1), tri.point(i2), pt);
    int iNewIndex = m_vecTriangles.size();
    triNew.setIndex(iNewIndex);
    
    // Set point indices
    triNew.setPointIndex(0, tri.pointIndex(i1));
    triNew.setPointIndex(1, tri.pointIndex(i2));
    triNew.setPointIndex(2, iPointIndex);
    
    // Set neighbor relationships for new triangle
    triNew.setNeighbourIndex(0, tri.neighbourIndex(i1));
    triNew.setNeighbourIndex(1, iTriangleIndex);
    
    // Determine which original neighbor connects to new vs current triangle
    if (areNeighbours(iTriangleIndex, iNeighbourIndex0))
    {
        triNew.setNeighbourIndex(2, iNeighbourIndex1);
        tri.setNeighbourIndex(i0, iNeighbourIndex0);
    }
    else
    {
        triNew.setNeighbourIndex(2, iNeighbourIndex0);
        tri.setNeighbourIndex(i0, iNeighbourIndex1);
    }
    
    // Update old neighbor to point to new triangle
    int oldNeighbour = tri.neighbourIndex(i1);
    if (oldNeighbour != NO_NEIGHBOR)
    {
        updateNeighbourReference(oldNeighbour, iTriangleIndex, iNewIndex);
    }
    
    // Update original triangle with new point
    tri.setPointIndex(i1, iPointIndex);
    tri.setPoint(i1, pt);
    tri.setNeighbourIndex(i1, iNewIndex);
    
    m_vecTriangles.push_back(triNew);
    
    // Update edge neighbours
    updateEdgeNeighbours(iTriangleIndex, iNewIndex, iNeighbourIndex0, iNeighbourIndex1);
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
        neighbourQueue.pop();

        int iNeighbourIndex = neighbourQueue.front();
        Triangle& triNeighbour = m_vecTriangles[iNeighbourIndex];

        // Find which neighbor contains the point
        auto next = triNeighbour.neighborToward(ptTargetPoint);
        if (iNeighbourIndex >= 0 && next)
        {
            swapEdge(*next, iNeighbourIndex);
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
        {"triangles", m.triangles()},
        {"points", m.points()}
    };
}

void Mesh::removeHelperTriangles()
{
    using namespace delaunay;
    
    auto isHelperTriangle = [](const Triangle& tri) {
        for (size_t i = 0; i < 3; ++i) {
            if (isSuperVertex(tri.pointIndex(i))) return true;
        }
        return false;
    };
    
    for (auto& tri : m_vecTriangles) {
        if (isHelperTriangle(tri)) {
            updateRemovedNeighbours(tri.index());
        }
    }
    
    std::erase_if(m_vecTriangles, isHelperTriangle);
    
    updateTriangleIndicesAfterRemoval();
}

// Updates triangle indices after some triangles have been removed.
void Mesh::updateTriangleIndicesAfterRemoval()
{
    using namespace delaunay;
    
    // Build old index -> new index mapping
    std::unordered_map<int, int> indexMap;
    for (size_t i = 0; i < m_vecTriangles.size(); ++i)
    {
        int oldIndex = m_vecTriangles[i].index();
        indexMap[oldIndex] = static_cast<int>(i);
        m_vecTriangles[i].setIndex(static_cast<int>(i));
    }
    
    // Update all neighbour references using the map
    for (auto& tri : m_vecTriangles)
    {
        for (size_t edge = 0; edge < 3; ++edge)
        {
            int oldNeighbourIdx = tri.neighbourIndex(edge);
            if (oldNeighbourIdx == NO_NEIGHBOR) continue;
            
            auto it = indexMap.find(oldNeighbourIdx);
            int newIdx = (it != indexMap.end()) ? it->second : NO_NEIGHBOR;
            tri.setNeighbourIndex(edge, newIdx);
        }
    }
}

// Updates the neighbors of a triangle that has been removed.
void Mesh::updateRemovedNeighbours(int iRemovedTriangleIndex)
{
    using namespace delaunay;
    
    const Triangle& triRemoved = m_vecTriangles[iRemovedTriangleIndex];

    for (size_t i = 0; i < 3; ++i)
    {
        int neighbourIdx = triRemoved.neighbourIndex(i);
        if (neighbourIdx == NO_NEIGHBOR) continue;

        // Clear the back-reference from neighbour to removed triangle
        updateNeighbourReference(neighbourIdx, iRemovedTriangleIndex, NO_NEIGHBOR);
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

void Mesh::updateNeighbourReference(int neighbourIdx, int oldRef, int newRef)
{
    if (neighbourIdx == delaunay::NO_NEIGHBOR) return;
    
    Triangle& neighbour = m_vecTriangles[neighbourIdx];
    for (size_t i = 0; i < 3; ++i)
    {
        if (neighbour.neighbourIndex(i) == oldRef)
        {
            neighbour.setNeighbourIndex(i, newRef);
            return;
        }
    }
}