#define _USE_MATH_DEFINES

#include <sstream>
#include <iostream>
#include "triangle.h"
#include "point.h"
#include <cmath>


// Default constructor: initializes the triangle with default points
Triangle::Triangle()
{
    this->pt0 = Point();
    this->pt1 = Point(3.0f, 0.0f);
    this->pt2 = Point(3.0f, 4.0f);

    aiNeighbourIndices[0] = -1;
    aiNeighbourIndices[1] = -1;
    aiNeighbourIndices[2] = -1;

    aiPointIndices[0] = -1;
    aiPointIndices[1] = -1;
    aiPointIndices[2] = -1;
}

// Constructor: initializes the triangle with given points
Triangle::Triangle(const Point& pt0, const Point& pt1, const Point& pt2)
{
    if (getArea() >= 0)
    {
        this->pt0 = pt0;
        this->pt1 = pt1;
        this->pt2 = pt2;
    }
    else
    {
        this->pt0 = pt1;
        this->pt1 = pt0;
        this->pt2 = pt2;
    }

    aiNeighbourIndices[0] = -1;
    aiNeighbourIndices[1] = -1;
    aiNeighbourIndices[2] = -1;

    aiPointIndices[0] = -1;
    aiPointIndices[1] = -1;
    aiPointIndices[2] = -1;
}


// Get the length of a specified side
double Triangle::getLength(int iSide) const
{
    if (iSide == 0) return pt0.findDistance(pt1);
    if (iSide == 1) return pt1.findDistance(pt2);
    if (iSide == 2) return pt2.findDistance(pt0);
    return 0.0;
}

// Get the perimeter of the triangle
double Triangle::getPerimeter() const
{
    return getLength(0) + getLength(1) + getLength(2);
}

// Get a specified angle in the triangle
double Triangle::getAng(int iAngle) const
{
    double dL1 = getLength(0);
    double dL2 = getLength(1);
    double dL3 = getLength(2);

    if (iAngle == 0)
    {
        return (180 / M_PI) * std::acos(((dL2 * dL2) + (dL3 * dL3) - (dL1 * dL1)) / (2 * dL2 * dL3));
    }
    if (iAngle == 1)
    {
        return (180 / M_PI) * std::acos(((dL1 * dL1) + (dL3 * dL3) - (dL2 * dL2)) / (2 * dL1 * dL3));
    }
    if (iAngle == 2)
    {
        return (180 / M_PI) * std::acos(((dL1 * dL1) + (dL2 * dL2) - (dL3 * dL3)) / (2 * dL1 * dL2));
    }
    return 0.0;
}

// Get the area of the triangle
double Triangle::getArea() const
{
    return 0.5 * (pt0.getX() * (pt1.getY() - pt2.getY()) + pt1.getX() * (pt2.getY() - pt0.getY()) + pt2.getX() * (pt0.getY() - pt1.getY()));
}

// Check if the triangle contains a given point
bool Triangle::contains(const Point& ptTargetPoint) const
{
    double dTpx = ptTargetPoint.getX();
    double dTpy = ptTargetPoint.getY();

    // Determinants of segments
    double dL1 = (dTpx - pt0.getX()) * (pt1.getY() - pt0.getY()) - (pt1.getX() - pt0.getX()) * (dTpy - pt0.getY());
    double dL2 = (dTpx - pt1.getX()) * (pt2.getY() - pt1.getY()) - (pt2.getX() - pt1.getX()) * (dTpy - pt1.getY());
    double dL3 = (dTpx - pt2.getX()) * (pt0.getY() - pt2.getY()) - (pt0.getX() - pt2.getX()) * (dTpy - pt2.getY());

    // All the signs must be the same (all positive or all negative)
    bool bHasPos = (dL1 > 0) || (dL2 > 0) || (dL3 > 0);

    return !(bHasPos);  // Either all positive or all negative
}

// Find the path to the triangle containing a given point
int Triangle::findPathToContainingTriangle(const Point& ptTargetPoint) const
{
    double dTpx = ptTargetPoint.getX();
    double dTpy = ptTargetPoint.getY();

    // Determinants of segments
    double dL1 = (dTpx - pt0.getX()) * (pt1.getY() - pt0.getY()) - (pt1.getX() - pt0.getX()) * (dTpy - pt0.getY());
    double dL2 = (dTpx - pt1.getX()) * (pt2.getY() - pt1.getY()) - (pt2.getX() - pt1.getX()) * (dTpy - pt1.getY());
    double dL3 = (dTpx - pt2.getX()) * (pt0.getY() - pt2.getY()) - (pt0.getX() - pt2.getX()) * (dTpy - pt2.getY());

    if (!contains(ptTargetPoint))
    {
        if (dL1 > 0) return getNeighbourIndex(0);
        if (dL2 > 0) return getNeighbourIndex(1);
        if (dL3 > 0) return getNeighbourIndex(2);

    }

    return -1; // Return -1 if no positive determinant is found
}

// Set a specific point of the triangle
void Triangle::setPoint(int iPoint, const Point& newPoint)
{
    if (iPoint == 0) this->pt0 = newPoint;
    if (iPoint == 1) this->pt1 = newPoint;
    if (iPoint == 2) this->pt2 = newPoint;

}


// Get a specific point of the triangle
Point Triangle::getPoint(int iPoint) const
{
    if (iPoint == 0) return pt0;
    if (iPoint == 1) return pt1;
    if (iPoint == 2) return pt2;
    return Point();  // Default return if iPoint is invalid
}

// Print the points of the triangle
void Triangle::printPoints() const
{
    std::ostringstream oss; // Create a string stream
    oss << "Point 0: (" << pt0.getX() << ", " << pt0.getY() << "), "
        << "Point 1: (" << pt1.getX() << ", " << pt1.getY() << "), "
        << "Point 2: (" << pt2.getX() << ", " << pt2.getY() << ")";

    std::cout << oss.str() << std::endl; // Output the string
}

// Get the circumcenter of the triangle
Point Triangle::getCircumcenter() const {
    double dX1 = pt0.getX(), dY1 = pt0.getY();
    double dX2 = pt1.getX(), dY2 = pt1.getY();
    double dX3 = pt2.getX(), dY3 = pt2.getY();

    // Midpoints of the sides
    Point ptMidAB((dX1 + dX2) / 2, (dY1 + dY2) / 2);
    Point ptMidBC((dX2 + dX3) / 2, (dY2 + dY3) / 2);

    // Find slopes of the sides
    double dSlopeAB = pt0.findSlope(pt1);
    double dSlopeBC = pt1.findSlope(pt2);

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
        dIntercept1 = ptMidAB.getX();
    }
    else
    {
        dIntercept1 = ptMidAB.getY() - dPerpSlopeAB * ptMidAB.getX();
    }

    if (bIsVerticalBC)
    {
        dIntercept2 = ptMidBC.getX();
    }
    else
    {
        dIntercept2 = ptMidBC.getY() - dPerpSlopeBC * ptMidBC.getX();
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
bool Triangle::isInCircumcircle(const Point& pt) const
{
    Point ptCircumcenter = getCircumcenter();
    double dRadius = ptCircumcenter.findDistance(getPoint(1)); // Calculate the radius
    double dPointDistance = ptCircumcenter.findDistance(pt); // Calculate the distance from the point to the circumcenter

    return dPointDistance < dRadius;
}

// Getter for aiNeighbourIndices
int Triangle::getNeighbourIndex(int iIndex) const
{
    if (iIndex >= 0 && iIndex < 3)
    {
        return aiNeighbourIndices[iIndex];
    }
    return -1; // Return -1 for invalid index
}

// Setter for aiNeighbourIndices
void Triangle::setNeighbourIndex(int iIndex, int iValue) {
    if (iIndex >= 0 && iIndex < 3)
    {
        aiNeighbourIndices[iIndex] = iValue;
    }
}

// Getter for aiPointIndices
int Triangle::getPointIndex(int iIndex) const
{
    if (iIndex >= 0 && iIndex < 3)
    {
        return aiPointIndices[iIndex];
    }
    return -1; // Return -1 for invalid index
}

// Setter for aiPointIndices
void Triangle::setPointIndex(int iIndex, int iValue)
{
    if (iIndex >= 0 && iIndex < 3)
    {
        aiPointIndices[iIndex] = iValue;
    }
}

//Getter for iIndex
int Triangle::getIndex() const
{
    return iIndex;
}

//Setter for iIndex
void Triangle::setIndex(int iValue)
{
    this->iIndex = iValue;
}

// Determines whether a point is on the perimeter of the triangle, and on which edge is it located on
int Triangle::onEdge(const Point& pt) const
{

    // Check if pt is on the line segment 0
    double crossProduct = (pt.getY() - pt0.getY()) * (pt1.getX() - pt0.getX()) - (pt.getX() - pt0.getX()) * (pt1.getY() - pt0.getY());
    if (std::abs(crossProduct) < 1e-7) // Check for collinearity
    {
        double dotProduct = (pt.getX() - pt0.getX()) * (pt1.getX() - pt0.getX()) + (pt.getY() - pt0.getY()) * (pt1.getY() - pt0.getY());
        if (dotProduct >= 0)
        {
            double squaredLength = (pt1.getX() - pt0.getX()) * (pt1.getX() - pt0.getX()) + (pt1.getY() - pt0.getY()) * (pt1.getY() - pt0.getY());
            if (dotProduct <= squaredLength)
            {
                return 0;
            }
        }
    }

    // Check if pt is on the line segment 1
    crossProduct = (pt.getY() - pt1.getY()) * (pt2.getX() - pt1.getX()) - (pt.getX() - pt1.getX()) * (pt2.getY() - pt1.getY());
    if (std::abs(crossProduct) < 1e-7) // Check for collinearity
    {
        double dotProduct = (pt.getX() - pt1.getX()) * (pt2.getX() - pt1.getX()) + (pt.getY() - pt1.getY()) * (pt2.getY() - pt1.getY());
        if (dotProduct >= 0)
        {
            double squaredLength = (pt2.getX() - pt1.getX()) * (pt2.getX() - pt1.getX()) + (pt2.getY() - pt1.getY()) * (pt2.getY() - pt1.getY());
            if (dotProduct <= squaredLength)
            {
                return 1;
            }
        }
    }

    // Check if pt is on the line segment 2
    crossProduct = (pt.getY() - pt2.getY()) * (pt0.getX() - pt2.getX()) - (pt.getX() - pt2.getX()) * (pt0.getY() - pt2.getY());
    if (std::abs(crossProduct) < 1e-7) // Check for collinearity
    {
        double dotProduct = (pt.getX() - pt2.getX()) * (pt0.getX() - pt2.getX()) + (pt.getY() - pt2.getY()) * (pt0.getY() - pt2.getY());
        if (dotProduct >= 0)
        {
            double squaredLength = (pt0.getX() - pt2.getX()) * (pt0.getX() - pt2.getX()) + (pt0.getY() - pt2.getY()) * (pt0.getY() - pt2.getY());
            if (dotProduct <= squaredLength)
            {
                return 2;
            }
        }
    }

    return -1;
}

// Gets the point of the center of the edge
Point Triangle::getEdgeMidpoint(int iSide) const
{
    Point ptStart, ptEnd;

    // Determine which points make up the edge
    if (iSide == 0)
    {
        ptStart = pt0;
        ptEnd = pt1;
    }
    else if (iSide == 1)
    {
        ptStart = pt1;
        ptEnd = pt2;
    }
    else
    {
        ptStart = pt2;
        ptEnd = pt0;
    }

    // Calculation of the midpoint
    double dXMid = (ptStart.getX() + ptEnd.getX()) / 2.0;
    double dYMid = (ptStart.getY() + ptEnd.getY()) / 2.0;

    // Return the midpoint
    return Point(dXMid, dYMid);
}
