#include "point.h"
#include <cmath>
#include <iostream>

// Function to find the distance between another point
double Point::distanceTo(const Point& p2) const
{
    double dx1 = m_x;
    double dy1 = m_y;

    double dx2 = p2.x();
    double dy2 = p2.y();

    double dy_t = dy2 - dy1;
    double dx_t = dx2 - dx1;

    double dDistance = std::sqrt(std::pow(dy_t, 2.0) + std::pow(dx_t, 2.0));

    return dDistance;
}

// Function to find the slope to another point
double Point::slopeTo(const Point& other) const
{
    return (other.m_y - m_y) / (other.m_x - m_x);
}

bool Point::operator==(const Point& other) const noexcept{
    constexpr double epsilon = 1e-6;
    return std::abs(m_x - other.m_x) < epsilon && 
           std::abs(m_y - other.m_y) < epsilon;
}

void to_json(nlohmann::json &j, const Point &p)
{
    j = nlohmann::json{{"x", p.x()},{"y", p.y()}};
}
