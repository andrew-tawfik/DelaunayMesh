#define _USE_MATH_DEFINES

#include "triangle.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "constants.h"

Triangle::Triangle() 
    : m_points{Point(), Point(3.0, 0.0), Point(3.0, 4.0)}
{
}

Triangle::Triangle(const Point& p0, const Point& p1, const Point& p2)
    : m_points{p0, p1, p2}
    , m_pointIndices{-1, -1, -1}
    , m_neighbourIndices{-1, -1, -1}
{
    // Fix winding order if needed
    if (area() < 0) {
        std::swap(m_points[0], m_points[1]);
    }
}

const Point& Triangle::point(size_t i) const
{
    if (i >= 3) {
        throw std::out_of_range("Triangle point index must be 0, 1, or 2");
    }
    return m_points[i];
}

void Triangle::setPoint(size_t i, const Point& p)
{
    if (i >= 3) {
        throw std::out_of_range("Triangle point index must be 0, 1, or 2");
    }
    m_points[i] = p;
}

int Triangle::pointIndex(size_t i) const
{
    if (i >= 3) {
        throw std::out_of_range("Triangle point index must be 0, 1, or 2");
    }
    return m_pointIndices[i];
}

void Triangle::setPointIndex(size_t i, int idx)
{
    if (i >= 3) {
        throw std::out_of_range("Triangle point index must be 0, 1, or 2");
    }
    m_pointIndices[i] = idx;
}

int Triangle::neighbourIndex(size_t i) const
{
    if (i >= 3) {
        throw std::out_of_range("Triangle neighbour index must be 0, 1, or 2");
    }
    return m_neighbourIndices[i];
}

void Triangle::setNeighbourIndex(size_t i, int idx)
{
    if (i >= 3) {
        throw std::out_of_range("Triangle neighbour index must be 0, 1, or 2");
    }
    m_neighbourIndices[i] = idx;
}

double Triangle::edgeLength(size_t edge) const
{
    if (edge >= 3) {
        throw std::out_of_range("Triangle edge index must be 0, 1, or 2");
    }
    return m_points[edge].distanceTo(m_points[(edge + 1) % 3]);
}

double Triangle::perimeter() const
{
    double sum = 0.0;
    for (size_t i = 0; i < 3; ++i) {
        sum += edgeLength(i);
    }
    return sum;
}

double Triangle::angle(size_t vertex) const
{
    if (vertex >= 3) {
        throw std::out_of_range("Triangle vertex index must be 0, 1, or 2");
    }

    // Get the three edge lengths
    double a = edgeLength(0);
    double b = edgeLength(1);
    double c = edgeLength(2);

    // Law of cosines for each angle
    double cosAngle = 0.0;
    switch (vertex) {
        case 0: cosAngle = (b * b + c * c - a * a) / (2.0 * b * c); break;
        case 1: cosAngle = (a * a + c * c - b * b) / (2.0 * a * c); break;
        case 2: cosAngle = (a * a + b * b - c * c) / (2.0 * a * b); break;
    }

    return (180.0 / M_PI) * std::acos(cosAngle);
}

double Triangle::area() const
{
    // Signed area using cross product (shoelace formula)
    return 0.5 * (
        m_points[0].x() * (m_points[1].y() - m_points[2].y()) +
        m_points[1].x() * (m_points[2].y() - m_points[0].y()) +
        m_points[2].x() * (m_points[0].y() - m_points[1].y())
    );
}

Point Triangle::edgeMidpoint(size_t edge) const
{
    if (edge >= 3) {
        throw std::out_of_range("Triangle edge index must be 0, 1, or 2");
    }
    
    const Point& start = m_points[edge];
    const Point& end = m_points[(edge + 1) % 3];
    
    return Point(
        (start.x() + end.x()) / 2.0,
        (start.y() + end.y()) / 2.0
    );
}

Point Triangle::circumcenter() const
{
    double x1 = m_points[0].x(), y1 = m_points[0].y();
    double x2 = m_points[1].x(), y2 = m_points[1].y();
    double x3 = m_points[2].x(), y3 = m_points[2].y();

    // Midpoints of edges AB and BC
    Point midAB((x1 + x2) / 2.0, (y1 + y2) / 2.0);
    Point midBC((x2 + x3) / 2.0, (y2 + y3) / 2.0);

    // Slopes of edges
    double slopeAB = m_points[0].slopeTo(m_points[1]);
    double slopeBC = m_points[1].slopeTo(m_points[2]);

    // Perpendicular slopes
    bool verticalPerpAB = (slopeAB == 0);
    bool verticalPerpBC = (slopeBC == 0);
    
    double perpSlopeAB = verticalPerpAB ? INFINITY : -1.0 / slopeAB;
    double perpSlopeBC = verticalPerpBC ? INFINITY : -1.0 / slopeBC;

    // Y-intercepts (or x-value for vertical lines)
    double intercept1 = verticalPerpAB ? midAB.x() : midAB.y() - perpSlopeAB * midAB.x();
    double intercept2 = verticalPerpBC ? midBC.x() : midBC.y() - perpSlopeBC * midBC.x();

    // Find intersection of perpendicular bisectors
    double cx, cy;
    
    if (verticalPerpAB) {
        cx = intercept1;
        cy = perpSlopeBC * cx + intercept2;
    } else if (verticalPerpBC) {
        cx = intercept2;
        cy = perpSlopeAB * cx + intercept1;
    } else {
        cx = (intercept2 - intercept1) / (perpSlopeAB - perpSlopeBC);
        cy = perpSlopeAB * cx + intercept1;
    }

    return Point(cx, cy);
}

// Print the points of the triangle
void Triangle::printPoints() const
{
    std::cout << "Point 0: (" << m_points[0].x() << ", " << m_points[0].y() << "), "
              << "Point 1: (" << m_points[1].x() << ", " << m_points[1].y() << "), "
              << "Point 2: (" << m_points[2].x() << ", " << m_points[2].y() << ")"
              << std::endl;
}

bool Triangle::contains(const Point& pt) const
{
    double px = pt.x();
    double py = pt.y();

    // Calculate signed areas (cross products) for each edge
    double d1 = (px - m_points[0].x()) * (m_points[1].y() - m_points[0].y()) 
              - (m_points[1].x() - m_points[0].x()) * (py - m_points[0].y());
    
    double d2 = (px - m_points[1].x()) * (m_points[2].y() - m_points[1].y()) 
              - (m_points[2].x() - m_points[1].x()) * (py - m_points[1].y());
    
    double d3 = (px - m_points[2].x()) * (m_points[0].y() - m_points[2].y()) 
              - (m_points[0].x() - m_points[2].x()) * (py - m_points[2].y());

    // Point is inside if all signs are the same (all <= 0 for CCW winding)
    return (d1 <= 0) && (d2 <= 0) && (d3 <= 0);
}

bool Triangle::isInCircumcircle(const Point& pt) const
{
    Point cc = circumcenter();
    double radius = cc.distanceTo(m_points[0]);
    double distance = cc.distanceTo(pt);
    
    // Use epsilon to avoid floating-point edge cases
    constexpr double epsilon = 1e-9;
    return distance < (radius - epsilon);
}

bool Triangle::isPointOnEdge(const Point& pt, const Point& edgeStart, const Point& edgeEnd) const
{
    constexpr double epsilon = 1e-9;
    
    // Check collinearity using cross product
    double crossProduct = (pt.y() - edgeStart.y()) * (edgeEnd.x() - edgeStart.x()) 
                        - (pt.x() - edgeStart.x()) * (edgeEnd.y() - edgeStart.y());
    
    if (std::abs(crossProduct) >= epsilon) {
        return false;  // Not collinear
    }
    
    // Check if point is within the segment bounds using dot product
    double dotProduct = (pt.x() - edgeStart.x()) * (edgeEnd.x() - edgeStart.x()) 
                      + (pt.y() - edgeStart.y()) * (edgeEnd.y() - edgeStart.y());
    
    if (dotProduct < 0) {
        return false;  // Before start
    }
    
    double squaredLength = (edgeEnd.x() - edgeStart.x()) * (edgeEnd.x() - edgeStart.x()) 
                         + (edgeEnd.y() - edgeStart.y()) * (edgeEnd.y() - edgeStart.y());
    
    return dotProduct <= squaredLength;  // Within segment
}

int Triangle::onEdge(const Point& pt) const
{
    for (size_t i = 0; i < 3; ++i) {
        if (isPointOnEdge(pt, m_points[i], m_points[(i + 1) % 3])) {
            return static_cast<int>(i);
        }
    }
    return -1;  // Not on any edge
}

std::optional<int> Triangle::neighborToward(const Point& pt) const
{
    double px = pt.x();
    double py = pt.y();

    // Calculate signed areas for each edge
    double d1 = (px - m_points[0].x()) * (m_points[1].y() - m_points[0].y()) 
              - (m_points[1].x() - m_points[0].x()) * (py - m_points[0].y());
    
    double d2 = (px - m_points[1].x()) * (m_points[2].y() - m_points[1].y()) 
              - (m_points[2].x() - m_points[1].x()) * (py - m_points[1].y());
    
    double d3 = (px - m_points[2].x()) * (m_points[0].y() - m_points[2].y()) 
              - (m_points[0].x() - m_points[2].x()) * (py - m_points[2].y());

    // Return neighbour across the edge with positive determinant
    if (d1 > 0) return m_neighbourIndices[0];
    if (d2 > 0) return m_neighbourIndices[1];
    if (d3 > 0) return m_neighbourIndices[2];

    return std::nullopt;
}

void to_json(nlohmann::json& j, const Triangle& t)
{
    bool isHelper = false;
    for (size_t i = 0; i < 3; ++i) {
        if (delaunay::isSuperVertex(t.pointIndex(i))) {
            isHelper = true;
            break;
        }
    }
    
    j = nlohmann::json{
        {"helper", isHelper},
        {"index", t.index()},
        {"pt0", t.point(0)},
        {"pt1", t.point(1)},
        {"pt2", t.point(2)}
    };
}