#pragma once

#include "gear_params.h"
#include <vector>
#include <utility>

using Profile = std::vector<std::pair<double, double>>;

// Involute gear geometry — shared by roughing and finishing.
namespace gear {

Profile ComputeProfile(const GearParams& g);
double  TwistAngle(const GearParams& g, double z);
double  HelixAngleDeg(const GearParams& g, double z);
std::pair<double, double> RotatePoint(double x, double y, double angle_rad);
double  RadToDeg(double rad);
double calc_theta(const double _rb, const double _rd, const double _rg);
double calc_radius(const double _theta, const double _rd, const double _rg);


} // namespace gear
