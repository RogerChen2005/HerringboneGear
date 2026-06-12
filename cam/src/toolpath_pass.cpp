#include "toolpath_pass.h"
#include "gear_geometry.h"

ToolpathPass::ToolpathPass(const GearParams& params)
    : nc_(params), params_(params), derived_(params),
      twist_(gear::TwistAngle(params, params.F))
{
}

NCConverter& ToolpathPass::Generate(int teeth_count)
{
    const double tooth_step = 2.0 * M_PI / params_.z;
    int cnt = teeth_count <= params_.z ? teeth_count : params_.z;
    nc_.ClearAll();
    nc_.Comment(std::string("=== ") + PassName() + " PASS ===");
    for (int n = 0; n < cnt; ++n)
        CutTooth(n * tooth_step);
    return nc_;
}

//void ToolpathPass::CutAcross(const Point& p)
//{
//    Point mid = p.rotated(twist_);
//    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 0 : 2), -90, -gear::RadToDeg(p.angle()));
//    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
//    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 2 : 0), -90, -gear::RadToDeg(p.angle()));
//    reverse_ = !reverse_;
//}

void ToolpathPass::CutAcross(const Point& p, const double r_cutter) {
    Point mid = p.rotated(twist_);
    double beta = gear::DegToRad(params_.beta);
    Point start = p.rotated(-gear::TwistAngle(params_, -(r_cutter) * std::tan(beta) * std::cos(beta)));//刀具回去 
    double temp = - (r_cutter) * std::tan(beta) * std::cos(beta) / derived_.rd;//需要传入gear参数 

    nc_.CutLine(start.y, start.x, params_.F * (reverse_ ? -temp : 2), -90, -gear::RadToDeg(start.angle()));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
    nc_.CutLine(start.y, start.x, params_.F * (reverse_ ? 2 : -temp), -90, -gear::RadToDeg(start.angle()));
    reverse_ = !reverse_;
}


void ToolpathPass::CutAcrossFinal(const Point& p, const double r_cutter)
{
    Point mid = p.rotated(twist_);
    double beta = gear::DegToRad(params_.beta);
    Point mid2 = p.rotated(twist_ + (r_cutter / std::cos(beta) - r_cutter) / derived_.rd);
    Point start = p.rotated(-gear::TwistAngle(params_, - r_cutter *std::tan(beta) * std::cos(beta)));//刀具回去 
    double temp = -r_cutter * std::tan(beta) * std::cos(beta) / derived_.rd;//需要传入gear参数 

    nc_.CutLine(start.y, start.x, params_.F * (reverse_ ? -temp : 2), -90, -gear::RadToDeg(start.angle()));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
    nc_.CutLine(mid2.y, mid2.x, params_.F, -90, -gear::RadToDeg(mid2.angle()));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
    nc_.CutLine(start.y, start.x, params_.F * (reverse_ ? 2 : -temp), -90, -gear::RadToDeg(start.angle()));
    reverse_ = !reverse_;
}