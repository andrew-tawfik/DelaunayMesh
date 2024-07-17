#include "point.h"
#include "triangle.h"
#include "mesh.h"
#include <iostream>
#include <vector>

void printTriangleDetails(const Triangle& tri, int triIndex)
{
    std::cout << "Triangle " << triIndex << ": ";
    tri.printPoints();
    std::cout << "Perimeter: " << tri.getPerimeter() << std::endl;
    std::cout << "Area: " << tri.getArea() << std::endl;
    std::cout << "Angles: ";
    for (int i = 0; i < 3; ++i)
    {
        std::cout << tri.getAng(i) << " ";
    }

    std::cout << std::endl;
    std::cout << "Circumcenter: (" << tri.getCircumcenter().getX() << ", " << tri.getCircumcenter().getY() << ")\n" << std::endl;
}

int main()
{
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

    // Print initial triangles and details
    std::cout << "Initial Triangles:\n";
    for (int i = 0; i < vecTriangles.size(); ++i)
    {
        printTriangleDetails(vecTriangles[i], i);
    }

    // Test point containment and paths
    std::vector<Point> testPoints =
    {
        Point(11, 11), Point(14, 11), Point(11, 9), Point(15, 15)
    };
    std::cout << "Contains Points Test:\n";
    for (const auto& pt : testPoints)
    {
        std::cout << "Point (" << pt.getX() << ", " << pt.getY() << ")\n";
        for (int i = 0; i < vecTriangles.size(); ++i) {
            std::cout << " - In Triangle " << i << ": " << vecTriangles[i].contains(pt) << std::endl;
        }
    }

    // Test circumcircle containment
    std::cout << "\nCircumcircle Containment Test:\n";
    for (const auto& pt : testPoints)
    {
        for (int i = 0; i < vecTriangles.size(); ++i)
        {
            std::cout << " - Point (" << pt.getX() << ", " << pt.getY() << ") in circumcircle of Triangle " << i << ": " << vecTriangles[i].isInCircumcircle(pt) << std::endl;
        }
    }

    // Mesh Class Testing
    std::vector<Point> vecPtRectangle =
    {
        Point(7.5, 2.0),
        Point(15.0, 2.0),
        Point(22.5, 2.0),
        Point(22.5, 7.0),
        Point(22.5, 12.0),
        Point(15.0, 12.0),
        Point(7.5, 12.0),
        Point(7.5, 7.0)
    };

    Mesh m(vecPtRectangle);
    m.setTriVector(vecTriangles);

    // Test findContainingTriangle
    std::cout << "\nTarget point is inside Triangle at index: " ;
    std::cout << m.findContainingTriangle(testPoints[0]) << std::endl;

    // Test createTriangles
    Mesh k(vecPtRectangle);
    k.setTriVector({ k.superTriangle() });
    // k.buildMesh();


    k.createTriangles(0, 0);
    k.createTriangles(2, 1);
    k.createTriangles(3, 2);
    k.createTriangles(5, 3);
    k.createTriangles(7, 4);

    std::vector<Triangle> meshTriangles = k.getTriVector();
    std::cout << "\nThe size of the vector after the createTriangles operation: " << meshTriangles.size() << std::endl;

    for (int i = 0; i < meshTriangles.size(); ++i)
    {
        std::cout << "Triangle " << i << ": ";
        meshTriangles[i].printPoints();
        std::cout << "\ta) Neighbour at index:  " << meshTriangles[i].getNeighbourIndex(0) << std::endl;
        std::cout << "\tb) Neighbour at index:  " << meshTriangles[i].getNeighbourIndex(1) << std::endl;
        std::cout << "\tc) Neighbour at index:  " << meshTriangles[i].getNeighbourIndex(2) << std::endl;

    }

    return 0;
}
