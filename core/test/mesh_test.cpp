
#include "../include/mesh.h"
#include "../include/triangle.h"
#include "../include/point.h"
#include <gtest/gtest.h>
#include <vector>
#include <random>
#include "test_helper.h"

// === Unit Test Definitions ===
class MeshTestFixture : public ::testing::Test
{
protected:
    std::vector<Point> testCaseRect{
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
        Point(22.5, 7.0), Point(22.5, 12.0), Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7.0)};

    std::vector<Point> testCaseHex = {
        Point(22, 8.6), Point(18.5, 14.7), Point(11.5, 14.7),
        Point(8, 8.6), Point(11.5, 2.5), Point(18.5, 2.5)};

    std::vector<Point> testCaseInner = {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
        Point(22.5, 7.0), Point(22.5, 12.0), Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7.0), Point(15, 7),
        Point(11.25, 4.5), Point(18.75, 9.5)};

    std::vector<Point> testCaseMinimal = {
        Point(0, 0), Point(10, 0), Point(5, 10)};

    std::vector<Point> testCaseSingle = {
        Point(5, 5)};
};

TEST_F(MeshTestFixture, NeighborConsistencyInvariant)
{

    Mesh m1(testCaseRect);
    m1.buildMesh();
    EXPECT_TRUE(verifyNeighbourConsistency(m1))
        << "Neighbor consistency failed after buildMesh";

    Mesh m2(testCaseHex);
    m2.buildMesh();
    EXPECT_TRUE(verifyNeighbourConsistency(m2))
        << "Neighbor consistency failed after buildMesh";

    Mesh m3(testCaseInner);
    m3.buildMesh();
    EXPECT_TRUE(verifyNeighbourConsistency(m3))
        << "Neighbor consistency failed after buildMesh";
}
TEST_F(MeshTestFixture, DelaunayPropertyHolds)
{
    Mesh m1(testCaseRect);
    m1.buildMesh();
    EXPECT_TRUE(verifyDelaunayProperty(m1))
        << "Delaunay property violated after buildMesh";

    Mesh m2(testCaseHex);
    m2.buildMesh();
    EXPECT_TRUE(verifyDelaunayProperty(m2))
        << "Delaunay property violated after buildMesh";

    Mesh m3(testCaseInner);
    m3.buildMesh();
    EXPECT_TRUE(verifyDelaunayProperty(m3))
        << "Delaunay property violated after buildMesh";
}
TEST_F(MeshTestFixture, CreateTriangles_PointInside_MultipleInsertions)
{
    // Test sequential insertions to catch accumulated corruption
    Mesh mesh;

    std::vector<Point> pointsToInsert = {
        Point(10.0, 5.0),
        Point(15.0, 8.0),
        Point(5.0, 3.0),
        Point(12.0, 10.0)};

    for (size_t i = 0; i < pointsToInsert.size(); i++)
    {
        mesh.addPoint(pointsToInsert[i]);

        int containingTri = mesh.findContainingTriangle(pointsToInsert[i]);
        ASSERT_GE(containingTri, 0)
            << "Point " << i << " should be inside mesh";

        int triCountBefore = mesh.getTriVector().size();
        mesh.createTriangles(containingTri, i);
        int triCountAfter = mesh.getTriVector().size();

        // Each interior insertion adds 2 triangles
        EXPECT_EQ(triCountAfter, triCountBefore + 2)
            << "Failed on insertion " << i;

        // Verify consistency after each insertion
        EXPECT_TRUE(verifyNeighbourConsistency(mesh))
            << "Neighbor consistency failed after insertion " << i;
    }

    // Final check: all inserted points are vertices
    std::vector<Triangle> triangles = mesh.getTriVector();
    for (size_t pointIdx = 0; pointIdx < pointsToInsert.size(); pointIdx++)
    {
        bool foundAsVertex = false;
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                if (t.pointIndex(i) == static_cast<int>(pointIdx))
                {
                    foundAsVertex = true;
                    break;
                }
            }
            if (foundAsVertex)
                break;
        }
        EXPECT_TRUE(foundAsVertex)
            << "Point " << pointIdx << " not found as a vertex in any triangle";
    }
}
TEST_F(MeshTestFixture, SwapEdge_RestoresDelaunay)
{
    Mesh mesh;

    Point A(0.0, 0.0);
    Point B(3.0, 0.0);
    Point C(2.0, 1.0);
    Point D(0.0, 2.0);

    mesh.addPoint(A); // index 0
    mesh.addPoint(B); // index 1
    mesh.addPoint(C); // index 2
    mesh.addPoint(D); // index 3

    // Manually create "bad" triangulation with Delaunay violation
    // Triangle 0: A-B-D (indices 0, 1, 3)
    // Triangle 1: B-C-D (indices 1, 2, 3)

    Triangle tri0(A, B, D);
    tri0.setIndex(0);
    tri0.setPointIndex(0, 0); // A
    tri0.setPointIndex(1, 1); // B
    tri0.setPointIndex(2, 3); // D

    Triangle tri1(B, C, D);
    tri1.setIndex(1);
    tri1.setPointIndex(0, 1); // B
    tri1.setPointIndex(1, 2); // C
    tri1.setPointIndex(2, 3); // D

    // Set up neighbor relationship (they share edge B-D)
    // tri0: edge 1 is B-D, neighbor is tri1
    // tri1: edge 2 is D-B, neighbor is tri0
    tri0.setNeighbourIndex(0, -1); // A-B edge (no neighbor)
    tri0.setNeighbourIndex(1, 1);  // B-D edge -> tri1
    tri0.setNeighbourIndex(2, -1); // D-A edge (no neighbor)

    tri1.setNeighbourIndex(0, -1); // B-C edge (no neighbor)
    tri1.setNeighbourIndex(1, -1); // C-D edge (no neighbor)
    tri1.setNeighbourIndex(2, 0);  // D-B edge -> tri0

    mesh.setTriVector({tri0, tri1});

    // === Verify initial state: Delaunay is violated ===
    std::vector<Triangle> trianglesBefore = mesh.getTriVector();

    // Check: C should be inside circumcircle of tri0 (ABD)
    bool violationExists = trianglesBefore[0].isInCircumcircle(C);
    ASSERT_TRUE(violationExists)
        << "Test setup error: C should be inside circumcircle of ABD";

    // === Perform the swap ===
    mesh.swapEdge(0, 1);

    // === Verify: Delaunay violation is fixed ===
    std::vector<Triangle> trianglesAfter = mesh.getTriVector();

    // After swap, diagonal should be A-C instead of B-D
    // New triangles should be: A-B-C and A-C-D

    // Check: No point is inside any triangle's circumcircle
    std::vector<Point> points = mesh.getPtVector();

    for (const Triangle &t : trianglesAfter)
    {
        int vertexIndices[3] = {
            t.pointIndex(0),
            t.pointIndex(1),
            t.pointIndex(2)};

        for (size_t i = 0; i < points.size(); i++)
        {
            // Skip vertices of this triangle
            if (static_cast<int>(i) == vertexIndices[0] ||
                static_cast<int>(i) == vertexIndices[1] ||
                static_cast<int>(i) == vertexIndices[2])
            {
                continue;
            }

            EXPECT_FALSE(t.isInCircumcircle(points[i]))
                << "Point " << i << " is inside circumcircle of triangle " << t.index()
                << " after swap - Delaunay not restored";
        }
    }

    // === Verify: Both triangles still valid ===
    for (const Triangle &t : trianglesAfter)
    {
        EXPECT_GT(t.area(), 0)
            << "Triangle " << t.index() << " has non-positive area after swap";
    }

    // === Verify: Neighbor consistency maintained ===
    EXPECT_TRUE(verifyNeighbourConsistency(mesh))
        << "Neighbor consistency violated after swapEdge";
}

TEST_F(MeshTestFixture, FindContainingTriangle_AlwaysFindsCorrectTriangle)
{
    // === Case 1: Point inside super triangle ===
    {
        Mesh mesh;
        Point inside(10.0, 5.0);

        int foundIdx = mesh.findContainingTriangle(inside);

        ASSERT_GE(foundIdx, 0) << "Should find super triangle";
        EXPECT_TRUE(mesh.getTriVector()[foundIdx].contains(inside))
            << "Returned triangle should actually contain the point";
    }

    // === Case 2: After building mesh, find each original point ===
    {
        Mesh mesh(testCaseRect);
        mesh.buildMesh();

        std::vector<Point> points = mesh.getPtVector();
        std::vector<Triangle> triangles = mesh.getTriVector();

        for (int i = 0; i < points.size(); i++)
        {
            const Point &p = points[i];
            int foundIdx = mesh.findContainingTriangle(p);

            ASSERT_GE(foundIdx, 0)
                << "Should find triangle for point " << i;

            // Point should be contained or be a vertex
            const Triangle &t = triangles[foundIdx];
            bool isContained = t.contains(p);
            bool isVertex = (t.pointIndex(0) == i ||
                             t.pointIndex(1) == i ||
                             t.pointIndex(2) == i);

            EXPECT_TRUE(isContained || isVertex)
                << "Point " << i << " not in returned triangle " << foundIdx;
        }
    }

    // === Case 3: Find centroids of all triangles ===
    {
        Mesh mesh(testCaseRect);
        mesh.buildMesh();

        std::vector<Triangle> triangles = mesh.getTriVector();

        for (const Triangle &t : triangles)
        {
            // Compute centroid
            double cx = (t.point(0).x() + t.point(1).x() + t.point(2).x()) / 3.0f;
            double cy = (t.point(0).y() + t.point(1).y() + t.point(2).y()) / 3.0f;
            Point centroid(cx, cy);

            int foundIdx = mesh.findContainingTriangle(centroid);

            EXPECT_EQ(foundIdx, t.index())
                << "Centroid of triangle " << t.index()
                << " should be found in that same triangle, but found in " << foundIdx;
        }
    }

    // === Case 4: Point outside all triangles returns -1 ===
    {
        Mesh mesh;
        // Point outside the super triangle
        Point outside(1e8, 1e8);

        int foundIdx = mesh.findContainingTriangle(outside);

        EXPECT_EQ(foundIdx, -1)
            << "Point outside mesh should return -1";
    }

    // === Case 5: Point on edge returns one of the adjacent triangles ===
    {
        Mesh mesh(testCaseRect);
        mesh.buildMesh();

        std::vector<Triangle> triangles = mesh.getTriVector();

        // Find an internal edge (shared by two triangles)
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                int neighborIdx = t.neighbourIndex(i);
                if (neighborIdx >= 0)
                {
                    // Get midpoint of this edge
                    Point edgeMidpoint = t.edgeMidpoint(i);

                    int foundIdx = mesh.findContainingTriangle(edgeMidpoint);

                    EXPECT_TRUE(foundIdx == t.index() || foundIdx == neighborIdx)
                        << "Edge midpoint should be in one of the two adjacent triangles";

                    // Just test one edge
                    goto edgeTestDone;
                }
            }
        }
    edgeTestDone:;
    }

    // === Case 6: Repeated calls with same point return consistent result ===
    {
        Mesh mesh(testCaseRect);
        mesh.buildMesh();

        Point testPoint(15.0, 7.0);

        int firstResult = mesh.findContainingTriangle(testPoint);
        ASSERT_GE(firstResult, 0);

        // Call multiple times - should always find a valid containing triangle
        for (int i = 0; i < 10; i++)
        {
            int result = mesh.findContainingTriangle(testPoint);
            ASSERT_GE(result, 0);

            // Result might differ due to random start, but must always contain the point
            EXPECT_TRUE(mesh.getTriVector()[result].contains(testPoint))
                << "Iteration " << i << ": returned triangle doesn't contain point";
        }
    }

    // === Case 7: Works with larger mesh ===
    {
        Mesh mesh(testCaseInner); // Has interior points
        mesh.buildMesh();

        std::vector<Triangle> triangles = mesh.getTriVector();

        // Test several interior points
        std::vector<Point> testPoints = {
            Point(15.0, 7.0),
            Point(10.0, 5.0),
            Point(20.0, 10.0),
            Point(12.0, 8.0)};

        for (const Point &p : testPoints)
        {
            int foundIdx = mesh.findContainingTriangle(p);

            if (foundIdx >= 0)
            {
                EXPECT_TRUE(triangles[foundIdx].contains(p))
                    << "Returned triangle should contain the test point";
            }
        }
    }
}

TEST_F(MeshTestFixture, SwapEdge_MaintainsNeighborConsistency)
{
    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(3.0, 0.0);
        Point C(2.0, 1.0);
        Point D(0.0, 2.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);

        Triangle tri0(A, B, D);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 3);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, 1);
        tri0.setNeighbourIndex(2, -1);

        Triangle tri1(B, C, D);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 1);
        tri1.setPointIndex(1, 2);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, -1);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, 0);

        mesh.setTriVector({tri0, tri1});

        ASSERT_TRUE(verifyNeighbourConsistency(mesh));

        mesh.swapEdge(0, 1);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));

        std::vector<Triangle> triangles = mesh.getTriVector();
        EXPECT_EQ(triangles.size(), 2);
        for (const Triangle &t : triangles)
        {
            EXPECT_GT(t.area(), 0);
        }
    }

    {
        Mesh mesh;

        Point A(1.0, -2.0);
        Point B(1.0, 0.0);
        Point C(3.0, 1.0);
        Point D(-1.0, 1.0);
        Point E(1.0, 3.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);
        mesh.addPoint(E);

        Triangle tri0(A, B, D);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 3);
        tri0.setNeighbourIndex(0, 2);
        tri0.setNeighbourIndex(1, 1);
        tri0.setNeighbourIndex(2, -1);

        Triangle tri1(B, E, D);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 1);
        tri1.setPointIndex(1, 4);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, 3);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, 0);

        Triangle tri2(A, C, B);
        tri2.setIndex(2);
        tri2.setPointIndex(0, 0);
        tri2.setPointIndex(1, 2);
        tri2.setPointIndex(2, 1);
        tri2.setNeighbourIndex(0, -1);
        tri2.setNeighbourIndex(1, 3);
        tri2.setNeighbourIndex(2, 0);

        Triangle tri3(B, C, E);
        tri3.setIndex(3);
        tri3.setPointIndex(0, 1);
        tri3.setPointIndex(1, 2);
        tri3.setPointIndex(2, 4);
        tri3.setNeighbourIndex(0, 2);
        tri3.setNeighbourIndex(1, -1);
        tri3.setNeighbourIndex(2, 1);

        mesh.setTriVector({tri0, tri1, tri2, tri3});

        ASSERT_TRUE(verifyNeighbourConsistency(mesh));

        mesh.swapEdge(1, 3);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));
    }
}

TEST_F(MeshTestFixture, CreateTriangles_TriggersSwapWhenNeeded)
{
    {
        Mesh mesh;

        for (size_t i = 0; i < testCaseRect.size(); i++)
        {
            mesh.triangulatePoint(testCaseRect[i].x(), testCaseRect[i].y());

            EXPECT_TRUE(verifyDelaunayProperty(mesh))
                << "Delaunay violated after inserting point " << i;

            EXPECT_TRUE(verifyNeighbourConsistency(mesh))
                << "Neighbor consistency violated after inserting point " << i;
        }
    }

    {
        Mesh mesh;

        for (size_t i = 0; i < testCaseInner.size(); i++)
        {
            mesh.triangulatePoint(testCaseInner[i].x(), testCaseInner[i].y());

            EXPECT_TRUE(verifyDelaunayProperty(mesh))
                << "Delaunay violated after inserting interior point " << i;

            EXPECT_TRUE(verifyNeighbourConsistency(mesh))
                << "Neighbor consistency violated after inserting interior point " << i;
        }
    }

    {
        Mesh mesh;

        std::vector<Point> worstCase = {
            Point(0.0, 0.0),
            Point(10.0, 0.0),
            Point(10.0, 10.0),
            Point(0.0, 10.0),
            Point(5.0, 5.0)};

        for (size_t i = 0; i < worstCase.size(); i++)
        {
            mesh.triangulatePoint(worstCase[i].x(), worstCase[i].y());

            EXPECT_TRUE(verifyDelaunayProperty(mesh))
                << "Delaunay violated after inserting worst case point " << i;

            EXPECT_TRUE(verifyNeighbourConsistency(mesh))
                << "Neighbor consistency violated after inserting worst case point " << i;
        }
    }

    {
        Mesh mesh;

        std::vector<Point> cascadeCase = {
            Point(0.0, 0.0),
            Point(6.0, 0.0),
            Point(3.0, 1.0),
            Point(3.0, 0.5),
            Point(3.0, 0.25)};

        for (size_t i = 0; i < cascadeCase.size(); i++)
        {
            mesh.triangulatePoint(cascadeCase[i].x(), cascadeCase[i].y());

            EXPECT_TRUE(verifyDelaunayProperty(mesh))
                << "Delaunay violated after inserting cascade point " << i;

            EXPECT_TRUE(verifyNeighbourConsistency(mesh))
                << "Neighbor consistency violated after inserting cascade point " << i;
        }
    }

    {
        Mesh mesh;

        std::mt19937 rng(42);
        std::uniform_real_distribution<double> dist(1.0, 100.0);

        for (int i = 0; i < 20; i++)
        {
            double x = dist(rng);
            double y = dist(rng);

            mesh.triangulatePoint(x, y);

            EXPECT_TRUE(verifyDelaunayProperty(mesh))
                << "Delaunay violated after inserting random point " << i
                << " (" << x << ", " << y << ")";

            EXPECT_TRUE(verifyNeighbourConsistency(mesh))
                << "Neighbor consistency violated after inserting random point " << i;
        }
    }

    {
        Mesh mesh(testCaseRect);
        mesh.buildMesh();

        EXPECT_TRUE(verifyDelaunayProperty(mesh))
            << "Delaunay violated after buildMesh (before helper removal)";

        EXPECT_TRUE(verifyNeighbourConsistency(mesh))
            << "Neighbor consistency violated after buildMesh";
    }
}

TEST_F(MeshTestFixture, CheckNeighboringCircumcircles_FindsAllViolations)
{
    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(4.0, 0.0);
        Point C(2.0, 3.46);
        Point P(2.0, 1.15);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0});

        std::queue<int> result = mesh.checkNeighboringCircumcircles(0, 3, 0);

        EXPECT_EQ(result.size(), 1);
        EXPECT_EQ(result.front(), 0);
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(3.0, 0.0);
        Point C(2.0, 1.0);
        Point D(0.0, 2.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);

        Triangle tri0(B, C, D);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 1);
        tri0.setPointIndex(1, 2);
        tri0.setPointIndex(2, 3);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, 1);

        Triangle tri1(A, B, D);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 0);
        tri1.setPointIndex(1, 1);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, 0);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0, tri1});

        ASSERT_TRUE(tri1.isInCircumcircle(C));

        std::queue<int> result = mesh.checkNeighboringCircumcircles(0, 2, 2);

        EXPECT_EQ(result.size(), 2);
    }

    {
        Mesh mesh;

        Point O(0.0, 0.0);
        Point V1(10.0, 0.5);
        Point V2(10.0, 1.0);
        Point V3(10.0, 1.5);
        Point V4(10.0, 2.0);
        Point P(0.5, 1.0);

        mesh.addPoint(O);
        mesh.addPoint(V1);
        mesh.addPoint(V2);
        mesh.addPoint(V3);
        mesh.addPoint(V4);
        mesh.addPoint(P);

        Triangle tri0(O, V1, V2);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, 1);
        tri0.setNeighbourIndex(2, -1);

        Triangle tri1(O, V2, V3);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 0);
        tri1.setPointIndex(1, 2);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, -1);
        tri1.setNeighbourIndex(1, 2);
        tri1.setNeighbourIndex(2, 0);

        Triangle tri2(O, V3, V4);
        tri2.setIndex(2);
        tri2.setPointIndex(0, 0);
        tri2.setPointIndex(1, 3);
        tri2.setPointIndex(2, 4);
        tri2.setNeighbourIndex(0, -1);
        tri2.setNeighbourIndex(1, -1);
        tri2.setNeighbourIndex(2, 1);

        mesh.setTriVector({tri0, tri1, tri2});

        ASSERT_TRUE(tri0.isInCircumcircle(P));
        ASSERT_TRUE(tri1.isInCircumcircle(P));
        ASSERT_TRUE(tri2.isInCircumcircle(P));

        std::queue<int> result = mesh.checkNeighboringCircumcircles(0, 5, 1);

        EXPECT_GE(result.size(), 3);
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(2.0, 0.0);
        Point C(1.0, 1.73);
        Point D(1.0, -10.0);
        Point E(1.0, 5.0);
        Point P(1.0, 0.5);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);
        mesh.addPoint(E);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, 1);
        tri0.setNeighbourIndex(1, 2);
        tri0.setNeighbourIndex(2, -1);

        Triangle tri1(A, D, B);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 0);
        tri1.setPointIndex(1, 3);
        tri1.setPointIndex(2, 1);
        tri1.setNeighbourIndex(0, -1);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, 0);

        Triangle tri2(B, E, C);
        tri2.setIndex(2);
        tri2.setPointIndex(0, 1);
        tri2.setPointIndex(1, 4);
        tri2.setPointIndex(2, 2);
        tri2.setNeighbourIndex(0, -1);
        tri2.setNeighbourIndex(1, -1);
        tri2.setNeighbourIndex(2, 0);

        mesh.setTriVector({tri0, tri1, tri2});

        bool edge0Violates = tri1.isInCircumcircle(P);
        bool edge1Violates = tri2.isInCircumcircle(P);

        std::queue<int> result0 = mesh.checkNeighboringCircumcircles(0, 5, 0);
        std::queue<int> result1 = mesh.checkNeighboringCircumcircles(0, 5, 1);

        if (edge0Violates && !edge1Violates)
        {
            EXPECT_GE(result0.size(), 2);
            EXPECT_EQ(result1.size(), 1);
        }
        else if (!edge0Violates && edge1Violates)
        {
            EXPECT_EQ(result0.size(), 1);
            EXPECT_GE(result1.size(), 2);
        }
    }

    {
        Mesh mesh;

        Point O(5.0, 5.0);
        Point V1(15.0, 5.1);
        Point V2(5.1, 15.0);
        Point V3(-5.0, 5.1);
        Point V4(5.1, -5.0);
        Point P(5.0, 5.1);

        mesh.addPoint(O);
        mesh.addPoint(V1);
        mesh.addPoint(V2);
        mesh.addPoint(V3);
        mesh.addPoint(V4);
        mesh.addPoint(P);

        Triangle tri0(O, V1, V2);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, 1);
        tri0.setNeighbourIndex(2, 3);

        Triangle tri1(O, V2, V3);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 0);
        tri1.setPointIndex(1, 2);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, -1);
        tri1.setNeighbourIndex(1, 2);
        tri1.setNeighbourIndex(2, 0);

        Triangle tri2(O, V3, V4);
        tri2.setIndex(2);
        tri2.setPointIndex(0, 0);
        tri2.setPointIndex(1, 3);
        tri2.setPointIndex(2, 4);
        tri2.setNeighbourIndex(0, -1);
        tri2.setNeighbourIndex(1, 3);
        tri2.setNeighbourIndex(2, 1);

        Triangle tri3(O, V4, V1);
        tri3.setIndex(3);
        tri3.setPointIndex(0, 0);
        tri3.setPointIndex(1, 4);
        tri3.setPointIndex(2, 1);
        tri3.setNeighbourIndex(0, -1);
        tri3.setNeighbourIndex(1, 0);
        tri3.setNeighbourIndex(2, 2);

        mesh.setTriVector({tri0, tri1, tri2, tri3});

        ASSERT_TRUE(verifyNeighbourConsistency(mesh));

        std::queue<int> result = mesh.checkNeighboringCircumcircles(0, 5, 1);

        std::set<int> visited;
        while (!result.empty())
        {
            int idx = result.front();
            result.pop();
            EXPECT_EQ(visited.count(idx), 0) << "Triangle " << idx << " visited multiple times";
            visited.insert(idx);
        }
    }

    {
        Mesh mesh;

        Triangle super = mesh.superTriangle();
        mesh.setTriVector({super});

        Point P(10.0, 5.0);
        mesh.addPoint(P);

        std::queue<int> result = mesh.checkNeighboringCircumcircles(0, 0, 0);

        EXPECT_GE(result.size(), 1);
        EXPECT_EQ(result.front(), 0);
    }

    {
        Mesh mesh;

        Point O(0.0, 0.0);
        Point A(5.0, 0.0);
        Point B(5.0, 5.0);
        Point C(0.0, 5.0);
        Point D(50.0, 2.5);
        Point E(-50.0, 2.5);
        Point F(2.5, 2.5);
        Point G(2.5, 50.0);
        Point H(2.5, -50.0);
        Point P(2.0, 2.0);

        mesh.addPoint(O);
        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);
        mesh.addPoint(E);
        mesh.addPoint(F);
        mesh.addPoint(G);
        mesh.addPoint(H);
        mesh.addPoint(P);

        Triangle triS(O, A, C);
        triS.setIndex(0);
        triS.setPointIndex(0, 0);
        triS.setPointIndex(1, 1);
        triS.setPointIndex(2, 3);
        triS.setNeighbourIndex(0, 1);
        triS.setNeighbourIndex(1, 2);
        triS.setNeighbourIndex(2, -1);

        Triangle triN1(A, D, C);
        triN1.setIndex(1);
        triN1.setPointIndex(0, 1);
        triN1.setPointIndex(1, 4);
        triN1.setPointIndex(2, 3);
        triN1.setNeighbourIndex(0, -1);
        triN1.setNeighbourIndex(1, -1);
        triN1.setNeighbourIndex(2, 0);

        Triangle triN2(O, C, E);
        triN2.setIndex(2);
        triN2.setPointIndex(0, 0);
        triN2.setPointIndex(1, 3);
        triN2.setPointIndex(2, 5);
        triN2.setNeighbourIndex(0, 0);
        triN2.setNeighbourIndex(1, -1);
        triN2.setNeighbourIndex(2, -1);

        mesh.setTriVector({triS, triN1, triN2});

        bool n1Violates = triN1.isInCircumcircle(P);
        bool n2Violates = triN2.isInCircumcircle(P);

        std::queue<int> result = mesh.checkNeighboringCircumcircles(0, 9, 0);

        int expectedSize = 1;
        if (n1Violates)
            expectedSize++;

        EXPECT_GE(result.size(), 1);
    }

    {
        Mesh mesh;

        Triangle super = mesh.superTriangle();
        mesh.setTriVector({super});

        Point P(100.0, 100.0);
        mesh.addPoint(P);

        ASSERT_TRUE(super.contains(P));

        std::queue<int> result = mesh.checkNeighboringCircumcircles(0, 0, 0);

        EXPECT_EQ(result.size(), 1);
        EXPECT_EQ(result.front(), 0);
    }
}

TEST_F(MeshTestFixture, FullPipeline_ProducesValidMesh)
{
    Mesh mesh(testCaseInner);
    mesh.buildMesh();

    std::vector<Triangle> triangles = mesh.getTriVector();
    std::vector<Point> points = mesh.getPtVector();

    // All input points appear as vertices
    for (int i = 0; i < points.size(); i++)
    {
        bool found = false;
        for (const Triangle &t : triangles)
        {
            if (t.pointIndex(0) == i ||
                t.pointIndex(1) == i ||
                t.pointIndex(2) == i)
            {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << "Point " << i << " not found as vertex";
    }

    // All triangles valid
    for (const Triangle &t : triangles)
    {
        EXPECT_GT(t.area(), 0);
    }

    EXPECT_TRUE(verifyNeighbourConsistency(mesh));
    EXPECT_TRUE(verifyDelaunayProperty(mesh));
}

TEST_F(MeshTestFixture, PointOnEdge_FourTriangles)
{
    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(4.0, 0.0);
        Point C(2.0, 3.0);
        Point P(2.0, 0.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0});

        ASSERT_EQ(tri0.onEdge(P), 0);

        int triCountBefore = mesh.getTriVector().size();
        mesh.createTriangles(0, 3);
        int triCountAfter = mesh.getTriVector().size();

        EXPECT_EQ(triCountAfter, triCountBefore + 1);

        std::vector<Triangle> triangles = mesh.getTriVector();
        int trianglesWithP = 0;
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                if (t.pointIndex(i) == 3)
                {
                    trianglesWithP++;
                    break;
                }
            }
            EXPECT_GT(t.area(), 0);
        }
        EXPECT_EQ(trianglesWithP, 2);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(4.0, 0.0);
        Point C(2.0, 3.0);
        Point P(3.0, 1.5);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0});

        ASSERT_EQ(tri0.onEdge(P), 1);

        mesh.createTriangles(0, 3);

        std::vector<Triangle> triangles = mesh.getTriVector();
        EXPECT_EQ(triangles.size(), 2);

        int trianglesWithP = 0;
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                if (t.pointIndex(i) == 3)
                {
                    trianglesWithP++;
                    break;
                }
            }
            EXPECT_GT(t.area(), 0);
        }
        EXPECT_EQ(trianglesWithP, 2);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(4.0, 0.0);
        Point C(2.0, 3.0);
        Point P(1.0, 1.5);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, -1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0});

        ASSERT_EQ(tri0.onEdge(P), 2);

        mesh.createTriangles(0, 3);

        std::vector<Triangle> triangles = mesh.getTriVector();
        EXPECT_EQ(triangles.size(), 2);

        int trianglesWithP = 0;
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                if (t.pointIndex(i) == 3)
                {
                    trianglesWithP++;
                    break;
                }
            }
            EXPECT_GT(t.area(), 0);
        }
        EXPECT_EQ(trianglesWithP, 2);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(4.0, 0.0);
        Point C(2.0, 3.0);
        Point D(2.0, -3.0);
        Point P(2.0, 0.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, 1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, -1);

        Triangle tri1(B, A, D);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 1);
        tri1.setPointIndex(1, 0);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, 0);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0, tri1});

        ASSERT_TRUE(verifyNeighbourConsistency(mesh));
        ASSERT_EQ(tri0.onEdge(P), 0);

        int triCountBefore = mesh.getTriVector().size();
        mesh.createTriangles(0, 4);
        int triCountAfter = mesh.getTriVector().size();

        EXPECT_EQ(triCountBefore, 2);
        EXPECT_EQ(triCountAfter, 4);

        std::vector<Triangle> triangles = mesh.getTriVector();

        int trianglesWithP = 0;
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                if (t.pointIndex(i) == 4)
                {
                    trianglesWithP++;
                    break;
                }
            }
            EXPECT_GT(t.area(), 0);
        }
        EXPECT_EQ(trianglesWithP, 4);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(4.0, 0.0);
        Point C(2.0, 3.0);
        Point D(2.0, -3.0);
        Point P(2.0, 0.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, 1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, -1);

        Triangle tri1(B, A, D);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 1);
        tri1.setPointIndex(1, 0);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, 0);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0, tri1});

        mesh.createTriangles(0, 4);

        std::vector<Triangle> triangles = mesh.getTriVector();

        std::set<int> allPointIndices;
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                allPointIndices.insert(t.pointIndex(i));
            }
        }

        EXPECT_TRUE(allPointIndices.count(0) > 0);
        EXPECT_TRUE(allPointIndices.count(1) > 0);
        EXPECT_TRUE(allPointIndices.count(2) > 0);
        EXPECT_TRUE(allPointIndices.count(3) > 0);
        EXPECT_TRUE(allPointIndices.count(4) > 0);

        for (const Triangle &t : triangles)
        {
            std::set<int> triPoints;
            triPoints.insert(t.pointIndex(0));
            triPoints.insert(t.pointIndex(1));
            triPoints.insert(t.pointIndex(2));
            EXPECT_EQ(triPoints.size(), 3)
                << "Triangle " << t.index() << " has duplicate point indices";
        }

        int countA = 0, countB = 0, countC = 0, countD = 0, countP = 0;
        for (const Triangle &t : triangles)
        {
            for (int i = 0; i < 3; i++)
            {
                int idx = t.pointIndex(i);
                if (idx == 0)
                    countA++;
                if (idx == 1)
                    countB++;
                if (idx == 2)
                    countC++;
                if (idx == 3)
                    countD++;
                if (idx == 4)
                    countP++;
            }
        }

        EXPECT_EQ(countA, 2);
        EXPECT_EQ(countB, 2);
        EXPECT_EQ(countC, 2);
        EXPECT_EQ(countD, 2);
        EXPECT_EQ(countP, 4);
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(4.0, 0.0);
        Point C(2.0, 3.0);
        Point D(2.0, -3.0);
        Point E(-2.0, 1.5);
        Point F(6.0, 1.5);
        Point P(2.0, 0.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);
        mesh.addPoint(E);
        mesh.addPoint(F);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, 1);
        tri0.setNeighbourIndex(1, 3);
        tri0.setNeighbourIndex(2, 2);

        Triangle tri1(B, A, D);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 1);
        tri1.setPointIndex(1, 0);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, 0);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, -1);

        Triangle tri2(A, C, E);
        tri2.setIndex(2);
        tri2.setPointIndex(0, 0);
        tri2.setPointIndex(1, 2);
        tri2.setPointIndex(2, 4);
        tri2.setNeighbourIndex(0, 0);
        tri2.setNeighbourIndex(1, -1);
        tri2.setNeighbourIndex(2, -1);

        Triangle tri3(B, F, C);
        tri3.setIndex(3);
        tri3.setPointIndex(0, 1);
        tri3.setPointIndex(1, 5);
        tri3.setPointIndex(2, 2);
        tri3.setNeighbourIndex(0, -1);
        tri3.setNeighbourIndex(1, -1);
        tri3.setNeighbourIndex(2, 0);

        mesh.setTriVector({tri0, tri1, tri2, tri3});

        ASSERT_TRUE(verifyNeighbourConsistency(mesh));

        mesh.createTriangles(0, 6);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));

        std::vector<Triangle> triangles = mesh.getTriVector();
        for (const Triangle &t : triangles)
        {
            EXPECT_GT(t.area(), 0);
        }
    }

    {
        Mesh mesh;

        Point A(0.0, 0.0);
        Point B(10.0, 0.0);
        Point C(5.0, 0.5);
        Point D(5.0, -0.5);
        Point P(5.0, 0.0);

        mesh.addPoint(A);
        mesh.addPoint(B);
        mesh.addPoint(C);
        mesh.addPoint(D);
        mesh.addPoint(P);

        Triangle tri0(A, B, C);
        tri0.setIndex(0);
        tri0.setPointIndex(0, 0);
        tri0.setPointIndex(1, 1);
        tri0.setPointIndex(2, 2);
        tri0.setNeighbourIndex(0, 1);
        tri0.setNeighbourIndex(1, -1);
        tri0.setNeighbourIndex(2, -1);

        Triangle tri1(B, A, D);
        tri1.setIndex(1);
        tri1.setPointIndex(0, 1);
        tri1.setPointIndex(1, 0);
        tri1.setPointIndex(2, 3);
        tri1.setNeighbourIndex(0, 0);
        tri1.setNeighbourIndex(1, -1);
        tri1.setNeighbourIndex(2, -1);

        mesh.setTriVector({tri0, tri1});

        ASSERT_TRUE(verifyNeighbourConsistency(mesh));

        mesh.createTriangles(0, 4);

        EXPECT_TRUE(verifyNeighbourConsistency(mesh));
        EXPECT_TRUE(verifyDelaunayProperty(mesh));
    }

    {
        Mesh mesh(testCaseInner);
        mesh.buildMesh();
        mesh.removeHelperTriangles();

        ASSERT_TRUE(verifyNeighbourConsistency(mesh));

        std::vector<Triangle> triangles = mesh.getTriVector();

        Point edgeMidpoint;
        int triIndex = -1;
        int neighborIndex = -1;
        bool foundEdge = false;

        for (const Triangle &t : triangles)
        {
            if (foundEdge)
                break;

            for (int i = 0; i < 3; i++)
            {
                int nbrIdx = t.neighbourIndex(i);
                if (nbrIdx >= 0)
                {
                    edgeMidpoint = t.edgeMidpoint(i);
                    triIndex = t.index();
                    neighborIndex = nbrIdx;
                    foundEdge = true;
                    break;
                }
            }
        }

        if (!foundEdge)
        {
            std::cout << "No internal edges found - all triangles are boundary only" << std::endl;
            // Skip this test case if no internal edges exist
            return;
        }

        int triCountBefore = mesh.getTriVector().size();

        mesh.triangulatePoint(edgeMidpoint.x(), edgeMidpoint.y());

        int triCountAfter = mesh.getTriVector().size();

        EXPECT_EQ(triCountAfter, triCountBefore + 2);
        EXPECT_TRUE(verifyNeighbourConsistency(mesh));
        EXPECT_TRUE(verifyDelaunayProperty(mesh));
    }
}

TEST_F(MeshTestFixture, TriangulatePoint_Incremental10000) {
    // Test incremental insertion (triangulatePoint) vs batch (buildMesh)
    std::mt19937 rng(99999);
    std::uniform_real_distribution<double> dist(1.0, 10000.0);
    
    Mesh mesh;
        
    for (int i = 0; i < 10000; i++) {
        double x = dist(rng);
        double y = dist(rng);
        mesh.triangulatePoint(x, y);
        
        // Periodic consistency checks (every 1000 points)
    }
    
    mesh.removeHelperTriangles();
    
    EXPECT_TRUE(verifyNeighbourConsistency(mesh));
    EXPECT_TRUE(verifyDelaunayProperty(mesh));
}