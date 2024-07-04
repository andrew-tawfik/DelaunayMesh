#include "point.h"
#include "triangle.h"
#include <iostream>
#include "mesh.h"

int main() {
    Point p1(0.0, 0.0), p2(30.0, 0.0), p3(15.0, 25.981);

    Triangle tri1(p2, p1, p3);

    std::cout << "The length of the side 'a' is: " << tri1.getLength('a') << std::endl;
    std::cout << "The length of the side 'b' is: " << tri1.getLength('b') << std::endl;
    std::cout << "The length of the side 'c' is: " << tri1.getLength('c') << std::endl;

    std::cout << std::endl;

    std::cout << "The perimeter of the Triangle is: " << tri1.getPerimeter() << std::endl;
    std::cout << "The area of the Triangle is: " << tri1.getArea() << std::endl;

    std::cout << std::endl;

    std::cout << "Angle A: " << tri1.getAng('A') << std::endl;
    std::cout << "Angle B: " << tri1.getAng('B') << std::endl;
    std::cout << "Angle C: " << tri1.getAng('C') << std::endl;
    std::cout << "Sum of all angles in this triangle is: " << tri1.getAng('A') + tri1.getAng('B') + tri1.getAng('C') << std::endl;

    std::cout << p1.findSlope(p2) << std::endl;
    std::cout << p1.betweenPoints(p2, Point(2.0, 2.5)) << std::endl;


    std::vector<Point> vecPoints = {
        Point(7.5, 2.0), Point(15.0, 2.0),



        Point(22.5, 2.0), Point(22.5, 7.0),
        Point(22.5, 12.0), Point(15.0, 12.0),
        Point(7.5, 12.0), Point(7.5, 7)


    };
    /*
    Mesh m;
    m.setPoints(vecPoints);
    m.buildMesh();

    Point p4(5.0, 5.0), p5(20.0, 20.0);

    Triangle tri2(p1, p2, p4), tri3(p1, p3, p4), tri4(p2, p3, p4), tri5(p1, p2, p5);

    std::vector<Triangle> vecTriangleVector = {tri1, tri2, tri3, tri4, tri5};

    // Print the areas of the triangles
    for (auto& triangle : vecTriangleVector) {
        triangle.printPoints();
    }

    std::vector<Triangle> vecSortedTriangles = m.sortTriangles(vecTriangleVector);

    // Print the sorted triangles
    std::cout << "Sorted triangles:" << std::endl;
    for (auto& triangle : vecSortedTriangles) {
        triangle.printPoints();
    }

    std::vector<Triangle> vecTriList;
    Triangle tri7(Point(0, 0), Point(3, 0), Point(0, 4));

    Triangle tri8(Point(0, 4), Point(3, 0), Point(3, 4));
    vecTriList.push_back(tri8);

    Triangle tri9(Point(0, 0), Point(2, 2), Point(0, 4));
    vecTriList.push_back(tri9);

    Triangle tri10(Point(0, 0), Point(3, 4), Point(0, 2));
    vecTriList.push_back(tri10);

    for (auto& triNeighbour : tri7.neighbours(vecTriList)) {
        triNeighbour.printPoints();
    }

    std::cout << tri7.neighbours(vecTriList).size() << std::endl;

    Triangle tri6 = Triangle(Point(15, 25.981), Point(0, 0), Point(7.5, 2));

    if (tri6.isInCircumcircle(Point(15, 2))) {
        std::cout << "Yes" << std::endl;
    }
*/
    return 0;
}
