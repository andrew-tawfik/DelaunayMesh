#include "../include/mesh.h"
#include "../include/triangle.h"
#include "../include/point.h"
#include <gtest/gtest.h>

// === Unit Test Definitions ===

// 1. Constructor and Initialization
TEST(MeshTest, Constructor_ValidPoints)
{
    std::vector<Point> points = { Point(0, 0), Point(1, 0), Point(0, 1)};
    Mesh mesh(points);
    EXPECT_EQ(mesh.getShape(), points);
}

TEST(MeshTest, Constructor_EmptyPoints)
{
    std::vector<Point> points;
    Mesh mesh(points);
    EXPECT_TRUE(mesh.getShape().empty());
}

// 2. Core Mesh Methods
TEST(MeshTest, BuildMesh_ValidPoints)
{
    std::vector<Point> points = { Point(1, 1), Point(2, 1), Point(1, 2), Point(2, 2)};
    Mesh mesh(points);
    mesh.setTriVector({ mesh.superTriangle() });
    mesh.buildMesh();
    EXPECT_FALSE(mesh.getTriVector().empty());
}

TEST(MeshTest, BuildMesh_EmptyPoints)
{
    std::vector<Point> points;
    Mesh mesh(points);
    mesh.buildMesh();
    EXPECT_TRUE(mesh.getTriVector().empty());
}

TEST(MeshTest, BuildMesh_LargeInput) {
    std::vector<Point> testCaseRect = {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
        Point(22.5, 7.0), Point(22.5, 12.0),Point(15.0, 12.0),
        Point(7.5, 12.0),Point(7.5, 7.0)
    };
    Mesh mesh(testCaseRect);
    mesh.setTriVector({ mesh.superTriangle() });
    mesh.buildMesh();

    const auto& m = mesh.getTriVector();
    EXPECT_EQ(m.size(), 17);

    for (Triangle t : m)
    {
        for(int i = 0; i < 3; i++) {

            int iNeighbourIndex = t.getNeighbourIndex(i);

            if(iNeighbourIndex > -1)
            {
                EXPECT_TRUE(mesh.areNeighbours(t.getIndex(), iNeighbourIndex));
            }
        }
    }

}

TEST(MeshTest, RemoveHelperTriangles_RemovesCorrectly) {
    // Implement the test here
}

// 4. Triangle Operations
TEST(MeshTest, FindContainingTriangle_PointInsideTriangle) {
    std::vector<Point> testCaseRect = {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
        Point(22.5, 7.0), Point(22.5, 12.0),Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7.0)
    };
    Point targetPoint = Point(15, 7);
    Mesh mesh(testCaseRect);
    mesh.setTriVector({ mesh.superTriangle()});
    mesh.buildMesh();
    const auto& m = mesh.getTriVector();
    int foundTriangle = mesh.findContainingTriangle(targetPoint);
    EXPECT_TRUE( m[foundTriangle].contains(targetPoint) );
}

TEST(MeshTest, FindContainingTriangle_PointOutsideTriangles) {
    Point targetPoint = Point(25, 26);
    std::vector <Point> points;
    Mesh mesh(points);
    mesh.setTriVector({ mesh.superTriangle()});
    int foundTriangle = mesh.findContainingTriangle(targetPoint);
    EXPECT_EQ(foundTriangle, -1);
}

TEST(MeshTest, FindContainingTriangle_PointOnEdge) {
    std::vector<Point> testCaseRect = {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
        Point(22.5, 7.0), Point(22.5, 12.0),Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7.0)
    };
    int count = 0;
    Point edgePoint;

    while(count < 10) {
        Mesh mesh(testCaseRect);
        mesh.setTriVector({ mesh.superTriangle()});
        mesh.buildMesh();
        mesh.removeHelperTriangles();
        const auto& m = mesh.getTriVector();
        int randomTriangle = rand() % m.size();
        int randomNeighbour = -1;

        for(int i = 0; i < 3; i++) {
            if (m[randomTriangle].getNeighbourIndex(i) > -1) {
                randomNeighbour = m[randomTriangle].getNeighbourIndex(i);
                edgePoint = m[randomTriangle].getEdgeMidpoint(i);
                break;
            }
        }
        int foundTriangle = mesh.findContainingTriangle(edgePoint);
        EXPECT_TRUE(foundTriangle == randomTriangle || foundTriangle == randomNeighbour);
        count++;
    }
}

TEST(MeshTest, FindContainingTriangle_ForEdgeMidpoint) {
    // Arrange: Define the input points
    std::vector<Point> testCaseRect = {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
        Point(22.5, 7.0), Point(22.5, 12.0), Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7.0)
    };

    // Setup the mesh
    Mesh mesh(testCaseRect);
    mesh.setTriVector({ mesh.superTriangle() });
    mesh.buildMesh();
    mesh.removeHelperTriangles();

    // Get triangles
    const auto& triangles = mesh.getTriVector();
    ASSERT_FALSE(triangles.empty());

    // Test each triangle's edges
    for (Triangle t : triangles) {
        for (int j = 0; j < 3; ++j) {
            int neighborIndex = t.getNeighbourIndex(j);
            if (neighborIndex > -1) {
                Point edgePoint = t.getEdgeMidpoint(j);

                // Validate the edge point is found in the correct triangle or neighbor
                int foundTriangle = mesh.findContainingTriangle(edgePoint);
                EXPECT_TRUE(foundTriangle == t.getIndex() || foundTriangle == neighborIndex);
            }
        }
    }
}

TEST(MeshTest, CreateTriangles_TwoNew) {
    std::vector<Point> testCaseOne = {
        Point(7.5, 2.0)
    };
    Mesh mesh(testCaseOne);
    mesh.setTriVector({ mesh.superTriangle()});
    EXPECT_EQ(mesh.getTriVector().size(), 1);
    mesh.createTriangles(0, 0);
    EXPECT_EQ(mesh.getTriVector().size(), 3);
}

TEST(MeshTest, CreateTriangles_InvalidIndices) {
    std::vector<Point> testCaseOne = {Point(7.5, 2.0)};
    Mesh mesh(testCaseOne);
    mesh.setTriVector({ mesh.superTriangle()});

    EXPECT_NO_THROW(mesh.createTriangles(-1, 0));
    EXPECT_NO_THROW(mesh.createTriangles(0, 10));
}

TEST(MeshTest, MeshTest_CreateTriangles_OutsideMesh) {
    std::vector<Point> testCaseOne = {Point(30, 30)};
    Mesh mesh(testCaseOne);
    mesh.setTriVector({ mesh.superTriangle()});

    EXPECT_NO_THROW(mesh.createTriangles(0, 0));
}

TEST(MeshTest, MeshTest_CreateTriangles_Adjacency) {
    std::vector<Point> testCaseRect = {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
        Point(22.5, 7.0), Point(22.5, 12.0), Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7.0)
    };
    Mesh mesh(testCaseRect);
    mesh.setTriVector({ mesh.superTriangle()});

    int iPointIndex = 0;

    for (auto point : mesh.getShape())
    {
        // Find the triangle that contains the current point
        int iTriIndex = mesh.findContainingTriangle(point);

        // Create new triangles from the found triangle and current point
        mesh.createTriangles(iTriIndex, iPointIndex);

        iPointIndex += 1;  // Move to the next point

        // Stop processing when the last three points (super triangle points) are reached
        if ((mesh.getShape().size() - 3) == iPointIndex) { break; }
    }
    for(auto triangle : mesh.getTriVector()) {
        for(int i = 0; i < 3; i++) {
            int neighbourIndex = mesh.getTriVector()[triangle.getIndex()].getNeighbourIndex(i);
            if (neighbourIndex > -1) {
                EXPECT_TRUE(mesh.areNeighbours(triangle.getIndex(), neighbourIndex));
            }
        }
    }
}

// 6. Edge Cases and Boundary Tests
TEST(MeshTest, BuildMesh_HandlesEmptyInput) {
    // Implement the test here
}

TEST(MeshTest, SetShape_HandlesEmptyInput) {
    // Implement the test here
}

TEST(MeshTest, FindContainingTriangle_PointOnBoundary) {
    // Implement the test here
}

TEST(MeshTest, SuperTriangle_HandlesEdgeCase) {
    // Implement the test here
}

// 7. Performance and Stress Tests
TEST(MeshTest, BuildMesh_PerformanceOnLargeInput) {
    // Implement the test here
}

TEST(MeshTest, EquilateralizeTriangles_StressTest) {
    // Implement the test here
}
