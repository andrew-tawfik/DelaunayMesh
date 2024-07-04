#include "point.h"
#include <cmath>
#include <iostream>

// Default constructor
Point::Point() {}

// Parameterized constructor
Point::Point(float fx, float fy) : fx(fx), fy(fy) {}

// Getter for x
float Point::getX()
{
    return fx;
}

// Setter for x
void Point::setX(float fx)
{
    this->fx = fx;
}

// Getter for y
float Point::getY()
{
    return fy;
}

// Setter for y
void Point::setY(float fy)
{
    this->fy = fy;
}

double Point::findDistance(Point p2)
{
    double dx1 = fx;
    double dy1 = fy;

    double dx2 = p2.getX();
    double dy2 = p2.getY();

    double dy_t = dy2 - dy1;
    double dx_t = dx2 - dx1;

    double dDistance = std::sqrt(std::pow(dy_t, 2.0) + std::pow(dx_t, 2.0));

    return dDistance;
}

double Point::findSlope(Point p2)
{
    double dx1 = fx;
    double dy1 = fy;

    double dx2 = p2.getX();
    double dy2 = p2.getY();

    double dSlope = (dy2 - dy1) / (dx2 - dx1);

    return dSlope;
}

bool Point::betweenPoints(Point p2, Point pTargetPoint)
{
    double db = p2.getY() - (p2.getX() * findSlope(p2));
    double dtpx = pTargetPoint.getX();
    double dtpy = pTargetPoint.getY();

    if (dtpy == findSlope(p2) * dtpx + db)
    {
        if ((dtpx >= (std::fmin(getX(), p2.getX()))) && (dtpx <= (std::fmax(getX(), p2.getX()))))
        {
            if ((dtpy >= (std::fmin(getY(), p2.getY()))) && (dtpy <= (std::fmax(getY(), p2.getY()))))
            {
                return true;
            }
        }
    }
    else
    {
        return false;
    }
}

bool Point::equals(Point pOther)
{
    return fx == pOther.getX() && fy == pOther.getY();
}
