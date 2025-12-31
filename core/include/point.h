#include "json.hpp"

#ifndef POINT_H
#define POINT_H

// Class representing a 2D point
class Point {

private:
    double m_x = 0.0;
    double m_y = 0.0;

public:
    // Constructors
    Point() = default;
    constexpr Point(double x, double y) : m_x(x), m_y(y) {}


    // Rule of Five: declare all special members
    ~Point() = default;
    Point(const Point&) = default;
    Point& operator=(const Point&) = default;
    Point(Point&&) noexcept = default;
    Point& operator=(Point&&) noexcept = default;

    // Getter for x coordinate
    [[nodiscard]] double x() const noexcept {return m_x; }
    [[nodiscard]] double y() const noexcept {return m_y; }

    // Setter for x coordinate
    void setX(double x) noexcept {m_x = x; }

    // Setter for y coordinate
    void setY(double y) noexcept {m_y = y;}

    // Function to find the distance between another point
    [[nodiscard]] double distanceTo(const Point& p2) const noexcept;

    // Function to find the slope to another point
    [[nodiscard]] double slopeTo(const Point& p2) const noexcept;

    bool operator==(const Point& other) const noexcept;


};

// Serialization functions
void to_json(nlohmann::json& j, const Point& p);

#endif // POINT_H
