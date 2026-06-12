#pragma once

#include "nc_converter.h"
#include "gear_derived.h"
#include "point.h"

// Common state and behaviour shared by all machining passes
// (roughing, finishing): NC output, derived geometry, herringbone
// twist and the alternating cut direction.
class ToolpathPass {
public:
    explicit ToolpathPass(const GearParams& params);
    virtual ~ToolpathPass() = default;

    // Generate the pass for the first teeth_count teeth (clamped to z).
    // Returns the NC toolpath (no program header/footer).
    NCConverter& Generate(int teeth_count);

protected:
    NCConverter nc_;
    GearParams  params_;
    GearDerived derived_;
    double      twist_;
    bool        reverse_ = false;

    // One full cut across the face width: entry → V-apex → exit.
    // Alternates direction on every call.
    void CutAcross(const Point& p, const double r_cutter);
    void CutAcrossFinal(const Point& p, const double r_cutter);

private:
    // Per-tooth toolpath, implemented by each pass.
    virtual void CutTooth(double base) = 0;
    virtual const char* PassName() const = 0;
};
