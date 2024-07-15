#include "point.h"
#include "triangle.h"
#include <iostream>
#include "mesh.h"

int main()
{

    // Triangle Class Testing

    // Create points for the triangles
    Point pt0(10, 10);
    Point pt1(13, 10);
    Point pt2(13, 14);
    Point pt3(16, 10);
    Point pt4(13, 6);

    // Create Triangle instances
    Triangle tri0(pt0, pt1, pt2);
    Triangle tri1(pt1, pt3, pt2); // Neighbor to the right
    Triangle tri2(pt0, pt4, pt1); // Neighbor below

    tri0.setIndex(0);
    tri1.setIndex(1);
    tri2.setIndex(2);

    // Set neighbor indices
    tri0.setNeighbourIndex(0, 2); // Side 0 of tri0 is neighbor to tri2
    tri0.setNeighbourIndex(1, 1); // Side 1 of tri0 is neighbor to tri1


    tri1.setNeighbourIndex(2, 0); // Side 0 of tri1 is neighbor to tri0

    tri2.setNeighbourIndex(2, 0); // Side 0 of tri2 is neighbor to tri0

    std::vector <Triangle> vecTriangles;
    vecTriangles.push_back(tri0);
    vecTriangles.push_back(tri1);
    vecTriangles.push_back(tri2);

    // Print initial triangles
    std::cout << "Initial Triangles:" << std::endl;
    tri0.printPoints();
    tri1.printPoints();
    tri2.printPoints();

    // Test getLength
    std::cout << "\nLengths of the sides:" << std::endl;
    for (int nSide = 0; nSide < 3; ++nSide)
    {
        std::cout << "Triangle 0, Side " << nSide << ": " << tri0.getLength(nSide) << std::endl;
        std::cout << "Triangle 1, Side " << nSide << ": " << tri1.getLength(nSide) << std::endl;
        std::cout << "Triangle 2, Side " << nSide << ": " << tri2.getLength(nSide) << std::endl;
    }

    // Test getPerimeter
    std::cout << "\nPerimeters:" << std::endl;
    std::cout << "Triangle 0: " << tri0.getPerimeter() << std::endl;
    std::cout << "Triangle 1: " << tri1.getPerimeter() << std::endl;
    std::cout << "Triangle 2: " << tri2.getPerimeter() << std::endl;

    // Test getArea
    std::cout << "\nAreas:" << std::endl;
    std::cout << "Triangle 0: " << tri0.getArea() << std::endl;
    std::cout << "Triangle 1: " << tri1.getArea() << std::endl;
    std::cout << "Triangle 2: " << tri2.getArea() << std::endl;

    // Test getAng
    std::cout << "\nAngles:" << std::endl;
    for (int nAngle = 0; nAngle < 3; ++nAngle)
    {
        std::cout << "Triangle 0, Angle " << nAngle << ": " << tri0.getAng(nAngle) << " degrees" << std::endl;
        std::cout << "Triangle 1, Angle " << nAngle << ": " << tri1.getAng(nAngle) << " degrees" << std::endl;
        std::cout << "Triangle 2, Angle " << nAngle << ": " << tri2.getAng(nAngle) << " degrees" << std::endl;
    }

    // Test contains
    Point ptInside1(11, 11);
    Point ptInside2(14, 11);
    Point ptInside3(11, 9);
    Point ptOutside(15, 15);
    std::cout << "\nContains point (11, 11): " << tri0.contains(ptInside1) << std::endl;
    std::cout << "Contains point (14, 11): " << tri1.contains(ptInside2) << std::endl;
    std::cout << "Contains point (11, 9): " << tri2.contains(ptInside3) << std::endl;
    std::cout << "Contains point (15, 15): " << tri0.contains(ptOutside) << std::endl;

    // Test findPathToContainingTriangle
    std::cout << "\nPath to containing triangle for point (11, 11): " << tri0.findPathToContainingTriangle(ptInside1) << std::endl;
    std::cout << "Path to containing triangle for point (14, 11): " << tri0.findPathToContainingTriangle(ptInside3) << std::endl;
    std::cout << "Path to containing triangle for point (11, 9): " << tri2.findPathToContainingTriangle(ptInside1) << std::endl;

    // Test getCircumcenter
    Point ptCircumcenter1 = tri0.getCircumcenter();
    Point ptCircumcenter2 = tri1.getCircumcenter();
    Point ptCircumcenter3 = tri2.getCircumcenter();
    std::cout << "\nCircumcenter of Triangle 0: (" << ptCircumcenter1.getX() << ", " << ptCircumcenter1.getY() << ")" << std::endl;
    std::cout << "Circumcenter of Triangle 1: (" << ptCircumcenter2.getX() << ", " << ptCircumcenter2.getY() << ")" << std::endl;
    std::cout << "Circumcenter of Triangle 2: (" << ptCircumcenter3.getX() << ", " << ptCircumcenter3.getY() << ")" << std::endl;

    // Test isInCircumcircle
    std::cout << "\nPoint (11, 11) in circumcircle of Triangle 0: " << tri0.isInCircumcircle(ptInside1) << std::endl;
    std::cout << "Point (14, 11) in circumcircle of Triangle 1: " << tri1.isInCircumcircle(ptInside2) << std::endl;
    std::cout << "Point (11, 9) in circumcircle of Triangle 2: " << tri2.isInCircumcircle(ptInside3) << std::endl;

    // Print points of each triangle
    std::cout << "\nTriangle points:" << std::endl;
    std::cout << "Triangle 0: ";
    tri0.printPoints();
    std::cout << "Triangle 1: ";
    tri1.printPoints();
    std::cout << "Triangle 2: ";
    tri2.printPoints();


    // Mesh Class Testing

    //Points form a rectangle
    std::vector<Point> vecPtRectangle =
    {
        Point(7.5, 2.0), Point(15.0, 2.0),
        Point(22.5, 2.0), Point(22.5, 7.0),
        Point(22.5, 12.0), Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7)
    };

    //Create mesh object of a Rectangle
    Mesh m(vecPtRectangle);

    // Test getPoints
    std::cout << "\nMesh Points:" << std::endl;
    std::vector<Point> meshPoints = m.getShape();
    for (const auto& point : meshPoints)
    {
        std::cout << "(" << point.getX() << ", " << point.getY() << ")" << std::endl;
    }

    // Test getPointIndices
    std::cout << "\nMesh Point Indices:" << std::endl;
    std::vector<int> meshPointIndices = m.getPointIndices();
    for (int index : meshPointIndices)
    {
        std::cout << index << " ";
    }
    std::cout << std::endl;

    // Test getTriangleIndices
    std::cout << "\nMesh Triangle Indices:" << std::endl;
    std::vector<int> meshTriangleIndices = m.getTriangleIndices();
    for (int index : meshTriangleIndices)
    {
        std::cout << index << " ";
    }
    std::cout << std::endl;


    // Test findContainingTriangle
    m.setTriVector(vecTriangles);
    std::cout << m.findContainingTriangle(ptInside1) << std::endl;



    return 0;
}
