# Delaunay Triangulation Project

## Table of Contents

1. [Overview](#overview)
2. [Algorithm](#algorithm)
3. [Class Structure](#class-structure)
4. [Future Modifications](#future-modifications)


## Overview

This project implements a Delaunay triangulation algorithm using C++. The program efficiently generates a mesh of triangles from a given set of points, ensuring that no point lies inside the circumcircle of any triangle in the mesh. Delauny triangulation for mesh generation results in well-shaped triangles that are useful in various applications, including computational geometry, finite element analysis, and computer graphics.


## Algorithm 
The Delaunay triangulation algorithm follows these steps: 
1. **Initialization**: 
   - Create a super triangle that encompasses all input points. This triangle is large enough to contain all the given points within it. 
   - Add this super triangle to the mesh. 

2. **Adding Points**: 
   - Iterate over each input point and find the triangle that contains it. 
   - If the point lies inside a triangle, split the triangle into three new triangles. 
   - If the point lies on an edge, split the triangle into two new triangles and handle neighboring triangles accordingly. 

3. **Edge Flipping**: 
   - After adding each point, check the circumcircles of the neighboring triangles. 
   - If a point lies inside the circumcircle of a triangle, swap the edge between the triangles to maintain the Delaunay property. 

4. **Maintaining Neighbors**: 
   - Update neighbor relationships for all affected triangles after adding points and swapping edges. 
   - Ensure the mesh remains consistent and maintains the Delaunay triangulation properties. 

5. **Triangle Removal** 
   - When the points are all processed, remove all triangles connected to points of the super triangle. 

6. **Triangle Equilateralization**
   - If any triangle has an angle under 40 degrees, add a point in that triangle's circumcenter. 

7. **Visualization**: 
   - Use Qt's `QGraphicsScene` and `QGraphicsView` to visualize the resulting triangulation.


## Class Structure

- **Point Class**: Represents a point in 2D space.
- **Triangle Class**: Represents a triangle formed by three points.
- **Mesh Class**: Manages a collection of points and triangles to build and maintain the Delaunay triangulation mesh, including adding points and handling neighbor relationships.
- **QTriangle Class**: Integrates with Qt to visualize the triangulation using `QGraphicsScene` and `QGraphicsView`.


## Future Modifications

### Interactive GUI

- **Add Point Placement**: Implement functionality to allow users to click on the canvas to place points for triangulation.
- **Real-Time Visualization**: Enable real-time updates to the mesh as points are added or moved, observing the effects on the triangulation.

### Performance Improvements

- **Algorithm Optimization**: Explore ways to optimize the Delaunay triangulation algorithm for higher performance.
