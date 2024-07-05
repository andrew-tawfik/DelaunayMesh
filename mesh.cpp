#include "mesh.h"
#include <iostream>
#include "triangle.h"
#include "point.h"
#include <math.h>
#include <cmath>

Mesh::Mesh()
{
    Triangle triInitialTriangle = Triangle(Point(0.0, 0.0), Point(30.0, 0.0), Point(15.0, 25.981));
    vecTriangles.push_back(triInitialTriangle);
}

void Mesh::setPoints(std::vector<Point> vecPoints)
{
    this->vecPoints = vecPoints;
}

std::vector<Point> Mesh::getPoints()
{
    return vecPoints;
}




void Mesh::buildMesh()
{
    std::vector<Triangle> vecNewTriangles;

    for (auto p = vecPoints.begin(); p != vecPoints.end(); ++p)
    {
        vecNewTriangles.clear();
        for (auto t = vecTriangles.begin(); t != vecTriangles.end();)
        {
            if (t->contains(*p))
            {
                Triangle tri1 = Triangle(t->getPoint(1), t->getPoint(2), *p);
                Triangle tri2 = Triangle(t->getPoint(1), t->getPoint(3), *p);
                Triangle tri3 = Triangle(t->getPoint(2), t->getPoint(3), *p);

                std::cout << "Three new triangles have been created: " << std::endl;

                std::cout << "Triangle 1 has coordinates: ";
                tri1.printPoints();
                vecNewTriangles.push_back(tri1);

                std::cout << "Triangle 2 has coordinates: ";
                tri2.printPoints();
                vecNewTriangles.push_back(tri2);

                std::cout << "Triangle 3 has coordinates: ";
                tri3.printPoints();
                vecNewTriangles.push_back(tri3);

                // Remove the current triangle
                t = vecTriangles.erase(t);

                // Handle neighboring triangles


            }
            else
            {
                ++t;
            }
        }

        vecTriangles.insert(vecTriangles.end(), vecNewTriangles.begin(), vecNewTriangles.end());
        vecTriangles = sortTriangles(vecTriangles);
        std::cout << vecTriangles.size() << std::endl;
    }

    for (auto t = vecTriangles.begin(); t != vecTriangles.end(); ++t)
    {
        t->printPoints();
    }
}
std::vector<Triangle> Mesh::sortTriangles(std::vector<Triangle> vecTri)
{
    std::vector<Triangle> vecSortedTriangles = vecTri;

    for (size_t i = 1; i < vecSortedTriangles.size(); ++i)
    {
        Triangle triKey = vecSortedTriangles[i];
        size_t j = i - 1;

        while (j >= 0 && vecSortedTriangles[j].getArea() > triKey.getArea())
        {
            vecSortedTriangles[j + 1] = vecSortedTriangles[j];
            j = j - 1;
        }
        vecSortedTriangles[j + 1] = triKey;
    }

    return vecSortedTriangles;
}

void Mesh::flipEdge(Triangle triTriangle1, Triangle triTriangle2, Point p)
{
    Point pCommon1;
    Point pCommon2;
    Point pOther1;
    Point pOther2;

    for (int i = 0; i < 3; ++i)
    {
        if (triTriangle1.getPoint(i).equals(triTriangle2.getPoint(0)))
        {
            pCommon1 = triTriangle1.getPoint(i);
            pCommon2 = triTriangle2.getPoint(0);
            pOther1 = triTriangle1.getPoint((i + 1) % 3);
            pOther2 = triTriangle1.getPoint((i + 2) % 3);
            break;
        }
        else if (triTriangle1.getPoint(i).equals(triTriangle2.getPoint(1)))
        {
            pCommon1 = triTriangle1.getPoint(i);
            pCommon2 = triTriangle2.getPoint(1);
            pOther1 = triTriangle1.getPoint((i + 1) % 3);
            pOther2 = triTriangle1.getPoint((i + 2) % 3);
            break;
        }
        else if (triTriangle1.getPoint(i).equals(triTriangle2.getPoint(2)))
        {
            pCommon1 = triTriangle1.getPoint(i);
            pCommon2 = triTriangle2.getPoint(2);
            pOther1 = triTriangle1.getPoint((i + 1) % 3);
            pOther2 = triTriangle1.getPoint((i + 2) % 3);
            break;
        }
    }

    triTriangle1 = Triangle(pCommon1, pOther1, p);
    triTriangle2 = Triangle(pCommon2, pOther2, p);
    vecTriangles.push_back(triTriangle1);
    vecTriangles.push_back(triTriangle2);
}







