#ifndef POINT_H
#define POINT_H

// Class representing a 2D point
class Point {
private:
    float fx{0.0}, fy{0.0};  // x and y coordinates of the point

public:
    // Default constructor
    Point();

    // Parameterized constructor
    Point(float fx, float fy);

    // Getter for x coordinate
    float getX();

    // Setter for x coordinate
    void setX(float fx);

    // Getter for y coordinate
    float getY();

    // Setter for y coordinate
    void setY(float fy);

    // Function to find the distance to another point
    double findDistance(Point p2);

    // Function to find the slope to another point
    double findSlope(Point p2);

    // Function to check if a target point is between the current point and another point
    bool betweenPoints(Point p2, Point pTargetPoint);

    // Function to check if the current point is equal to another point
    bool equals(Point pOther);
};

#endif // POINT_H
