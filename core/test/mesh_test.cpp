
#include "../include/mesh.h"
#include "../include/triangle.h"
#include "../include/point.h"
#include <gtest/gtest.h>
#include <vector>

// === Unit Test Definitions ===
class MeshTestFixture : public ::testing::Test {
protected:
  
  std::vector<Point> testCaseRect { 
    Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0), 
    Point(22.5, 7.0), Point(22.5, 12.0),Point(15.0, 12.0), 
    Point(7.5, 12.0),Point(7.5, 7.0)
  };


  std::vector<Point> testCaseHex = {
      Point(22, 8.6), Point(18.5, 14.7), Point(11.5, 14.7),
      Point(8, 8.6), Point(11.5, 2.5), Point(18.5, 2.5)
  };

  std::vector<Point> testCaseInner = {
      Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0),
      Point(22.5, 7.0), Point(22.5, 12.0), Point(15.0, 12.0),
      Point(7.5, 12.0), Point(7.5, 7.0), Point(15, 7),
      Point(11.25, 4.5), Point(18.75, 9.5)
  };

  std::vector<Point> testCaseMinimal = {
      Point(0, 0), Point(10, 0), Point(5, 10)
  };

  std::vector<Point> testCaseSingle = {
      Point(5, 5)
  };


  bool verifyNeighbourConsistency(const Mesh& mesh) {
    bool isConsistent = false;
    std::vector<Triangle> triangles = mesh.getTriVector();
    for (const Triangle& t: triangles) {
      for (int i = 0; i < 3; i++) {
        const int neighbourIndex = t.getNeighbourIndex(i);

        if (neighbourIndex == -1) {
            continue;
        }

        // Neighbor index out of bounds
        if (neighbourIndex < 0 || neighbourIndex >= triangles.size()) {
            return false;
        }
        
        // Verify bidirectional: neighbor must point back to us
        const Triangle& neighbour = triangles[neighbourIndex];
        bool neighbourPointsBack = false;
        
        for (int j = 0; j < 3; j++) {
            if (neighbour.getNeighbourIndex(j) == t.getIndex()) {
                neighbourPointsBack = true;
                break;
            }
        }
        
        if (!neighbourPointsBack) {
            return false;
        }
      } 
    }
    return true;
  }

  bool verifyDelaunayProperty(const Mesh& mesh, int* failTriangle = nullptr, int* failPoint = nullptr) {
    std::vector<Triangle> triangles = mesh.getTriVector();
    std::vector<Point> points = mesh.getPtVector();
    
    for (const Triangle& t : triangles) {
        int vertexIndices[3] = {
            t.getPointIndex(0),
            t.getPointIndex(1),
            t.getPointIndex(2)
        };
        
        for (int i = 0; i < points.size(); i++) {
            if (i == vertexIndices[0] || i == vertexIndices[1] || i == vertexIndices[2]) {
                continue;
            }
            
            if (t.isInCircumcircle(points[i])) {
                if (failTriangle) *failTriangle = t.getIndex();
                if (failPoint) *failPoint = i;
                return false;
            }
        }
    }
    
    return true;
  }
};

TEST_F(MeshTestFixture, NeighborConsistencyInvariant) {
  
  
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
TEST_F(MeshTestFixture, DelaunayPropertyHolds) {
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
TEST_F(MeshTestFixture, PointInside_ThreeValidTriangles) {

}
TEST_F(MeshTestFixture, SwapEdge_RestoresDelaunay) {

}
TEST_F(MeshTestFixture, FindContainingTriangle_Correct) {

}
TEST_F(MeshTestFixture, PointOnEdge_FourTriangles) {

}
TEST_F(MeshTestFixture, AllInputPointsAreVertices) {
    
}
TEST_F(MeshTestFixture, SwapEdge_NeighborConsistency) {

}
TEST_F(MeshTestFixture, TriggersSwapWhenNeeded) {

} 
TEST_F(MeshTestFixture, FindsAllViolations) {

}
TEST_F(MeshTestFixture, InvalidIndex_NoCorruption) {

}
TEST_F(MeshTestFixture, AreNeighbours_Correct) {

}
TEST_F(MeshTestFixture, HandlesNegativePointIndices) {

}
TEST_F(MeshTestFixture, RectangleTestCase_Count) {

}
TEST_F(MeshTestFixture, SequentialCalls_Consistent) {

}
