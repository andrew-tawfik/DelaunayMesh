#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"
#include <vector>
#include <map>

class Triangle {
public:
    Triangle();
    Triangle(Point pt0, Point pt1, Point pt2);

    double getLength(int iSide);
    double getPerimeter();
    double getAng(int iAngle);
    double getArea();
    bool contains(Point ptTargetPoint);
    int findPathToContainingTriangle(Point ptTargetPoint);

    Point getPoint(int iPoint);
    void printPoints();
    Point getCircumcenter();
    bool isInCircumcircle(Point pt);

    int getNeighbourIndex(int iIndex);
    void setNeighbourIndex(int iIndex, int iValue);
    int getPointIndex(int iIndex);
    void setPointIndex(int iIndex, int iValue);

private:
    Point pt0, pt1, pt2;
    int aiNeighbourIndices[3];
    int aiPointIndices[3];
};

#endif // TRIANGLE_H
