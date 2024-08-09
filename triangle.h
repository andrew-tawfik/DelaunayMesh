#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"
#include <vector>
#include <map>

// Class representing a triangle defined by three points
class Triangle {
private:
    Point pt0, pt1, pt2;  // Points defining the triangle
    int iIndex; // Index of the triangle
    int aiNeighbourIndices[3];  // Indices of neighboring triangles
    int aiPointIndices[3];  // Indices of points in the triangle

public:
    // Default constructor
    Triangle();

    // Constructor to initialize a triangle with three points
    Triangle(const Point& pt0, const Point& pt1, const Point& pt2);

    // Getter and setter for neighboring triangle indices
    int getNeighbourIndex(int iIndex) const;
    void setNeighbourIndex(int iIndex, int iValue);

    // Getter and setter for point indices
    int getPointIndex(int iIndex) const;
    int getPointIndex() const;
    void setPointIndex(int iIndex, int iValue);

    int getIndex() const;
    void setIndex(int iValue);

    // Function to get the length of a specified side
    double getLength(int iSide) const;

    // Function to get the perimeter of the triangle
    double getPerimeter() const;

    // Function to get a specified angle in the triangle
    double getAng(int iAngle)const;

    // Function to get the area of the triangle
    double getArea() const;

    // Function to check if the triangle contains a given point
    bool contains(const Point& ptTargetPoint) const;

    // Function to find the path to the triangle containing a given point
    int findPathToContainingTriangle(const Point& ptTargetPoint) const;

    // Function to get a specified point of the triangle
    Point getPoint(int iPoint) const;

    // Function to set new points based
    void setPoint(int iPoint, const Point& newPoint);

    // Function to print the points of the triangle
    void printPoints() const;

    // Function to get the circumcenter of the triangle
    Point getCircumcenter() const;

    // Function to check if a point is inside the circumcircle of the triangle
    bool isInCircumcircle(const Point& pt) const;

    int onEdge(const Point& pt) const;

    Point getCentroid() const;

};

#endif // TRIANGLE_H
