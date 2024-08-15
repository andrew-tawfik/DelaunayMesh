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
    float getX() const;

    // Setter for x coordinate
    void setX(float fx);

    // Getter for y coordinate
    float getY() const;

    // Setter for y coordinate
    void setY(float fy);

    // Function to find the distance between another point
    double findDistance(const Point& p2) const;

    // Function to find the slope to another point
    double findSlope(const Point& p2) const;

};

#endif // POINT_H
