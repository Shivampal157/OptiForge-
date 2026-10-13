#pragma once

#include "optiforge/ir.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace optiforge {

struct LivenessResult {
    std::unordered_map<ValueId, std::size_t> uses;
    std::size_t peak_live_values{0};
};

LivenessResult analyze_liveness(const Function& function);

}  // namespace optiforge
