#pragma once

#include <cmath>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

// 2-D point used throughout the CAM toolpath generation.
//
// Construct from Cartesian (x, y) or polar (r, p) coordinates,
// and translate along an arbitrary vector.
struct Point {
    double x = 0.0;
    double y = 0.0;

    // Cartesian constructor
    Point(double x, double y);

    // Polar constructor: r = radius, p = angle in radians
    static Point fromPolar(double r, double p);

    // Translate the point by vector (dx, dy)
    Point& move(double dx, double dy);

    // Translate the point by polar vector (r, p), p in radians
    Point& movePolar(double r, double p);

    Point& rotate(double p);

    Point rotated(double p) const;

    // Return a new point translated by (dx, dy)
    Point moved(double dx, double dy) const;

    // Return a new point translated by polar vector (r, p)
    Point movedPolar(double r, double p) const;

    // Distance to another point
    double distanceTo(const Point& other) const;

    // Angle from origin to this point (atan2)
    double angle() const;

    double radius() const;
};
