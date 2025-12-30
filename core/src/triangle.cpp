#define _USE_MATH_DEFINES

#include <sstream>
#include <iostream>
#include "triangle.h"
#include "point.h"
#include <cmath>


// Default constructor: initializes the triangle with default points
Triangle::Triangle()
{
    this->m_pt0 = Point();
    this->m_pt1 = Point(3.0f, 0.0f);
    this->m_pt2 = Point(3.0f, 4.0f);

    m_aiNeighbourIndices[0] = -1;
    m_aiNeighbourIndices[1] = -1;
    m_aiNeighbourIndices[2] = -1;

    m_aiPointIndices[0] = -1;
    m_aiPointIndices[1] = -1;
    m_aiPointIndices[2] = -1;
}

// Constructor: initializes the triangle with given points
Triangle::Triangle(const Point& pt0, const Point& pt1, const Point& pt2)
{
    if (getArea() >= 0)
    {
        this->m_pt0 = pt0;
        this->m_pt1 = pt1;
        this->m_pt2 = pt2;
    }
    else
    {
        this->m_pt0 = pt1;
        this->m_pt1 = pt0;
        this->m_pt2 = pt2;
    }

    m_aiNeighbourIndices[0] = -1;
    m_aiNeighbourIndices[1] = -1;
    m_aiNeighbourIndices[2] = -1;

    m_aiPointIndices[0] = -1;
    m_aiPointIndices[1] = -1;
    m_aiPointIndices[2] = -1;
}


// Get the length of a specified side
double Triangle::getLength(int iSide) const
{
    if (iSide == 0) return m_pt0.distanceTo(m_pt1);
    if (iSide == 1) return m_pt1.distanceTo(m_pt2);
    if (iSide == 2) return m_pt2.distanceTo(m_pt0);
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
    return 0.5 * (m_pt0.x() * (m_pt1.y() - m_pt2.y()) + m_pt1.x() * (m_pt2.y() - m_pt0.y()) + m_pt2.x() * (m_pt0.y() - m_pt1.y()));
}

// Check if the triangle contains a given point
bool Triangle::contains(const Point& ptTargetPoint) const
{
    double dTpx = ptTargetPoint.x();
    double dTpy = ptTargetPoint.y();

    // Determinants of segments
    double dL1 = (dTpx - m_pt0.x()) * (m_pt1.y() - m_pt0.y()) - (m_pt1.x() - m_pt0.x()) * (dTpy - m_pt0.y());
    double dL2 = (dTpx - m_pt1.x()) * (m_pt2.y() - m_pt1.y()) - (m_pt2.x() - m_pt1.x()) * (dTpy - m_pt1.y());
    double dL3 = (dTpx - m_pt2.x()) * (m_pt0.y() - m_pt2.y()) - (m_pt0.x() - m_pt2.x()) * (dTpy - m_pt2.y());

    // All the signs must be the same (all positive or all negative)
    bool bAllNeg = (dL1 <= 0) && (dL2 <= 0) && (dL3 <= 0);

    return bAllNeg;  // Either all positive or all negative
}

// Find the path to the triangle containing a given point
int Triangle::findPathToContainingTriangle(const Point& ptTargetPoint) const
{
    double dTpx = ptTargetPoint.x();
    double dTpy = ptTargetPoint.y();

    // Determinants of segments
    double dL1 = (dTpx - m_pt0.x()) * (m_pt1.y() - m_pt0.y()) - (m_pt1.x() - m_pt0.x()) * (dTpy - m_pt0.y());
    double dL2 = (dTpx - m_pt1.x()) * (m_pt2.y() - m_pt1.y()) - (m_pt2.x() - m_pt1.x()) * (dTpy - m_pt1.y());
    double dL3 = (dTpx - m_pt2.x()) * (m_pt0.y() - m_pt2.y()) - (m_pt0.x() - m_pt2.x()) * (dTpy - m_pt2.y());

    if (!contains(ptTargetPoint))
    {
        if (dL1 > 0) return getNeighbourIndex(0);
        if (dL2 > 0) return getNeighbourIndex(1);
        if (dL3 > 0) return getNeighbourIndex(2);

    }

    return -2; // Return -2 if no positive determinant is found
}

// Set a specific point of the triangle
void Triangle::setPoint(int iPoint, const Point& newPoint)
{
    if (iPoint == 0) this->m_pt0 = newPoint;
    if (iPoint == 1) this->m_pt1 = newPoint;
    if (iPoint == 2) this->m_pt2 = newPoint;

}


// Get a specific point of the triangle
Point Triangle::getPoint(int iPoint) const
{
    if (iPoint == 0) return m_pt0;
    if (iPoint == 1) return m_pt1;
    if (iPoint == 2) return m_pt2;
    return Point();  // Default return if iPoint is invalid
}

// Print the points of the triangle
void Triangle::printPoints() const
{
    std::ostringstream oss; // Create a string stream
    oss << "Point 0: (" << m_pt0.x() << ", " << m_pt0.y() << "), "
        << "Point 1: (" << m_pt1.x() << ", " << m_pt1.y() << "), "
        << "Point 2: (" << m_pt2.x() << ", " << m_pt2.y() << ")";

    std::cout << oss.str() << std::endl; // Output the string
}

// Get the circumcenter of the triangle
Point Triangle::getCircumcenter() const {
    double dX1 = m_pt0.x(), dY1 = m_pt0.y();
    double dX2 = m_pt1.x(), dY2 = m_pt1.y();
    double dX3 = m_pt2.x(), dY3 = m_pt2.y();

    // Midpoints of the sides
    Point ptMidAB((dX1 + dX2) / 2, (dY1 + dY2) / 2);
    Point ptMidBC((dX2 + dX3) / 2, (dY2 + dY3) / 2);

    // Find slopes of the sides
    double dSlopeAB = m_pt0.slopeTo(m_pt1);
    double dSlopeBC = m_pt1.slopeTo(m_pt2);

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
        dIntercept1 = ptMidAB.x();
    }
    else
    {
        dIntercept1 = ptMidAB.y() - dPerpSlopeAB * ptMidAB.x();
    }

    if (bIsVerticalBC)
    {
        dIntercept2 = ptMidBC.x();
    }
    else
    {
        dIntercept2 = ptMidBC.y() - dPerpSlopeBC * ptMidBC.x();
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
    double dRadius = ptCircumcenter.distanceTo(getPoint(1)); // Calculate the radius
    double dPointDistance = ptCircumcenter.distanceTo(pt); // Calculate the distance from the point to the circumcenter

    // Epsilon tolerance: point must be clearly inside
    const double epsilon = 1e-6;
    return dPointDistance < (dRadius-epsilon);
}

// Getter for aiNeighbourIndices
int Triangle::getNeighbourIndex(int iIndex) const
{
    if (iIndex >= 0 && iIndex < 3)
    {
        return m_aiNeighbourIndices[iIndex];
    }
    return -1; // Return -1 for invalid index
}

// Setter for aiNeighbourIndices
void Triangle::setNeighbourIndex(int iIndex, int iValue) {
    if (iIndex >= 0 && iIndex < 3)
    {
        m_aiNeighbourIndices[iIndex] = iValue;
    }
}

// Getter for aiPointIndices
int Triangle::getPointIndex(int iIndex) const
{
    if (iIndex >= 0 && iIndex < 3)
    {
        return m_aiPointIndices[iIndex];
    }
    return -1; // Return -1 for invalid index
}

// Setter for aiPointIndices
void Triangle::setPointIndex(int iIndex, int iValue)
{
    if (iIndex >= 0 && iIndex < 3)
    {
        m_aiPointIndices[iIndex] = iValue;
    }
}

//Getter for iIndex
int Triangle::getIndex() const
{
    return m_iIndex;
}

//Setter for iIndex
void Triangle::setIndex(int iValue)
{
    this->m_iIndex = iValue;
}

// Determines whether a point is on the perimeter of the triangle, and on which edge is it located on
int Triangle::onEdge(const Point& pt) const
{

    // Check if pt is on the line segment 0
    double crossProduct = (pt.y() - m_pt0.y()) * (m_pt1.x() - m_pt0.x()) - (pt.x() - m_pt0.x()) * (m_pt1.y() - m_pt0.y());
    if (std::abs(crossProduct) < 1e-7) // Check for collinearity
    {
        double dotProduct = (pt.x() - m_pt0.x()) * (m_pt1.x() - m_pt0.x()) + (pt.y() - m_pt0.y()) * (m_pt1.y() - m_pt0.y());
        if (dotProduct >= 0)
        {
            double squaredLength = (m_pt1.x() - m_pt0.x()) * (m_pt1.x() - m_pt0.x()) + (m_pt1.y() - m_pt0.y()) * (m_pt1.y() - m_pt0.y());
            if (dotProduct <= squaredLength)
            {
                return 0;
            }
        }
    }

    // Check if pt is on the line segment 1
    crossProduct = (pt.y() - m_pt1.y()) * (m_pt2.x() - m_pt1.x()) - (pt.x() - m_pt1.x()) * (m_pt2.y() - m_pt1.y());
    if (std::abs(crossProduct) < 1e-7) // Check for collinearity
    {
        double dotProduct = (pt.x() - m_pt1.x()) * (m_pt2.x() - m_pt1.x()) + (pt.y() - m_pt1.y()) * (m_pt2.y() - m_pt1.y());
        if (dotProduct >= 0)
        {
            double squaredLength = (m_pt2.x() - m_pt1.x()) * (m_pt2.x() - m_pt1.x()) + (m_pt2.y() - m_pt1.y()) * (m_pt2.y() - m_pt1.y());
            if (dotProduct <= squaredLength)
            {
                return 1;
            }
        }
    }

    // Check if pt is on the line segment 2
    crossProduct = (pt.y() - m_pt2.y()) * (m_pt0.x() - m_pt2.x()) - (pt.x() - m_pt2.x()) * (m_pt0.y() - m_pt2.y());
    if (std::abs(crossProduct) < 1e-7) // Check for collinearity
    {
        double dotProduct = (pt.x() - m_pt2.x()) * (m_pt0.x() - m_pt2.x()) + (pt.y() - m_pt2.y()) * (m_pt0.y() - m_pt2.y());
        if (dotProduct >= 0)
        {
            double squaredLength = (m_pt0.x() - m_pt2.x()) * (m_pt0.x() - m_pt2.x()) + (m_pt0.y() - m_pt2.y()) * (m_pt0.y() - m_pt2.y());
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
        ptStart = m_pt0;
        ptEnd = m_pt1;
    }
    else if (iSide == 1)
    {
        ptStart = m_pt1;
        ptEnd = m_pt2;
    }
    else
    {
        ptStart = m_pt2;
        ptEnd = m_pt0;
    }

    // Calculation of the midpoint
    double dXMid = (ptStart.x() + ptEnd.x()) / 2.0;
    double dYMid = (ptStart.y() + ptEnd.y()) / 2.0;

    // Return the midpoint
    return Point(dXMid, dYMid);
}

void to_json(nlohmann::json &j, const Triangle &t)
{
    bool helper_triangle = false;
    for (int i = 0; i < 3; ++i) {
        if (t.getPointIndex(i) <= -10) {
            helper_triangle = true;
            break; // Exit early if the condition is not met
        }
    }
    j = nlohmann::json {
        {"helper", helper_triangle}, // helper flag: 0 : 1
        {"index", t.getIndex()}, // triangle index
        {"pt0", t.getPoint(0)}, // Point 0
        {"pt1", t.getPoint(1)}, // Point 1
        {"pt2", t.getPoint(2)} // Point 2
    };
}
