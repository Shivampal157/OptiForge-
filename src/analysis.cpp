#include "optiforge/analysis.h"

#include <algorithm>
#include <unordered_set>

namespace optiforge {

LivenessResult analyze_liveness(const Function& function) {
    LivenessResult result;
    std::unordered_set<ValueId> live;
    for (auto it = function.instructions().rbegin();
         it != function.instructions().rend(); ++it) {
        if (!it->live) {
            continue;
        }
        live.erase(it->result);
        if (it->lhs != InvalidValue) {
            live.insert(it->lhs);
            ++result.uses[it->lhs];
        }
        if (it->rhs != InvalidValue) {
            live.insert(it->rhs);
            ++result.uses[it->rhs];
        }
        result.peak_live_values = std::max(result.peak_live_values, live.size());
    }
    return result;
}

}  // namespace optiforge
