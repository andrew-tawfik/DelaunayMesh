#define _USE_MATH_DEFINES

#include "triangle.h"
#include <cmath>
#include <iostream>


Triangle::Triangle() 
    : m_points{Point(), Point(3.0, 0.0), Point(3.0, 4.0)}
{
}

Triangle::Triangle(const Point& p0, const Point& p1, const Point& p2)
    : m_points{p0, p1, p2}
{
    if (area() < 0) {
        std::swap(m_points[0], m_points[1]);
    }
}

double Triangle::edgeLength(size_t edge) const noexcept
{
    assert(edge < 3);
    return m_points[edge].distanceTo(m_points[(edge + 1) % 3]);
}

double Triangle::perimeter() const noexcept
{
    return edgeLength(0) + edgeLength(1) + edgeLength(2);
}

double Triangle::angle(size_t vertex) const noexcept
{
    assert(vertex < 3);
    
    double a = edgeLength(0);
    double b = edgeLength(1);
    double c = edgeLength(2);

    double cosAngle = 0.0;
    switch (vertex) {
        case 0: cosAngle = (b * b + c * c - a * a) / (2.0 * b * c); break;
        case 1: cosAngle = (a * a + c * c - b * b) / (2.0 * a * c); break;
        case 2: cosAngle = (a * a + b * b - c * c) / (2.0 * a * b); break;
    }

    return (180.0 / M_PI) * std::acos(cosAngle);
}

double Triangle::area() const noexcept
{
    return 0.5 * (
        m_points[0].x() * (m_points[1].y() - m_points[2].y()) +
        m_points[1].x() * (m_points[2].y() - m_points[0].y()) +
        m_points[2].x() * (m_points[0].y() - m_points[1].y())
    );
}

Point Triangle::edgeMidpoint(size_t edge) const noexcept
{
    assert(edge < 3);
    
    const Point& start = m_points[edge];
    const Point& end = m_points[(edge + 1) % 3];
    
    return Point(
        (start.x() + end.x()) / 2.0,
        (start.y() + end.y()) / 2.0
    );
}

Point Triangle::circumcenter() const noexcept
{
    double x1 = m_points[0].x(), y1 = m_points[0].y();
    double x2 = m_points[1].x(), y2 = m_points[1].y();
    double x3 = m_points[2].x(), y3 = m_points[2].y();

    Point midAB((x1 + x2) / 2.0, (y1 + y2) / 2.0);
    Point midBC((x2 + x3) / 2.0, (y2 + y3) / 2.0);

    double slopeAB = m_points[0].slopeTo(m_points[1]);
    double slopeBC = m_points[1].slopeTo(m_points[2]);

    bool verticalPerpAB = (slopeAB == 0);
    bool verticalPerpBC = (slopeBC == 0);
    
    double perpSlopeAB = verticalPerpAB ? INFINITY : -1.0 / slopeAB;
    double perpSlopeBC = verticalPerpBC ? INFINITY : -1.0 / slopeBC;

    double intercept1 = verticalPerpAB ? midAB.x() : midAB.y() - perpSlopeAB * midAB.x();
    double intercept2 = verticalPerpBC ? midBC.x() : midBC.y() - perpSlopeBC * midBC.x();

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
    double px = pt.x();
    double py = pt.y();

    double d1 = (px - m_points[0].x()) * (m_points[1].y() - m_points[0].y()) 
              - (m_points[1].x() - m_points[0].x()) * (py - m_points[0].y());
    
    double d2 = (px - m_points[1].x()) * (m_points[2].y() - m_points[1].y()) 
              - (m_points[2].x() - m_points[1].x()) * (py - m_points[1].y());
    
    double d3 = (px - m_points[2].x()) * (m_points[0].y() - m_points[2].y()) 
              - (m_points[0].x() - m_points[2].x()) * (py - m_points[2].y());

    return (d1 <= 0) && (d2 <= 0) && (d3 <= 0);
}

bool Triangle::isInCircumcircle(const Point& pt) const noexcept
{
    Point cc = circumcenter();
    double radius = cc.distanceTo(m_points[0]);
    double distance = cc.distanceTo(pt);
    
    constexpr double epsilon = 1e-6;
    return distance < (radius - epsilon);
}

int Triangle::onEdge(const Point& pt) const noexcept
{
    for (size_t i = 0; i < 3; ++i) {
        if (isPointOnEdge(pt, m_points[i], m_points[(i + 1) % 3])) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::optional<int> Triangle::neighborToward(const Point& pt) const noexcept
{
    double px = pt.x();
    double py = pt.y();

    double d1 = (px - m_points[0].x()) * (m_points[1].y() - m_points[0].y()) 
              - (m_points[1].x() - m_points[0].x()) * (py - m_points[0].y());
    
    double d2 = (px - m_points[1].x()) * (m_points[2].y() - m_points[1].y()) 
              - (m_points[2].x() - m_points[1].x()) * (py - m_points[1].y());
    
    double d3 = (px - m_points[2].x()) * (m_points[0].y() - m_points[2].y()) 
              - (m_points[0].x() - m_points[2].x()) * (py - m_points[2].y());

    if (d1 > 0) return m_neighbourIndices[0];
    if (d2 > 0) return m_neighbourIndices[1];
    if (d3 > 0) return m_neighbourIndices[2];

    return std::nullopt;
}

int Triangle::findPathToward(const Point& pt) const noexcept
{
    double px = pt.x();
    double py = pt.y();

    double d1 = (px - m_points[0].x()) * (m_points[1].y() - m_points[0].y()) 
              - (m_points[1].x() - m_points[0].x()) * (py - m_points[0].y());
    
    double d2 = (px - m_points[1].x()) * (m_points[2].y() - m_points[1].y()) 
              - (m_points[2].x() - m_points[1].x()) * (py - m_points[1].y());
    
    double d3 = (px - m_points[2].x()) * (m_points[0].y() - m_points[2].y()) 
              - (m_points[0].x() - m_points[2].x()) * (py - m_points[2].y());

    // If any determinant is positive, return that neighbor
    if (d1 > 0) return m_neighbourIndices[0];
    if (d2 > 0) return m_neighbourIndices[1];
    if (d3 > 0) return m_neighbourIndices[2];

    // All <= 0 means point is inside (or on edge)
    return delaunay::NO_NEIGHBOR;
}

void Triangle::printPoints() const
{
    std::cout << "Point 0: (" << m_points[0].x() << ", " << m_points[0].y() << "), "
              << "Point 1: (" << m_points[1].x() << ", " << m_points[1].y() << "), "
              << "Point 2: (" << m_points[2].x() << ", " << m_points[2].y() << ")"
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