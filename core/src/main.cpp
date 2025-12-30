#include "point.h"
#include "triangle.h"
#include "mesh.h"
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    // The following vectors are test cases showcasing different test cases
    std::vector<Point> testCaseHex { Point(22, 8.6), Point(18.5, 14.7), Point(11.5, 14.7), Point(8, 8.6), Point(11.5, 2.5), Point(18.5, 2.5) };

    std::vector<Point> testCaseRect { Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0), Point(22.5, 7.0), Point(22.5, 12.0),Point(15.0, 12.0), Point(7.5, 12.0),Point(7.5, 7.0) };

    std::vector<Point> testCaseInner { Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0), Point(22.5, 7.0), Point(22.5, 12.0), Point(15.0, 12.0), Point(7.5, 12.0), Point(7.5, 7.0), Point(15, 7), Point(11.25, 4.5), Point(18.75, 9.5), Point(18.75, 4.5), Point(11.25, 9.5), Point(11.25, 7), Point(18.75, 7) };

    std::vector<Point> testCasePicture { Point(11.0, 13.0), Point(13.0, 13.0), Point(17.0, 13.0), Point(19.0, 13.0), Point(9.0, 11.0), Point(15.0, 11.0), Point(21.0, 11.0),Point(21.0, 9.0), Point(9.0, 9.0), Point(11.0, 7.0), Point(19.0, 7.0), Point(15.0, 3.0) };

    // Mesh Workflow
    Mesh m;
    for (Point p : testCaseRect) {
        m.triangulatePoint(p.x(), p.y());
    }

    std::vector<Triangle> meshTriangles = m.triangles();

    // Prints the mesh and each triangles' neighbours
    for (Triangle t : meshTriangles)
    {
        std::cout << "Triangle " << t.index() << ": ";
        t.printPoints();
        std::cout << "\ta) Neighbour at index:  " << t.neighbourIndex(0) << std::endl;
        std::cout << "\tb) Neighbour at index:  " << t.neighbourIndex(1) << std::endl;
        std::cout << "\tc) Neighbour at index:  " << t.neighbourIndex(2) << std::endl;
    }

    return 0;
}