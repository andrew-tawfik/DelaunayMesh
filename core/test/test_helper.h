#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include "mesh.h"
#include "iostream"

inline bool verifyNeighbourConsistency(const Mesh &mesh)
{
    auto triangles = mesh.getTriVector();

    int violations = 0;
    // for every triangle
    for (const Triangle &t : triangles)
    {
        for (int i = 0; i < 3; i++)
        {
            const int neighbourIndex = t.neighbourIndex(i);

            if (neighbourIndex == -1)
            {
                continue;
            }

            // Neighbor index out of bounds
            if (neighbourIndex < 0 || neighbourIndex >= triangles.size())
            {
                violations++;
                continue;
            }

            // Verify bidirectional: neighbor must point back to us
            const Triangle &neighbour = triangles[neighbourIndex];
            bool neighbourPointsBack = false;

            for (int j = 0; j < 3; j++)
            {
                if (neighbour.neighbourIndex(j) == t.index())
                {
                    neighbourPointsBack = true;
                    break;
                }
            }

            if (!neighbourPointsBack)
            {
                violations++;
                continue;
            }
        }
    }

    if (violations > 0) {
        std::cout << "FAILED (" << violations << " violations)" << std::endl;
        return false;
    }
    
    return true;
}

bool verifyDelaunayProperty(const Mesh &mesh, int *failTriangle = nullptr, int *failPoint = nullptr)
{
    auto triangles = mesh.getTriVector();
    auto points = mesh.getPtVector();
    int violations = 0;

    for (const Triangle &t : triangles)
    {
        int vertexIndices[3] = {
            t.pointIndex(0),
            t.pointIndex(1),
            t.pointIndex(2)};

        for (int i = 0; i < points.size(); i++)
        {
            if (i == vertexIndices[0] || i == vertexIndices[1] || i == vertexIndices[2])
            {
                continue;
            }

            if (t.isInCircumcircle(points[i]))
            {
                violations++;
                if (failTriangle)
                    *failTriangle = t.index();
                if (failPoint)
                    *failPoint = i;
            }
        }
    }

    if (violations > 0) {
        std::cout << "FAILED (" << violations << " violations)" << std::endl;
        return false;
    }
    
    // std::cout << "PASSED" << std::endl;
    return true;
}

#endif