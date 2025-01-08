# Full-Stack Mesh Generation Project

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Technical Highlights](#technical-highlights)
4. [Algorithm](#algorithm)
6. [Demo](#demo)

---

## Overview

This project is a real-time triangulation engine that dynamically generates and updates meshes based on user-defined points. Using the **Delaunay triangulation algorithm**, the application ensures that the resulting meshes maintain desirable properties like well-shaped triangles and adherence to the Delaunay condition.

The project is ideal for applications in computer graphics, simulations, and finite element analysis, leveraging its high interactivity and reliable triangulation techniques.

---

## Features

- **Dynamic Mesh Updates**: The triangulation engine updates meshes in real time as users add points.
- **Industry-Standard Algorithm**: Implements the highly regarded **Delaunay triangulation algorithm**, a staple in the graphics industry.
- **User-Interactive Interface**: Seamless user interaction with immediate visualization of updated meshes.
- **Optimized for Performance**: High-performance backend ensures efficient handling of large datasets.
- **Real-Time Communication**: WebSocket-based API enables low-latency communication between the backend and frontend.

---

## Technical Highlights

1. **C++ Triangulation Engine**:
   - Designed for high performance, it calculates and updates meshes efficiently.
   - Implements edge-flipping and circumcircle calculations to maintain the Delaunay condition.

2. **React Frontend**:
   - Provides an intuitive interface for users to add points and see immediate results.
   - Features real-time rendering of mesh updates.

3. **WebSocket Integration**:
   - Enables continuous, low-latency communication between the frontend and backend for seamless updates.

---

## Algorithm

The Delaunay triangulation algorithm consists of the following steps:

1. **Initialization**:
   - A large super triangle is created to encompass all input points.
   - This super triangle serves as the starting mesh structure.

2. **Adding Points**:
   - For each input point, locate the triangle containing it.
   - Split the triangle into smaller triangles, ensuring the mesh maintains Delaunay properties.

3. **Edge Flipping**:
   - Check the circumcircles of adjacent triangles after adding points.
   - Flip edges where necessary to maintain the Delaunay condition (no point lies inside another triangle's circumcircle).

4. **Maintaining Neighbors**:
   - Update the relationships between triangles to ensure consistency in the mesh structure.


---

## Demo

![Demo of Real-Time Mesh Update](./demo.gif)

The above animation shows real-time triangulation and mesh updates as the user places points on the canvas.
