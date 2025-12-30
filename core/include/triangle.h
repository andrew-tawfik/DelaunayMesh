#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"
#include <vector>
#include <array>

// Class representing a triangle defined by three points
class Triangle {
private:
    std::array<Point, 3> m_points{};
    std::array<int, 3> m_pointIndices{-1, -1, -1};
    std::array<int, 3> m_neighbourIndices{-1, -1, -1};
    int m_index = -1;

    [[nodiscard]] bool isPointOnEdge(const Point& pt, const Point& edgeStart, const Point& edgeEnd) const;

public:
    // Constructors
    Triangle();
    Triangle(const Point& pt0, const Point& pt1, const Point& pt2);

    // Rule of Five
    ~Triangle() = default;
    Triangle(const Triangle&) = default;
    Triangle& operator=(const Triangle&) = default;
    Triangle(Triangle&&) noexcept = default;
    Triangle& operator=(Triangle&&) noexcept = default;

    // Point access
    [[nodiscard]] const Point& point(size_t i) const;
    void setPoint(size_t i, const Point& p);

    // Point index access
    [[nodiscard]] int pointIndex(size_t i) const;
    void setPointIndex(size_t i, int idx);

    // Neighbour index access
    [[nodiscard]] int neighbourIndex(size_t i) const;
    void setNeighbourIndex(size_t i, int idx);

    // Triangle index
    [[nodiscard]] int index() const noexcept { return m_index; }
    void setIndex(int idx) noexcept { m_index = idx; }

    // Geometry calculations
    [[nodiscard]] double edgeLength(size_t edge) const;
    [[nodiscard]] double perimeter() const noexcept;
    [[nodiscard]] double angle(size_t vertex) const;
    [[nodiscard]] double area() const noexcept;
    [[nodiscard]] Point circumcenter() const noexcept;
    [[nodiscard]] Point edgeMidpoint(size_t iSide) const;

    // Spatial Queries
    [[nodiscard]] bool contains(const Point& ptTargetPoint) const noexcept;
    [[nodiscard]] bool isInCircumcircle(const Point& pt) const noexcept;
    [[nodiscard]] int onEdge(const Point& pt) const noexcept;
    [[nodiscard]] std::optional<int> neighborToward(const Point& pt) const noexcept;


    // Debug
    void printPoints() const;
};

// Serialization function
void to_json(nlohmann::json& j, const Triangle& t);

#endif // TRIANGLE_H
