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

void ToolpathPass::CutAcross(const Point& p)
{
    Point mid = p.rotated(twist_);
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 0 : 2), -90, -gear::RadToDeg(p.angle()));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 2 : 0), -90, -gear::RadToDeg(p.angle()));
    reverse_ = !reverse_;
}
