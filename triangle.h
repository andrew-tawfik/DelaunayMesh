#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"
#include <vector>
#include <map>

// Class representing a triangle defined by three points
class Triangle {
public:
    // Default constructor
    Triangle();

    // Constructor to initialize a triangle with three points
    Triangle(Point pt0, Point pt1, Point pt2);

    // Function to get the length of a specified side
    double getLength(int iSide);

    // Function to get the perimeter of the triangle
    double getPerimeter();

    // Function to get a specified angle in the triangle
    double getAng(int iAngle);

    // Function to get the area of the triangle
    double getArea();

    // Function to check if the triangle contains a given point
    bool contains(Point ptTargetPoint);

    // Function to find the path to the triangle containing a given point
    int findPathToContainingTriangle(Point ptTargetPoint);

    // Function to get a specified point of the triangle
    Point getPoint(int iPoint);

    // Function to print the points of the triangle
    void printPoints();

    // Function to get the circumcenter of the triangle
    Point getCircumcenter();

    // Function to check if a point is inside the circumcircle of the triangle
    bool isInCircumcircle(Point pt);

    // Getter and setter for neighboring triangle indices
    int getNeighbourIndex(int iIndex);
    void setNeighbourIndex(int iIndex, int iValue);

    // Getter and setter for point indices
    int getPointIndex(int iIndex);
    void setPointIndex(int iIndex, int iValue);

private:
    Point pt0, pt1, pt2;  // Points defining the triangle
    int aiNeighbourIndices[3];  // Indices of neighboring triangles
    int aiPointIndices[3];  // Indices of points in the triangle
};

#endif // TRIANGLE_H
