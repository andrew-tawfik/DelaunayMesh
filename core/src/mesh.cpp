#include "mesh.h"
#include <iostream>
#include <random>
#include <stack>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include "constants.h"

using namespace delaunay;
namespace {
    // Lookup table for findNewEdge: [i][iSharedEdge] -> newEdge
    constexpr int NEW_EDGE_TABLE[3][3] = {
        {2, -1, 0},   // i=0: sharedEdge 0->2, 2->0
        {1, 0, -1},   // i=1: sharedEdge 0->1, 1->0
        {-1, 2, 1}    // i=2: sharedEdge 1->2, 2->1
    };

    // Super triangle points (created once, reused)
    constexpr Point SUPER_P0(-1000000.0, -1000000.0);
    constexpr Point SUPER_P1( 2000000.0, -1000000.0);
    constexpr Point SUPER_P2( 500000.0,  3000000.0);
}


Mesh::Mesh()
{
    setPtVector({});
    setTriVector({ superTriangle() });
}

Mesh::Mesh(const std::vector<Point>& vecPt)
{
    setPtVector(vecPt);
    setTriVector({ superTriangle() });
}


void Mesh::setPtVector(const std::vector<Point>& vecPt)
{
    m_vecPoints = vecPt;
}

void Mesh::addPoint(const Point& pt)
{
    m_vecPoints.push_back(pt);
}

void Mesh::setTriVector(const std::vector<Triangle>& vecTri)
{
    m_vecTriangles = vecTri;
}

void Mesh::triangulatePoint(double x, double y)
{    
    const int iPointIndex = static_cast<int>(m_vecPoints.size());
    const Point targetPoint(x, y);
    addPoint(targetPoint);
    
    const int iTriIndex = findContainingTriangle(targetPoint);
    if (iTriIndex == NO_NEIGHBOR) {
        throw std::runtime_error("triangulatePoint: no containing triangle found");
    }
    
    createTriangles(iTriIndex, iPointIndex);
}

void Mesh::buildMesh()
{
    int iPointIndex = 0;

    for (const auto& point : m_vecPoints)
    {
        int iTriIndex = findContainingTriangle(point);
        createTriangles(iTriIndex, iPointIndex);
        ++iPointIndex;
    }
}

int Mesh::findContainingTriangle(const Point& pt) const
{    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(m_vecTriangles.size()) - 1);

    int iCurrentIndex = dis(gen);

    while (true)
    {
        const Triangle& tri = m_vecTriangles[iCurrentIndex];
        
        int next = tri.findPathToward(pt);
        
        if (next == POINT_INSIDE) {  // -2: definitively inside
            return tri.index();
        }
        
        if (next == NO_NEIGHBOR) {  // -1: hit boundary, shouldn't happen
            break;
        }
        
        iCurrentIndex = next;
    }

    return NO_NEIGHBOR;
}

Triangle Mesh::superTriangle()
{    
    Triangle triSuper(SUPER_P0, SUPER_P1, SUPER_P2);
    triSuper.setPointIndex(0, SUPER_VERTEX_0);
    triSuper.setPointIndex(1, SUPER_VERTEX_1);
    triSuper.setPointIndex(2, SUPER_VERTEX_2);
    triSuper.setIndex(0);

    return triSuper;
}

Point Mesh::superTrianglePoint(int superVertexIndex)
{    
    int localIndex = std::abs(superVertexIndex) - std::abs(SUPER_VERTEX_0);
    switch (localIndex) {
        case 0: return SUPER_P0;
        case 1: return SUPER_P1;
        case 2: return SUPER_P2;
        default:
            throw std::out_of_range("Invalid super vertex index");
    }
}

void Mesh::createTriangles(int iTriangleIndex, int iPointIndex)
{    
    if (iTriangleIndex < 0 || static_cast<size_t>(iTriangleIndex) >= m_vecTriangles.size()) {
        throw std::out_of_range(
            "createTriangles: triangle index " + std::to_string(iTriangleIndex) + 
            " out of range [0, " + std::to_string(m_vecTriangles.size()) + ")"
        );
    }
    
    if (iPointIndex < 0 || static_cast<size_t>(iPointIndex) >= m_vecPoints.size()) {
        throw std::out_of_range(
            "createTriangles: point index " + std::to_string(iPointIndex) + 
            " out of range [0, " + std::to_string(m_vecPoints.size()) + ")"
        );
    }

    const Point& pt = m_vecPoints[iPointIndex];
    Triangle& triCurrent = m_vecTriangles[iTriangleIndex];

    if (!triCurrent.contains(pt)) {
        throw std::invalid_argument(
            "createTriangles: point (" + std::to_string(pt.x()) + ", " +
            std::to_string(pt.y()) + ") is outside triangle " +
            std::to_string(iTriangleIndex)
        );
    }

    int edge = triCurrent.onEdge(pt);
    if (edge < 0) {
        // Point is strictly inside the triangle
        createTrianglesInside(iTriangleIndex, iPointIndex);
    } else {
        // Point is on an edge
        handleEdgeCase(iTriangleIndex, iPointIndex);
    }
}

void Mesh::createTrianglesInside(int iTriangleIndex, int iPointIndex)
{    
    const Point& pt = m_vecPoints[iPointIndex];
    Triangle& triCurrent = m_vecTriangles[iTriangleIndex];

    // Create two new triangles
    Triangle triNew1(triCurrent.point(0), triCurrent.point(1), pt);
    Triangle triNew2(triCurrent.point(1), triCurrent.point(2), pt);

    const int iNewIndex1 = static_cast<int>(m_vecTriangles.size());
    const int iNewIndex2 = iNewIndex1 + 1;
    triNew1.setIndex(iNewIndex1);
    triNew2.setIndex(iNewIndex2);

    // Set point indices
    triNew1.setPointIndex(0, triCurrent.pointIndex(0));
    triNew1.setPointIndex(1, triCurrent.pointIndex(1));
    triNew1.setPointIndex(2, iPointIndex);

    triNew2.setPointIndex(0, triCurrent.pointIndex(1));
    triNew2.setPointIndex(1, triCurrent.pointIndex(2));
    triNew2.setPointIndex(2, iPointIndex);

    // Update current triangle
    triCurrent.setPointIndex(1, iPointIndex);
    triCurrent.setPoint(1, pt);

    // Set neighbor relationships
    triNew1.setNeighbourIndex(2, iTriangleIndex);
    triNew2.setNeighbourIndex(1, iTriangleIndex);
    triNew1.setNeighbourIndex(1, iNewIndex2);
    triNew2.setNeighbourIndex(2, iNewIndex1);

    // Save and update old neighbors
    const int iOldNeighbour1 = triCurrent.neighbourIndex(0);
    const int iOldNeighbour2 = triCurrent.neighbourIndex(1);

    triCurrent.setNeighbourIndex(0, iNewIndex1);
    triCurrent.setNeighbourIndex(1, iNewIndex2);

    if (iOldNeighbour1 != NO_NEIGHBOR) {
        updateNeighbourReference(iOldNeighbour1, iTriangleIndex, iNewIndex1);
        triNew1.setNeighbourIndex(0, iOldNeighbour1);
    }

    if (iOldNeighbour2 != NO_NEIGHBOR) {
        updateNeighbourReference(iOldNeighbour2, iTriangleIndex, iNewIndex2);
        triNew2.setNeighbourIndex(0, iOldNeighbour2);
    }

    m_vecTriangles.push_back(triNew1);
    m_vecTriangles.push_back(triNew2);

    restoreDelaunay(iPointIndex, {iTriangleIndex, iNewIndex1, iNewIndex2});
}

void Mesh::handleEdgeCase(int iTriangleIndex, int iPointIndex)
{    
    const Point& pt = m_vecPoints[iPointIndex];
    Triangle& tri = m_vecTriangles[iTriangleIndex];
    
    int edge = tri.onEdge(pt);
    if (edge < 0) return;
    
    // Compute indices using modular arithmetic
    const int i0 = edge;
    const int i1 = (edge + 1) % 3;
    const int i2 = (edge + 2) % 3;
    
    // Create new triangle
    Triangle triNew(tri.point(i1), tri.point(i2), pt);
    const int iNewIndex = static_cast<int>(m_vecTriangles.size());
    triNew.setIndex(iNewIndex);
    
    // Set point indices
    triNew.setPointIndex(0, tri.pointIndex(i1));
    triNew.setPointIndex(1, tri.pointIndex(i2));
    triNew.setPointIndex(2, iPointIndex);
    
    // Set neighbor relationships
    triNew.setNeighbourIndex(0, tri.neighbourIndex(i1));
    triNew.setNeighbourIndex(1, iTriangleIndex);
    
    // Update old neighbor
    const int oldNeighbour = tri.neighbourIndex(i1);
    if (oldNeighbour != NO_NEIGHBOR) {
        updateNeighbourReference(oldNeighbour, iTriangleIndex, iNewIndex);
    }
    
    // Update original triangle
    const int oppositeNeighbour = tri.neighbourIndex(i0);
    tri.setPointIndex(i1, iPointIndex);
    tri.setPoint(i1, pt);
    tri.setNeighbourIndex(i1, iNewIndex);
    tri.setNeighbourIndex(i0, NO_NEIGHBOR);
    
    m_vecTriangles.push_back(triNew);
    
    // Collect triangles for restoreDelaunay
    std::vector<int> initialTriangles = {iTriangleIndex, iNewIndex};
    
    // Handle opposite side if there's a neighbor
    if (oppositeNeighbour != NO_NEIGHBOR) {
        // This creates 2 more triangles
        const int beforeSize = static_cast<int>(m_vecTriangles.size());
        createTrianglesOppositeSide(oppositeNeighbour, iPointIndex, iTriangleIndex, iNewIndex);
        
        // Add the triangles created by createTrianglesOppositeSide
        initialTriangles.push_back(oppositeNeighbour);
        if (static_cast<int>(m_vecTriangles.size()) > beforeSize) {
            initialTriangles.push_back(beforeSize);  // The new triangle index
        }
    }
    
    restoreDelaunay(iPointIndex, initialTriangles);
}

void Mesh::createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{    
    const Point& pt = m_vecPoints[iPointIndex];
    Triangle& tri = m_vecTriangles[iTriangleIndex];
    
    const int edge = tri.onEdge(pt);
    if (edge < 0) return;
    
    const int i0 = edge;
    const int i1 = (edge + 1) % 3;
    const int i2 = (edge + 2) % 3;
    
    // Create new triangle
    Triangle triNew(tri.point(i1), tri.point(i2), pt);
    const int iNewIndex = static_cast<int>(m_vecTriangles.size());
    triNew.setIndex(iNewIndex);
    
    // Set point indices
    triNew.setPointIndex(0, tri.pointIndex(i1));
    triNew.setPointIndex(1, tri.pointIndex(i2));
    triNew.setPointIndex(2, iPointIndex);
    
    // Set neighbor relationships
    triNew.setNeighbourIndex(0, tri.neighbourIndex(i1));
    triNew.setNeighbourIndex(1, iTriangleIndex);
    
    // Determine which neighbor connects to new vs current
    if (areNeighbours(iTriangleIndex, iNeighbourIndex0)) {
        triNew.setNeighbourIndex(2, iNeighbourIndex1);
        tri.setNeighbourIndex(i0, iNeighbourIndex0);
    } else {
        triNew.setNeighbourIndex(2, iNeighbourIndex0);
        tri.setNeighbourIndex(i0, iNeighbourIndex1);
    }
    
    // Update old neighbor
    const int oldNeighbour = tri.neighbourIndex(i1);
    if (oldNeighbour != NO_NEIGHBOR) {
        updateNeighbourReference(oldNeighbour, iTriangleIndex, iNewIndex);
    }
    
    // Update original triangle
    tri.setPointIndex(i1, iPointIndex);
    tri.setPoint(i1, pt);
    tri.setNeighbourIndex(i1, iNewIndex);
    
    m_vecTriangles.push_back(triNew);
    
    updateEdgeNeighbours(iTriangleIndex, iNewIndex, iNeighbourIndex0, iNeighbourIndex1);
}

void Mesh::updateNeighbourReference(int neighbourIdx, int oldRef, int newRef) noexcept
{    
    if (neighbourIdx == NO_NEIGHBOR) return;
    
    Triangle& neighbour = m_vecTriangles[neighbourIdx];
    for (size_t i = 0; i < 3; ++i) {
        if (neighbour.neighbourIndex(i) == oldRef) {
            neighbour.setNeighbourIndex(i, newRef);
            return;
        }
    }
}

void Mesh::updateEdgeNeighbours(int iTriangleIndex, int iNewTriangleIndex, int iNeighbourIndex0, int iNeighbourIndex1)
{
    // Swap if necessary
    if (areNeighbours(iTriangleIndex, iNeighbourIndex0)) {
        std::swap(iNeighbourIndex0, iNeighbourIndex1);
    }

    Triangle& triCurrent = m_vecTriangles[iTriangleIndex];
    Triangle& triNew = m_vecTriangles[iNewTriangleIndex];
    Triangle& triOldNeighbour0 = m_vecTriangles[iNeighbourIndex0];
    Triangle& triOldNeighbour1 = m_vecTriangles[iNeighbourIndex1];

    int iEdgeIndex0 = -1;
    int iEdgeIndex1 = -1;

    // Find edge indices
    for (int i = 0; i < 3; ++i) {
        if (triNew.neighbourIndex(i) == iNeighbourIndex0) {
            iEdgeIndex0 = i;
        }
        if (triCurrent.neighbourIndex(i) == iNeighbourIndex1) {
            iEdgeIndex1 = i;
        }
    }

    // Validate
    if (iEdgeIndex0 < 0 || iEdgeIndex1 < 0) {
        throw std::runtime_error("updateEdgeNeighbours: could not find edge indices");
    }

    // Calculate midpoints and update
    Point ptMid0 = triNew.edgeMidpoint(iEdgeIndex0);
    Point ptMid1 = triCurrent.edgeMidpoint(iEdgeIndex1);

    int iTargetEdge0 = triOldNeighbour0.onEdge(ptMid0);
    int iTargetEdge1 = triOldNeighbour1.onEdge(ptMid1);

    if (iTargetEdge0 >= 0) {
        triOldNeighbour0.setNeighbourIndex(iTargetEdge0, iNewTriangleIndex);
    }
    if (iTargetEdge1 >= 0) {
        triOldNeighbour1.setNeighbourIndex(iTargetEdge1, iTriangleIndex);
    }
}

bool Mesh::areNeighbours(int iTri1, int iTri2) const noexcept
{
    const Triangle& tri1 = m_vecTriangles[iTri1];
    const Triangle& tri2 = m_vecTriangles[iTri2];

    int sharedCount = 0;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (tri1.pointIndex(i) == tri2.pointIndex(j)) {
                ++sharedCount;
            }
        }
    }
    return sharedCount == 2;
}

void Mesh::updateNeighboursAfterSwap(int oldNeighborIndex, int oldTriangleIndex, int newTriangleIndex) noexcept
{    
    if (oldNeighborIndex == NO_NEIGHBOR || 
        oldNeighborIndex == oldTriangleIndex || 
        oldNeighborIndex == newTriangleIndex) {
        return;
    }

    Triangle& oldNeighbor = m_vecTriangles[oldNeighborIndex];
    const Triangle& triCurrent = m_vecTriangles[oldTriangleIndex];

    // Check if update is needed
    bool needsUpdate = true;
    for (size_t i = 0; i < 3; ++i) {
        if (triCurrent.neighbourIndex(i) == oldNeighborIndex) {
            needsUpdate = false;
            break;
        }
    }

    if (needsUpdate) {
        for (size_t i = 0; i < 3; ++i) {
            if (oldNeighbor.neighbourIndex(i) == oldTriangleIndex) {
                oldNeighbor.setNeighbourIndex(i, newTriangleIndex);
                break;
            }
        }
    }
}

void Mesh::restoreDelaunay(int iPointIndex, const std::vector<int>& initialTriangles)
{    
    const Point& pt = m_vecPoints[iPointIndex];
    
    // Stack of edges to check: (triangleIndex, edgeIndex)
    std::vector<std::pair<int, int>> edgeStack;
    edgeStack.reserve(32);
    
    // Seed with edges opposite to new point in initial triangles
    for (const int triIdx : initialTriangles) {
        const Triangle& tri = m_vecTriangles[triIdx];
        for (int i = 0; i < 3; ++i) {
            if (tri.pointIndex(i) == iPointIndex) {
                const int oppositeEdge = (i + 1) % 3;
                if (tri.neighbourIndex(oppositeEdge) != NO_NEIGHBOR) {
                    edgeStack.emplace_back(triIdx, oppositeEdge);
                }
                break;
            }
        }
    }
    
    // Process until empty
    while (!edgeStack.empty()) {
        const auto [triIdx, edgeIdx] = edgeStack.back();
        edgeStack.pop_back();
        
        if (triIdx < 0 || triIdx >= static_cast<int>(m_vecTriangles.size())) {
            continue;
        }
        
        Triangle& tri = m_vecTriangles[triIdx];
        const int neighborIdx = tri.neighbourIndex(edgeIdx);
        
        if (neighborIdx == NO_NEIGHBOR) continue;
        
        // Verify triangle still contains our point
        const int p0 = tri.pointIndex(0);
        const int p1 = tri.pointIndex(1);
        const int p2 = tri.pointIndex(2);
        if (p0 != iPointIndex && p1 != iPointIndex && p2 != iPointIndex) {
            continue;
        }
        
        if (m_vecTriangles[neighborIdx].isInCircumcircle(pt)) {
            swapEdge(triIdx, neighborIdx);
            
            // Add new opposite edges to stack
            for (int i = 0; i < 3; ++i) {
                if (m_vecTriangles[triIdx].pointIndex(i) == iPointIndex) {
                    const int oppEdge = (i + 1) % 3;
                    if (m_vecTriangles[triIdx].neighbourIndex(oppEdge) != NO_NEIGHBOR) {
                        edgeStack.emplace_back(triIdx, oppEdge);
                    }
                    break;
                }
            }
            
            for (int i = 0; i < 3; ++i) {
                if (m_vecTriangles[neighborIdx].pointIndex(i) == iPointIndex) {
                    const int oppEdge = (i + 1) % 3;
                    if (m_vecTriangles[neighborIdx].neighbourIndex(oppEdge) != NO_NEIGHBOR) {
                        edgeStack.emplace_back(neighborIdx, oppEdge);
                    }
                    break;
                }
            }
        }
    }
}

void Mesh::swapEdge(int iTri1, int iTri2)
{    
    Triangle& triCurrent = m_vecTriangles[iTri1];
    Triangle& triNeighbour = m_vecTriangles[iTri2];

    int diff1 = NO_NEIGHBOR;
    int diff2 = NO_NEIGHBOR;
    std::vector<int> shared;

    // Find shared and different points
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (triCurrent.pointIndex(i) == triNeighbour.pointIndex(j)) {
                shared.push_back(triCurrent.pointIndex(i));
            }
        }
    }

    for (size_t i = 0; i < 3; ++i) {
        int ptCurrent = triCurrent.pointIndex(i);
        int ptNeighbour = triNeighbour.pointIndex(i);

        if (std::find(shared.begin(), shared.end(), ptCurrent) == shared.end()) {
            diff1 = ptCurrent;
        }
        if (std::find(shared.begin(), shared.end(), ptNeighbour) == shared.end()) {
            diff2 = ptNeighbour;
        }
    }

    // Save original neighbor indices
    const int iCurrentN0 = triCurrent.neighbourIndex(0);
    const int iCurrentN1 = triCurrent.neighbourIndex(1);
    const int iCurrentN2 = triCurrent.neighbourIndex(2);

    const int iNeighbourN0 = triNeighbour.neighbourIndex(0);
    const int iNeighbourN1 = triNeighbour.neighbourIndex(1);
    const int iNeighbourN2 = triNeighbour.neighbourIndex(2);

    const int iCurrentPt0 = triCurrent.pointIndex(0);
    const int iCurrentPt1 = triCurrent.pointIndex(1);
    const int iCurrentPt2 = triCurrent.pointIndex(2);

    // Update triCurrent
    for (size_t i = 0; i < 3; ++i) {
        if (triCurrent.pointIndex(i) == shared[0]) {
            triCurrent.setPointIndex(i, diff2);

            if (!isSuperVertex(diff2)) {
                triCurrent.setPoint(i, m_vecPoints[diff2]);
            } else {
                triCurrent.setPoint(i, superTrianglePoint(diff2));
            }

            int iSharedEdge = findSharedEdge(triCurrent, diff1, diff2);
            int iNewEdge = findNewEdge(i, iSharedEdge);

            int iStolenEdge = NO_NEIGHBOR;
            if ((triNeighbour.pointIndex(0) == shared[1] && triNeighbour.pointIndex(1) == diff2) ||
                (triNeighbour.pointIndex(1) == shared[1] && triNeighbour.pointIndex(0) == diff2)) {
                iStolenEdge = iNeighbourN0;
            } else if ((triNeighbour.pointIndex(1) == shared[1] && triNeighbour.pointIndex(2) == diff2) ||
                       (triNeighbour.pointIndex(2) == shared[1] && triNeighbour.pointIndex(1) == diff2)) {
                iStolenEdge = iNeighbourN1;
            } else if ((triNeighbour.pointIndex(2) == shared[1] && triNeighbour.pointIndex(0) == diff2) ||
                       (triNeighbour.pointIndex(0) == shared[1] && triNeighbour.pointIndex(2) == diff2)) {
                iStolenEdge = iNeighbourN2;
            }

            triCurrent.setNeighbourIndex(iSharedEdge, iTri2);
            if (iNewEdge >= 0) {
                triCurrent.setNeighbourIndex(iNewEdge, iStolenEdge);
            }
            break;
        }
    }

    // Update triNeighbour
    for (size_t i = 0; i < 3; ++i) {
        if (triNeighbour.pointIndex(i) == shared[1]) {
            triNeighbour.setPointIndex(i, diff1);
            
            if (!isSuperVertex(diff1)) {
                triNeighbour.setPoint(i, m_vecPoints[diff1]);
            } else {
                triNeighbour.setPoint(i, superTrianglePoint(diff1));
            }

            int iSharedEdge = findSharedEdge(triNeighbour, diff1, diff2);
            int iNewEdge = findNewEdge(i, iSharedEdge);

            int iStolenEdge = NO_NEIGHBOR;
            if ((iCurrentPt0 == diff1 && iCurrentPt1 == shared[0]) ||
                (iCurrentPt1 == diff1 && iCurrentPt0 == shared[0])) {
                iStolenEdge = iCurrentN0;
            } else if ((iCurrentPt1 == diff1 && iCurrentPt2 == shared[0]) ||
                       (iCurrentPt2 == diff1 && iCurrentPt1 == shared[0])) {
                iStolenEdge = iCurrentN1;
            } else if ((iCurrentPt2 == diff1 && iCurrentPt0 == shared[0]) ||
                       (iCurrentPt0 == diff1 && iCurrentPt2 == shared[0])) {
                iStolenEdge = iCurrentN2;
            }

            triNeighbour.setNeighbourIndex(iSharedEdge, iTri1);
            if (iNewEdge >= 0) {
                triNeighbour.setNeighbourIndex(iNewEdge, iStolenEdge);
            }
            break;
        }
    }

    // Update all affected neighbors
    updateNeighboursAfterSwap(iCurrentN0, iTri1, iTri2);
    updateNeighboursAfterSwap(iCurrentN1, iTri1, iTri2);
    updateNeighboursAfterSwap(iCurrentN2, iTri1, iTri2);
    updateNeighboursAfterSwap(iNeighbourN0, iTri2, iTri1);
    updateNeighboursAfterSwap(iNeighbourN1, iTri2, iTri1);
    updateNeighboursAfterSwap(iNeighbourN2, iTri2, iTri1);
}

int Mesh::findSharedEdge(const Triangle& tri, int iDiff1, int iDiff2) const noexcept
{
    for (size_t i = 0; i < 3; ++i) {
        int curr = tri.pointIndex(i);
        int next = tri.pointIndex((i + 1) % 3);
        
        if ((curr == iDiff1 && next == iDiff2) || (curr == iDiff2 && next == iDiff1)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int Mesh::findNewEdge(int i, int iSharedEdge) const noexcept
{
    if (i < 0 || i > 2 || iSharedEdge < 0 || iSharedEdge > 2) {
        return -1;
    }
    return NEW_EDGE_TABLE[i][iSharedEdge];
}

void Mesh::removeHelperTriangles()
{    
    auto isHelperTriangle = [](const Triangle& tri) {
        for (size_t i = 0; i < 3; ++i) {
            if (isSuperVertex(tri.pointIndex(i))) {
                return true;
            }
        }
        return false;
    };
    
    // Disconnect helper triangles
    for (auto& tri : m_vecTriangles) {
        if (isHelperTriangle(tri)) {
            updateRemovedNeighbours(tri.index());
        }
    }
    
    // Remove them
    std::erase_if(m_vecTriangles, isHelperTriangle);
    
    updateTriangleIndicesAfterRemoval();
}

void Mesh::updateRemovedNeighbours(int iRemovedTriangleIndex)
{    
    const Triangle& triRemoved = m_vecTriangles[iRemovedTriangleIndex];

    for (size_t i = 0; i < 3; ++i) {
        int neighbourIdx = triRemoved.neighbourIndex(i);
        if (neighbourIdx == NO_NEIGHBOR) continue;
        updateNeighbourReference(neighbourIdx, iRemovedTriangleIndex, NO_NEIGHBOR);
    }
}

void Mesh::updateTriangleIndicesAfterRemoval()
{    
    // Build old -> new index mapping
    std::unordered_map<int, int> indexMap;
    for (size_t i = 0; i < m_vecTriangles.size(); ++i) {
        int oldIndex = m_vecTriangles[i].index();
        indexMap[oldIndex] = static_cast<int>(i);
        m_vecTriangles[i].setIndex(static_cast<int>(i));
    }
    
    // Update all neighbour references
    for (auto& tri : m_vecTriangles) {
        for (size_t edge = 0; edge < 3; ++edge) {
            int oldNeighbourIdx = tri.neighbourIndex(edge);
            if (oldNeighbourIdx == NO_NEIGHBOR) continue;
            
            auto it = indexMap.find(oldNeighbourIdx);
            int newIdx = (it != indexMap.end()) ? it->second : NO_NEIGHBOR;
            tri.setNeighbourIndex(edge, newIdx);
        }
    }
}

void to_json(nlohmann::json& j, const Mesh& m)
{
    j = nlohmann::json{
        {"triangles", m.triangles()},
        {"points", m.points()}
    };
}