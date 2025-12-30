#include <iostream>
#include <chrono>
#include <random>
#include "mesh.h"
#include "test_helper.h"

void benchmarkIncrementalInsertion(int numPoints) {
    std::mt19937 rng(99999);
    std::uniform_real_distribution<double> dist(1.0, 150000.0);
    
    Mesh mesh;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numPoints; i++) {
        double x = dist(rng);
        double y = dist(rng);
        mesh.triangulatePoint(x, y);
        
        if ((i + 1) % 10000 == 0) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
            std::cout << "  " << (i + 1) << " points: " << elapsed.count() << " ms" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    mesh.removeHelperTriangles();
    
    std::cout << std::endl;
    std::cout << "=== Results ===" << std::endl;
    std::cout << "Total time:    " << duration.count() << " ms" << std::endl;
    std::cout << "Points/sec:    " << (numPoints * 1000.0 / duration.count()) << std::endl;
    std::cout << "Triangles:     " << mesh.triangles().size() << std::endl;
    std::cout << "Points:        " << mesh.points().size() << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    int numPoints = 100000;
    
    if (argc > 1) {
        numPoints = std::atoi(argv[1]);
    }
    
    std::cout << "Benchmarking Delaunay Triangulation" << std::endl;
    std::cout << "====================================" << std::endl;
    
    benchmarkIncrementalInsertion(numPoints);
    
    return 0;
}