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

    Triangle triTest = Triangle(Point(15, 2), Point(22.5, 7), Point(22.5, 12));
    Point p1 = Point(22.5, 10);

    std::cout << triTest.onEdge(p1) << std::endl;

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

    // Test createTriangles
    Mesh k(vecPtRectangle);

    k.setTriVector({ k.superTriangle() });

    //k.buildMesh();


    k.createTriangles(0, 3);

    k.createTriangles(2, 4);
    //k.checkNeighboringCircumcircles(2, 4, 2);

    k.createTriangles(3, 5);

    k.createTriangles(5, 6);
    //k.checkNeighboringCircumcircles(8, 6, 0);

    k.createTriangles(7, 7);
    //k.checkNeighboringCircumcircles(10, 7, 0);

    k.createTriangles(6, 8);

    k.createTriangles(0, 9);

    k.createTriangles(4, 10);



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
