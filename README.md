## Triangle Mesh Project
This project is a C++ implementation of a triangle mesh generator. 
The program defines a Point class for representing points in 2D space, a Triangle class with various geometric functions, and a Mesh class that builds and manages a mesh of triangles. 


### Program Workflow
1. Find the Triangle Containing the Point:
    - Use determinant signs to determine which triangle contains a point.
    - If the point is not in the current triangle, check neighboring triangles.
2. Create New Triangles:
    - Create new triangles by connecting the point to the vertices of the containing triangle.
    - Update the mesh with the new triangles.

3. Check Neighbors:
    - Check if the point lies inside the circumcircles of neighboring triangles.
    - If true, create additional triangles and update neighbors.
    
