#pragma once

#include "roughing.h"
#include "finishing.h"

// Generate a complete roughing program (header + toolpath + footer).
NCConverter generateRoughing(const GearParams& g, const RoughParams& p);

// Generate a complete finishing program (header + toolpath + footer).
NCConverter generateFinishing(const GearParams& g, const FinishParams& p);
