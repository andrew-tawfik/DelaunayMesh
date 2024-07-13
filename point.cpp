#include "point.h"
#include <cmath>
#include <iostream>

// Default constructor: initializes the point at (0.0, 0.0)
Point::Point() {}

// Parameterized constructor: initializes the point at (fx, fy)
Point::Point(float fx, float fy) : fx(fx), fy(fy) {}

// Getter for x coordinate
float Point::getX() const{
    return fx;
}

// Setter for x coordinate
void Point::setX(float fx) {
    this->fx = fx;
}

// Getter for y coordinate
float Point::getY() const{
    return fy;
}

// Setter for y coordinate
void Point::setY(float fy) {
    this->fy = fy;
}

// Function to find the distance to another point
double Point::findDistance(const Point& p2) const{
    double dx1 = fx;
    double dy1 = fy;

    double dx2 = p2.getX();
    double dy2 = p2.getY();

    double dy_t = dy2 - dy1;
    double dx_t = dx2 - dx1;

    double dDistance = std::sqrt(std::pow(dy_t, 2.0) + std::pow(dx_t, 2.0));

    return dDistance;
}

// Function to find the slope to another point
double Point::findSlope(const Point& p2) const{
    double dx1 = fx;
    double dy1 = fy;

    double dx2 = p2.getX();
    double dy2 = p2.getY();

    double dSlope = (dy2 - dy1) / (dx2 - dx1);

    return dSlope;
}

// Function to check if a target point is between the current point and another point
bool Point::betweenPoints(const Point& p2, const Point& pTargetPoint) const {
    double db = p2.getY() - (p2.getX() * findSlope(p2));
    double dtpx = pTargetPoint.getX();
    double dtpy = pTargetPoint.getY();

    if (dtpy == findSlope(p2) * dtpx + db) {
        if ((dtpx >= (std::fmin(getX(), p2.getX()))) && (dtpx <= (std::fmax(getX(), p2.getX())))) {
            if ((dtpy >= (std::fmin(getY(), p2.getY()))) && (dtpy <= (std::fmax(getY(), p2.getY())))) {
                return true;
            }
        }
    }
    return false;
}

// Function to check if the current point is equal to another point
bool Point::equals(const Point& pOther) const {
    return fx == pOther.getX() && fy == pOther.getY();
}
