# ToDoList

### June 27

getNeighbours from sides ('A', 'B', 'C')

findTriangle function using contains:

- looks in current triangle, checks if all determinant are negative 
- if any are positive check that sides neighbour
- this is the shortest path


### June 28

Triangle class:

  - need to implement pointIndices array
  - need to remove the array
  - make sure that the circumcirle method also work for right-angled triangles

Mesh class :

1. Each triangle created needs to have 
- an index
- array of pointIndices
- array of neighbourIndices

2. Program Workflow
- Find in which triangle does the point lie inside from determinant signs
- createTriangles()
- Check if the point lies inside neighbour circumcircles 
- if True, create Triangles

3. 
