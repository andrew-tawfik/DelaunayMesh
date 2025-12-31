#ifndef MESH_H
#define MESH_H

#include "point.h"
#include "triangle.h"
#include <vector>
#include <queue>

class Mesh {
public:
    // Constructors
    Mesh();
    explicit Mesh(const std::vector<Point>& vecPt);

    // Rule of Five
    ~Mesh() = default;
    Mesh(const Mesh&) = default;
    Mesh& operator=(const Mesh&) = default;
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    // Accessors (const references to avoid copies)
    [[nodiscard]] const std::vector<Point>& points() const { return m_vecPoints; }
    [[nodiscard]] const std::vector<Triangle>& triangles() const { return m_vecTriangles; }

    // Mutators
    void setPtVector(const std::vector<Point>& vecPt);
    void addPoint(const Point& pt);
    void setTriVector(const std::vector<Triangle>& vecTri);

    // Triangle location
    [[nodiscard]] int findContainingTriangle(const Point& pt) const;

    // Super triangle
    [[nodiscard]] static Triangle superTriangle();
    [[nodiscard]] static Point superTrianglePoint(int superVertexIndex);

    // Triangle creation
    void createTriangles(int iTriangleIndex, int iPointIndex);
    void createTrianglesInside(int iTriangleIndex, int iPointIndex);
    void handleEdgeCase(int iTriangleIndex, int iPointIndex);
    void createTrianglesOppositeSide(int iTriangleIndex, int iPointIndex, int iNeighbourIndex0, int iNeighbourIndex1);

    // Neighbor management
    void updateNeighbourReference(int neighbourIdx, int oldRef, int newRef) noexcept;
    void updateEdgeNeighbours(int iTriangleIndex, int iNewTriangleIndex, int iNeighbourIndex0, int iNeighbourIndex1);
    [[nodiscard]] bool areNeighbours(int iTri1, int iTri2) const noexcept;
    void updateNeighboursAfterSwap(int oldNeighborIndex, int oldTriangleIndex, int newTriangleIndex) noexcept;
    void updateRemovedNeighbours(int iRemovedTriangleIndex);
    void updateTriangleIndicesAfterRemoval();

    // Delaunay restoration
    void restoreDelaunay(int iPointIndex, const std::vector<int>& initialTriangles);
    [[nodiscard]] std::queue<int> checkNeighboringCircumcircles(int iTriangleIndex, int iPointIndex, int iEdgeIndex);

    // Edge swapping
    void swapEdge(int iTri1, int iTri2);
    void swapAll(std::queue<int>& neighbourQueue, int iPointIndex);
    [[nodiscard]] int findSharedEdge(const Triangle& tri, int iDiff1, int iDiff2) const noexcept;
    [[nodiscard]] int findNewEdge(int i, int iSharedEdge) const noexcept;

    // Main triangulation interface
    void triangulatePoint(double x, double y);
    void buildMesh();
    void removeHelperTriangles();

private:
    std::vector<Point> m_vecPoints;
    std::vector<Triangle> m_vecTriangles;
};

// JSON serialization
void to_json(nlohmann::json& j, const Mesh& m);

#endif // MESH_H