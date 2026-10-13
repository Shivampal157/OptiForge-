#include "optiforge/optimizer.h"
#include "optiforge/analysis.h"

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <utility>

namespace optiforge {

OptimizationStats Optimizer::run(Function& function) const {
    OptimizationStats stats;
    constant_fold(function, stats);
    common_subexpression_elimination(function, stats);
    dead_code_elimination(function, stats);
    return stats;
}

OptimizationReport Optimizer::run_with_report(Function& function) const {
    OptimizationReport report;
    for (const auto& instruction : function.instructions()) {
        report.instructions_before += instruction.live ? 1U : 0U;
    }
    report.stats = run(function);
    for (const auto& instruction : function.instructions()) {
        report.instructions_after += instruction.live ? 1U : 0U;
    }
    report.peak_live_values = analyze_liveness(function).peak_live_values;
    return report;
}

void Optimizer::constant_fold(Function& function, OptimizationStats& stats) {
    std::unordered_map<ValueId, std::int64_t> constants;
    for (auto& instruction : function.instructions()) {
        if (!instruction.live) {
            continue;
        }
        if (instruction.op == Op::Constant) {
            constants[instruction.result] = instruction.immediate;
            continue;
        }
        if (instruction.op != Op::Add && instruction.op != Op::Sub &&
            instruction.op != Op::Mul) {
            continue;
        }
        const auto lhs = constants.find(instruction.lhs);
        const auto rhs = constants.find(instruction.rhs);
        if (lhs == constants.end() || rhs == constants.end()) {
            continue;
        }
        switch (instruction.op) {
            case Op::Add: instruction.immediate = lhs->second + rhs->second; break;
            case Op::Sub: instruction.immediate = lhs->second - rhs->second; break;
            case Op::Mul: instruction.immediate = lhs->second * rhs->second; break;
            default: break;
        }
        instruction.op = Op::Constant;
        instruction.lhs = InvalidValue;
        instruction.rhs = InvalidValue;
        constants[instruction.result] = instruction.immediate;
        ++stats.folded_constants;
    }
}

void Optimizer::common_subexpression_elimination(Function& function,
                                                  OptimizationStats& stats) {
    struct Key {
        Op op;
        ValueId lhs;
        ValueId rhs;
        bool operator==(const Key& other) const {
            return op == other.op && lhs == other.lhs && rhs == other.rhs;
        }
    };
    struct Hash {
        std::size_t operator()(const Key& key) const {
            return static_cast<std::size_t>(key.op) * 1315423911U ^
                   (static_cast<std::size_t>(key.lhs) << 16U) ^ key.rhs;
        }
    };

    std::unordered_map<Key, ValueId, Hash> expressions;
    std::unordered_map<ValueId, ValueId> replacement;
    for (auto& instruction : function.instructions()) {
        if (!instruction.live ||
            (instruction.op != Op::Add && instruction.op != Op::Sub &&
             instruction.op != Op::Mul)) {
            continue;
        }
        auto resolve = [&replacement](ValueId id) {
            while (replacement.find(id) != replacement.end()) {
                id = replacement.at(id);
            }
            return id;
        };
        instruction.lhs = resolve(instruction.lhs);
        instruction.rhs = resolve(instruction.rhs);
        const Key key{instruction.op, instruction.lhs, instruction.rhs};
        const auto insertion = expressions.emplace(key, instruction.result);
        if (!insertion.second) {
            replacement[instruction.result] = insertion.first->second;
            instruction.live = false;
            ++stats.eliminated_common_subexpressions;
        }
    }
    for (auto& instruction : function.instructions()) {
        if (instruction.live) {
            if (replacement.find(instruction.lhs) != replacement.end()) {
                instruction.lhs = replacement[instruction.lhs];
            }
            if (replacement.find(instruction.rhs) != replacement.end()) {
                instruction.rhs = replacement[instruction.rhs];
            }
        }
    }
}

void Optimizer::dead_code_elimination(Function& function, OptimizationStats& stats) {
    std::unordered_map<ValueId, std::size_t> uses;
    for (const auto& instruction : function.instructions()) {
        if (!instruction.live) {
            continue;
        }
        if (instruction.lhs != InvalidValue) {
            ++uses[instruction.lhs];
        }
        if (instruction.rhs != InvalidValue) {
            ++uses[instruction.rhs];
        }
    }
    for (auto& instruction : function.instructions()) {
        if (instruction.live && has_result(instruction.op) &&
            uses[instruction.result] == 0) {
            instruction.live = false;
            ++stats.eliminated_instructions;
        }
    }
}

}  // namespace optiforge
