// constants.h (new file)
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace delaunay {

// These are negative to distinguish from real point indices
constexpr int SUPER_VERTEX_0 = -10;
constexpr int SUPER_VERTEX_1 = -11;
constexpr int SUPER_VERTEX_2 = -12;

constexpr int SUPER_VERTEX_THRESHOLD = -10;

constexpr int NO_NEIGHBOR = -1;

constexpr int POINT_INSIDE = -2;

// Helper to check if a point index is a super-triangle vertex
constexpr bool isSuperVertex(int pointIndex) {
    return pointIndex <= SUPER_VERTEX_THRESHOLD;
}

} // namespace delaunay

#endif // CONSTANTS_H