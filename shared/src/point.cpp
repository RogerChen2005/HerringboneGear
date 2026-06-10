#include "point.h"
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Point::Point(double x, double y) : x(x), y(y) {}

Point Point::fromPolar(double r, double p) {
    return Point(r * std::cos(p), r * std::sin(p));
}

// ---------------------------------------------------------------------------
// Movement
// ---------------------------------------------------------------------------

Point& Point::move(double dx, double dy) {
    x += dx;
    y += dy;
    return *this;
}

Point& Point::movePolar(double r, double p) {
    x += r * std::cos(p);
    y += r * std::sin(p);
    return *this;
}

Point& Point::rotate(double p) {
    double r = radius();
    double a = angle();
    x = r * std::cos(a + p);
    y = r * std::sin(a + p);
    return *this;
}

Point Point::rotated(double p) const{
    double r = radius();
    double a = angle();
    return Point::fromPolar(r,a + p);
}

Point Point::moved(double dx, double dy) const {
    return Point(x + dx, y + dy);
}

Point Point::movedPolar(double r, double p) const {
    return Point(x + r * std::cos(p), y + r * std::sin(p));
}

// ---------------------------------------------------------------------------
// Query
// ---------------------------------------------------------------------------

double Point::distanceTo(const Point& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

double Point::angle() const {
    return std::atan2(y, x);
}

double Point::radius() const {
    return std::sqrt(x * x + y * y);
}
