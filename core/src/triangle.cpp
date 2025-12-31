#define _USE_MATH_DEFINES

#include "triangle.h"
#include <cmath>
#include <iostream>

Triangle::Triangle() 
    : m_pt0()
    , m_pt1(3.0, 0.0)
    , m_pt2(3.0, 4.0)
{
}

Triangle::Triangle(const Point& p0, const Point& p1, const Point& p2)
    : m_pt0(p0)
    , m_pt1(p1)
    , m_pt2(p2)
{
    if (area() < 0) {
        std::swap(m_pt0, m_pt1);
    }
}

double Triangle::edgeLength(size_t edge) const noexcept
{
    if (edge == 0) return m_pt0.distanceTo(m_pt1);
    if (edge == 1) return m_pt1.distanceTo(m_pt2);
    return m_pt2.distanceTo(m_pt0);
}

double Triangle::perimeter() const noexcept
{
    return m_pt0.distanceTo(m_pt1) + m_pt1.distanceTo(m_pt2) + m_pt2.distanceTo(m_pt0);
}

double Triangle::angle(size_t vertex) const noexcept
{
    const double a = m_pt0.distanceTo(m_pt1);
    const double b = m_pt1.distanceTo(m_pt2);
    const double c = m_pt2.distanceTo(m_pt0);

    double cosAngle = 0.0;
    if (vertex == 0) {
        cosAngle = (b * b + c * c - a * a) / (2.0 * b * c);
    } else if (vertex == 1) {
        cosAngle = (a * a + c * c - b * b) / (2.0 * a * c);
    } else {
        cosAngle = (a * a + b * b - c * c) / (2.0 * a * b);
    }

    return (180.0 / M_PI) * std::acos(cosAngle);
}

double Triangle::area() const noexcept
{
    return 0.5 * (
        m_pt0.x() * (m_pt1.y() - m_pt2.y()) +
        m_pt1.x() * (m_pt2.y() - m_pt0.y()) +
        m_pt2.x() * (m_pt0.y() - m_pt1.y())
    );
}

Point Triangle::edgeMidpoint(size_t edge) const noexcept
{
    if (edge == 0) {
        return Point((m_pt0.x() + m_pt1.x()) / 2.0, (m_pt0.y() + m_pt1.y()) / 2.0);
    } else if (edge == 1) {
        return Point((m_pt1.x() + m_pt2.x()) / 2.0, (m_pt1.y() + m_pt2.y()) / 2.0);
    }
    return Point((m_pt2.x() + m_pt0.x()) / 2.0, (m_pt2.y() + m_pt0.y()) / 2.0);
}

Point Triangle::circumcenter() const noexcept
{
    const double x1 = m_pt0.x(), y1 = m_pt0.y();
    const double x2 = m_pt1.x(), y2 = m_pt1.y();
    const double x3 = m_pt2.x(), y3 = m_pt2.y();

    const Point midAB((x1 + x2) / 2.0, (y1 + y2) / 2.0);
    const Point midBC((x2 + x3) / 2.0, (y2 + y3) / 2.0);

    const double slopeAB = m_pt0.slopeTo(m_pt1);
    const double slopeBC = m_pt1.slopeTo(m_pt2);

    const bool verticalPerpAB = (slopeAB == 0);
    const bool verticalPerpBC = (slopeBC == 0);
    
    const double perpSlopeAB = verticalPerpAB ? INFINITY : -1.0 / slopeAB;
    const double perpSlopeBC = verticalPerpBC ? INFINITY : -1.0 / slopeBC;

    const double intercept1 = verticalPerpAB ? midAB.x() : midAB.y() - perpSlopeAB * midAB.x();
    const double intercept2 = verticalPerpBC ? midBC.x() : midBC.y() - perpSlopeBC * midBC.x();

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

bool Triangle::contains(const Point& pt) const noexcept
{
    const double px = pt.x();
    const double py = pt.y();

    const double d1 = (px - m_pt0.x()) * (m_pt1.y() - m_pt0.y()) 
              - (m_pt1.x() - m_pt0.x()) * (py - m_pt0.y());
    
    const double d2 = (px - m_pt1.x()) * (m_pt2.y() - m_pt1.y()) 
              - (m_pt2.x() - m_pt1.x()) * (py - m_pt1.y());
    
    const double d3 = (px - m_pt2.x()) * (m_pt0.y() - m_pt2.y()) 
              - (m_pt0.x() - m_pt2.x()) * (py - m_pt2.y());

    return (d1 <= 0) & (d2 <= 0) & (d3 <= 0);
}

bool Triangle::isInCircumcircle(const Point& pt) const noexcept
{
    // Faster: Use determinant-based test (no division, no sqrt)
    const double ax = m_pt0.x() - pt.x();
    const double ay = m_pt0.y() - pt.y();
    const double bx = m_pt1.x() - pt.x();
    const double by = m_pt1.y() - pt.y();
    const double cx = m_pt2.x() - pt.x();
    const double cy = m_pt2.y() - pt.y();
    
    const double det = (ax * ax + ay * ay) * (bx * cy - cx * by)
                     - (bx * bx + by * by) * (ax * cy - cx * ay)
                     + (cx * cx + cy * cy) * (ax * by - bx * ay);
    
    return det > 1e-10;  // Positive if inside (for CCW triangles)
}

int Triangle::onEdge(const Point& pt) const noexcept
{
    if (isPointOnEdge(pt, m_pt0, m_pt1)) return 0;
    if (isPointOnEdge(pt, m_pt1, m_pt2)) return 1;
    if (isPointOnEdge(pt, m_pt2, m_pt0)) return 2;
    return -1;
}

int Triangle::findPathToward(const Point& pt) const noexcept
{
    const double px = pt.x();
    const double py = pt.y();

    const double d1 = (px - m_pt0.x()) * (m_pt1.y() - m_pt0.y()) 
              - (m_pt1.x() - m_pt0.x()) * (py - m_pt0.y());
    
    const double d2 = (px - m_pt1.x()) * (m_pt2.y() - m_pt1.y()) 
              - (m_pt2.x() - m_pt1.x()) * (py - m_pt1.y());
    
    const double d3 = (px - m_pt2.x()) * (m_pt0.y() - m_pt2.y()) 
              - (m_pt0.x() - m_pt2.x()) * (py - m_pt2.y());

    if (d1 > 0) return m_neighbourIndices[0];
    if (d2 > 0) return m_neighbourIndices[1];
    if (d3 > 0) return m_neighbourIndices[2];

    return delaunay::POINT_INSIDE;
}

void Triangle::printPoints() const
{
    std::cout << "Point 0: (" << m_pt0.x() << ", " << m_pt0.y() << "), "
              << "Point 1: (" << m_pt1.x() << ", " << m_pt1.y() << "), "
              << "Point 2: (" << m_pt2.x() << ", " << m_pt2.y() << ")"
              << std::endl;
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