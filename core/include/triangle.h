#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"
#include "constants.h"
#include <array>
#include <optional>
#include <cassert>
#include <cmath>

class Triangle {
private:
    std::array<Point, 3> m_points{};
    std::array<int, 3> m_pointIndices{-1, -1, -1};
    std::array<int, 3> m_neighbourIndices{-1, -1, -1};
    int m_index = -1;

    inline bool isPointOnEdge(const Point& pt, const Point& edgeStart, const Point& edgeEnd) const noexcept {
        constexpr double epsilon = 1e-7;
        
        double crossProduct = (pt.y() - edgeStart.y()) * (edgeEnd.x() - edgeStart.x()) 
                            - (pt.x() - edgeStart.x()) * (edgeEnd.y() - edgeStart.y());
        
        if (std::abs(crossProduct) >= epsilon) {
            return false;
        }
        
        double dotProduct = (pt.x() - edgeStart.x()) * (edgeEnd.x() - edgeStart.x()) 
                          + (pt.y() - edgeStart.y()) * (edgeEnd.y() - edgeStart.y());
        
        if (dotProduct < 0) {
            return false;
        }
        
        double squaredLength = (edgeEnd.x() - edgeStart.x()) * (edgeEnd.x() - edgeStart.x()) 
                             + (edgeEnd.y() - edgeStart.y()) * (edgeEnd.y() - edgeStart.y());
        
        return dotProduct <= squaredLength;
    }

public:
    Triangle();
    Triangle(const Point& pt0, const Point& pt1, const Point& pt2);

    ~Triangle() = default;
    Triangle(const Triangle&) = default;
    Triangle& operator=(const Triangle&) = default;
    Triangle(Triangle&&) noexcept = default;
    Triangle& operator=(Triangle&&) noexcept = default;

    // Getters - all inline, assert-only checking (debug builds)
    [[nodiscard]] const Point& point(size_t i) const noexcept { 
        assert(i < 3);
        return m_points[i]; 
    }
    
    [[nodiscard]] int pointIndex(size_t i) const noexcept { 
        assert(i < 3);
        return m_pointIndices[i]; 
    }
    
    [[nodiscard]] int neighbourIndex(size_t i) const noexcept { 
        assert(i < 3);
        return m_neighbourIndices[i]; 
    }
    
    [[nodiscard]] int index() const noexcept { return m_index; }

    // Setters - all inline, assert-only checking (debug builds)
    void setPoint(size_t i, const Point& p) noexcept {
        assert(i < 3);
        m_points[i] = p;
    }
    
    void setPointIndex(size_t i, int idx) noexcept {
        assert(i < 3);
        m_pointIndices[i] = idx;
    }
    
    void setNeighbourIndex(size_t i, int idx) noexcept {
        assert(i < 3);
        m_neighbourIndices[i] = idx;
    }
    
    void setIndex(int idx) noexcept { m_index = idx; }

    // Geometry
    [[nodiscard]] double edgeLength(size_t edge) const noexcept;
    [[nodiscard]] double perimeter() const noexcept;
    [[nodiscard]] double angle(size_t vertex) const noexcept;
    [[nodiscard]] double area() const noexcept;
    [[nodiscard]] Point circumcenter() const noexcept;
    [[nodiscard]] Point edgeMidpoint(size_t edge) const noexcept;

    // Spatial queries
    [[nodiscard]] bool contains(const Point& pt) const noexcept;
    [[nodiscard]] bool isInCircumcircle(const Point& pt) const noexcept;
    [[nodiscard]] int onEdge(const Point& pt) const noexcept;
    [[nodiscard]] std::optional<int> neighborToward(const Point& pt) const noexcept;
    [[nodiscard]] int findPathToward(const Point& pt) const noexcept;


    void printPoints() const;
};

void to_json(nlohmann::json& j, const Triangle& t);

#endif // TRIANGLE_H