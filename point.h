#ifndef POINT_H
#define POINT_H

class Point
{

private:
    float fx{0.0}, fy{0.0};

public:
    // Default constructor
    Point();

    // Parameterized constructor
    Point(float fx, float fy);

    // Getter for x
    float getX();

    // Setter for x
    void setX(float fx);

    // Getter for y
    float getY();

    // Setter for y
    void setY(float fy);

    double findDistance(Point p2);

    double findSlope(Point p2);

    bool betweenPoints(Point p2, Point pTargetPoint);

    bool equals( Point pOther);

};

#endif // POINT_H

