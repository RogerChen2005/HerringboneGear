#pragma once

#include "gear_params.h"
#include "point.h"
#include <vector>
#include <string>
#include <utility>

using Profile = std::vector<Point>;

// Involute gear geometry — shared by roughing and finishing.
namespace gear {

// Check parameters for range and geometric consistency.
// Returns an empty string if valid, otherwise a human-readable error message.
std::string Validate(const GearParams& g);

Profile ComputeProfile(const GearParams& g);
double  TwistAngle(const GearParams& g, double z);
double  HelixAngleDeg(const GearParams& g, double z);
std::pair<double, double> RotatePoint(double x, double y, double angle_rad);
double  RadToDeg(double rad);
double  DegToRad(double deg);
double calc_theta(const double _rb, const double _rd, const double _rg);
double calc_radius(const double _theta, const double _rd, const double _rg);


} // namespace gear
