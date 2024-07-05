#define _USE_MATH_DEFINES

#include <sstream>
#include <iostream>
#include "triangle.h"
#include "point.h"
#include <cmath>
#include <vector>

// Default constructor
Triangle::Triangle()
{
    this->p1 = Point();
    this->p2 = Point(3.0f, 0.0f);
    this->p3 = Point(3.0f, 4.0f);

    neighbourIndices[0] = -1;
    neighbourIndices[1] = -1;
    neighbourIndices[2] = -1;

    pointIndices[0] = -1;
    pointIndices[1] = -1;
    pointIndices[2] = -1;
}

Triangle::Triangle(Point p1, Point p2, Point p3)
{
    if (getArea() >= 0)
    {
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
    } else
    {
        this->p1 = p2;
        this->p2 = p1;
        this->p3 = p3;
    }

    neighbourIndices[0] = -1;
    neighbourIndices[1] = -1;
    neighbourIndices[2] = -1;

    pointIndices[0] = -1;
    pointIndices[1] = -1;
    pointIndices[2] = -1;
}

// Get length of a side
double Triangle::getLength(int side)
{
    if (side == 0) return p1.findDistance(p2);
    if (side == 1) return p2.findDistance(p3);
    if (side == 2) return p3.findDistance(p1);
    return 0.0;
}

// Get perimeter of the triangle
double Triangle::getPerimeter()
{
    return getLength(0) + getLength(1) + getLength(2);
}

// Get angle in the triangle
double Triangle::getAng(int angle)
{
    double dL1 = getLength(0);
    double dL2 = getLength(1);
    double dL3 = getLength(2);

    if (angle == 0)
    {
        return (180 / M_PI) * std::acos(((dL2 * dL2) + (dL3 * dL3) - (dL1 * dL1)) / (2 * dL2 * dL3));
    }
    if (angle == 1)
    {
        return (180 / M_PI) * std::acos(((dL1 * dL1) + (dL3 * dL3) - (dL2 * dL2)) / (2 * dL1 * dL3));
    }
    if (angle == 2)
    {
        return (180 / M_PI) * std::acos(((dL1 * dL1) + (dL2 * dL2) - (dL3 * dL3)) / (2 * dL1 * dL2));
    }
    return 0.0;
}

// Get area of the triangle
double Triangle::getArea()
{
    return 0.5 * (p1.getX() * (p2.getY() - p3.getY()) + p2.getX() * (p3.getY() - p1.getY()) + p3.getX() * (p1.getY() - p2.getY()));
}

// Check if a point is inside the triangle
bool Triangle::contains(Point pTargetPoint)
{
    double dTpx = pTargetPoint.getX();
    double dTpy = pTargetPoint.getY();

    // Determinants of segments
    double dL1 = (dTpx - p2.getX()) * (p1.getY() - p2.getY()) - (p1.getX() - p2.getX()) * (dTpy - p2.getY());
    double dL2 = (dTpx - p3.getX()) * (p2.getY() - p3.getY()) - (p2.getX() - p3.getX()) * (dTpy - p3.getY());
    double dL3 = (dTpx - p1.getX()) * (p3.getY() - p1.getY()) - (p3.getX() - p1.getX()) * (dTpy - p1.getY());

    // All the signs must be all negative, check if any are positive
    bool bHasPos = (dL1 > 0) || (dL2 > 0) || (dL3 > 0);

    return !(bHasPos);
}

// Find the path to the triangle containing the point
int Triangle::findPathToContainingTriangle(Point pTargetPoint)
{
    double dTpx = pTargetPoint.getX();
    double dTpy = pTargetPoint.getY();

    // Determinants of segments
    double dL1 = (dTpx - p2.getX()) * (p1.getY() - p2.getY()) - (p1.getX() - p2.getX()) * (dTpy - p2.getY());
    double dL2 = (dTpx - p3.getX()) * (p2.getY() - p3.getY()) - (p2.getX() - p3.getX()) * (dTpy - p3.getY());
    double dL3 = (dTpx - p1.getX()) * (p3.getY() - p1.getY()) - (p3.getX() - p1.getX()) * (dTpy - p1.getY());

    if (dL1 > 0) return getNeighbourIndex(0);
    if (dL2 > 0) return getNeighbourIndex(1);
    if (dL3 > 0) return getNeighbourIndex(2);
    return -1; // Return -1 if no positive determinant is found
}

// Get a specific point of the triangle
Point Triangle::getPoint(int nPoint)
{
    if (nPoint == 1) return p1;
    if (nPoint == 2) return p2;
    if (nPoint == 3) return p3;
    return Point();  // Default return if nPoint is invalid
}

// Print the points of the triangle
void Triangle::printPoints()
{
    std::ostringstream oss; // Create a string stream
    oss << "Point 1: (" << p1.getX() << ", " << p1.getY() << "), "
        << "Point 2: (" << p2.getX() << ", " << p2.getY() << "), "
        << "Point 3: (" << p3.getX() << ", " << p3.getY() << ")";

    std::cout << oss.str() << std::endl; // Output the concatenated string
}

// Get the circumcenter of the triangle
Point Triangle::getCircumcenter()
{
    double dX1 = p1.getX(), dY1 = p1.getY();
    double dX2 = p2.getX(), dY2 = p2.getY();
    double dX3 = p3.getX(), dY3 = p3.getY();

    // Midpoints of the sides
    Point midAB((dX1 + dX2) / 2, (dY1 + dY2) / 2);
    Point midBC((dX2 + dX3) / 2, (dY2 + dY3) / 2);

    // Find slopes of the sides
    double dSlopeAB = p1.findSlope(p2);
    double dSlopeBC = p2.findSlope(p3);

    // Determine perpendicular slopes
    double dPerpSlopeAB, dPerpSlopeBC;

    if (dSlopeAB == 0)
    {
        dPerpSlopeAB = INFINITY; // Perpendicular to horizontal line is vertical
    }
    else
    {
        dPerpSlopeAB = -1 / dSlopeAB;
    }

    if (dSlopeBC == 0)
    {
        dPerpSlopeBC = INFINITY; // Perpendicular to horizontal line is vertical
    }
    else
    {
        dPerpSlopeBC = -1 / dSlopeBC;
    }

    double dIntercept1, dIntercept2;
    bool bIsVerticalAB = (dPerpSlopeAB == INFINITY);
    bool bIsVerticalBC = (dPerpSlopeBC == INFINITY);

    // Form the equations of the perpendicular bisectors
    if (bIsVerticalAB)
    {
        dIntercept1 = midAB.getX();
    }
    else
    {
        dIntercept1 = midAB.getY() - dPerpSlopeAB * midAB.getX();
    }

    if (bIsVerticalBC)
    {
        dIntercept2 = midBC.getX();
    }
    else
    {
        dIntercept2 = midBC.getY() - dPerpSlopeBC * midBC.getX();
    }

    double dCircumcenterX, dCircumcenterY;

    // Solve the equations to find the intersection point (circumcenter)
    if (bIsVerticalAB)
    {
        dCircumcenterX = dIntercept1;
        dCircumcenterY = dPerpSlopeBC * dCircumcenterX + dIntercept2;
    }
    else if (bIsVerticalBC)
    {
        dCircumcenterX = dIntercept2;
        dCircumcenterY = dPerpSlopeAB * dCircumcenterX + dIntercept1;
    }
    else
    {
        dCircumcenterX = (dIntercept2 - dIntercept1) / (dPerpSlopeAB - dPerpSlopeBC);
        dCircumcenterY = dPerpSlopeAB * dCircumcenterX + dIntercept1;
    }

    return Point(dCircumcenterX, dCircumcenterY);
}

// Check if a point is inside the circumcircle
bool Triangle::isInCircumcircle(Point p)
{
    Point circumcenter = getCircumcenter();
    double dRadius = circumcenter.findDistance(getPoint(1)); // Calculate the radius
    double dPointDistance = circumcenter.findDistance(p); // Calculate the distance from the point to the circumcenter

    return dPointDistance < dRadius;
}

// Getter for neighbourIndices
int Triangle::getNeighbourIndex(int index)
{
    if (index >= 0 && index < 3) {
        return neighbourIndices[index];
    }
    return -1; // Return -1 for invalid index
}

// Setter for neighbourIndices
void Triangle::setNeighbourIndex(int index, int value)
{
    if (index >= 0 && index < 3)
    {
        neighbourIndices[index] = value;
    }
}

// Getter for pointIndices
int Triangle::getPointIndex(int index)
{
    if (index >= 0 && index < 3)
    {
        return pointIndices[index];
    }
    return -1; // Return -1 for invalid index
}

// Setter for pointIndices
void Triangle::setPointIndex(int index, int value)
{
    if (index >= 0 && index < 3)
    {
        pointIndices[index] = value;
    }
}



