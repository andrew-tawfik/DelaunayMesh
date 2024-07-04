#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"
#include <vector>
#include <map>

class Triangle {
private:
    Point p1, p2, p3;
    int neighbourIndices[3];
    int pointIndices[3];

public:
    Triangle();
    Triangle(Point p1, Point p2, Point p3);

    Point getPoint(int nPoint);
    double getLength(char cSide);
    double getAng(char cAngle);
    double getArea();
    double getPerimeter();
    bool contains(Point pTargetPoint);
    void printPoints();
    Point getCircumcenter();
    bool isInCircumcircle(Point p);
    //-------------------------------------------------------------------------
    int getNeighbourIndex(int index);
    void setNeighbourIndex(int index, int value);

    int getPointIndex(int index);
    void setPointIndex(int index, int value);
    //-------------------------------------------------------------------------

    bool operator==(Triangle& other);
    std::vector<Triangle> neighbours(std::vector<Triangle> vecTriangles);


};

#endif // TRIANGLE_H
