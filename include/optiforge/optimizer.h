#pragma once

#include "optiforge/ir.h"

#include <cstddef>
#include <unordered_map>

namespace optiforge {

struct OptimizationStats {
    std::size_t folded_constants{0};
    std::size_t eliminated_instructions{0};
    std::size_t eliminated_common_subexpressions{0};
};

struct OptimizationReport {
    OptimizationStats stats;
    std::size_t instructions_before{0};
    std::size_t instructions_after{0};
    std::size_t peak_live_values{0};
};

class Optimizer {
public:
    OptimizationStats run(Function& function) const;
    OptimizationReport run_with_report(Function& function) const;

private:
    static void constant_fold(Function& function, OptimizationStats& stats);
    static void common_subexpression_elimination(Function& function,
                                                  OptimizationStats& stats);
    static void dead_code_elimination(Function& function, OptimizationStats& stats);
};

}  // namespace optiforge
