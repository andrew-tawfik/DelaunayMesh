# Delaunay Triangulation Project

## Table of Contents

1. [Overview](#overview)
2. [Algorithm](#algorithm)
3. [Class Structure](#class-structure)
4. [Functions](#functions)
   - [Point Class](#point-class)
   - [Triangle Class](#triangle-class)
   - [Mesh Class](#mesh-class)
   - [QTriangle Class](#qtriangle-class)
5. [Future Modifications](#future-modifications)


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

6. **Visualization**: 
   - Use Qt's `QGraphicsScene` and `QGraphicsView` to visualize the resulting triangulation.


## Class Structure

- **Point Class**: Represents a point in 2D space.
- **Triangle Class**: Represents a triangle formed by three points.
- **Mesh Class**: Manages a collection of points and triangles to build and maintain the Delaunay triangulation mesh, including creating a super triangle, adding points, and handling neighbor relationships.
- **QTriangle Class**: Integrates with Qt to visualize the triangulation using `QGraphicsScene` and `QGraphicsView`.

## Functions

### Point Class

The `Point` class represents a 2D point with methods for basic geometric calculations:

- `getX()`, `getY()`: Get the x and y coordinates.
- `setX(float fx)`, `setY(float fy)`: Set the x and y coordinates.
- `findDistance(const Point& p2)`: Calculate the distance to another point.
- `findSlope(const Point& p2)`: Calculate the slope to another point.
- `betweenPoints(const Point& p2, const Point& pTargetPoint)`: Check if a point lies between two other points.
- `equals(const Point& pOther)`: Check if two points are equal.

### Triangle Class

The `Triangle` class represents a triangle with methods for geometric properties and operations:

- `getLength(int iSide)`: Get the length of a side.
- `getPerimeter()`: Get the perimeter.
- `getAng(int iAngle)`: Get an angle.
- `getArea()`: Get the area.
- `contains(const Point& ptTargetPoint)`: Check if the triangle contains a point.
- `getCircumcenter()`: Get the circumcenter.
- `isInCircumcircle(const Point& pt)`: Check if a point is inside the circumcircle.

### Mesh Class

The `Mesh` class manages the collection of points and triangles:

- `setShape(const std::vector<Point>& vecPt)`: Set the shape of the mesh.
- `getShape()`: Get the shape of the mesh.
- `setTriVector(const std::vector<Triangle>& vecTri)`: Set the vector of triangles.
- `getTriVector()`: Get the vector of triangles.
- `setPointIndices(int iNumPoints)`: Set the point indices.
- `getPointIndices()`: Get the point indices.
- `setTriangleIndices(int numPoints)`: Set the triangle indices.
- `getTriangleIndices()`: Get the triangle indices.
- `superTriangle()`: Create a super triangle that encloses all points.
- `buildMesh()`: Build the mesh from points and triangles.
- `createTriangles(int iTriangleIndex, int iPointIndex)`: Create triangles.
- `findContainingTriangle(const Point& ptTargetPoint)`: Find the triangle containing a point.
- `swapEdge(int iTri1, int iTri2)`: Swap edges between triangles.
- `swapAll(std::queue<int>& neighbourQueue, int iPointIndex)`: Swap all necessary edges.
- `checkNeighboringCircumcircles(int iTriangleIndex, int iPointIndex, int iEdgeIndex)`: Check circumcircles of neighboring triangles.

### QTriangle Class

The `QTriangle` class integrates with Qt to provide visualization capabilities:

- `QTriangle(const QPointF& p0, const QPointF& p1, const QPointF& p2, QGraphicsItem *parent = nullptr)`: Constructor to initialize the triangle with three points.
- `paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)`: Method to draw the triangle.
- `boundingRect() const`: Method to get the bounding rectangle of the triangle.



## Future Modifications

### Interactive GUI

- **Add Point Placement**: Implement functionality to allow users to click on the canvas to place points for triangulation.
- **Real-Time Visualization**: Enable real-time updates to the mesh as points are added or moved.

### Enhanced Mesh Features

- **Mesh Editing**: Allow users to modify the mesh interactively, such as adding, removing, or moving points and observing the effects on the triangulation.

### Performance Improvements

- **Algorithm Optimization**: Explore ways to optimize the Delaunay triangulation algorithm for higher performance.
