#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"
#include <vector>
#include <map>

class Triangle {
public:
    Triangle();
    Triangle(Point p1, Point p2, Point p3);

    double getLength(int side);
    double getPerimeter();
    double getAng(int angle);
    double getArea();
    bool contains(Point pTargetPoint);
    int findPathToContainingTriangle(Point pTargetPoint);

    Point getPoint(int nPoint);
    void printPoints();
    Point getCircumcenter();
    bool isInCircumcircle(Point p);

    int getNeighbourIndex(int index);
    void setNeighbourIndex(int index, int value);
    int getPointIndex(int index);
    void setPointIndex(int index, int value);

private:
    Point p1, p2, p3;
    int neighbourIndices[3];
    int pointIndices[3];
};


#endif // TRIANGLE_H
