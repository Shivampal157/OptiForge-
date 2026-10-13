#include "optiforge/analysis.h"
#include "optiforge/optimizer.h"
#include "optiforge/riscv_emitter.h"

#include <iomanip>
#include <iostream>
#include <string>

namespace {

optiforge::Function build_polynomial() {
    using namespace optiforge;
    Function function;
    const auto x = function.load("a0");
    const auto c1 = function.constant(3);
    const auto c2 = function.constant(7);
    const auto c3 = function.constant(11);
    const auto x2 = function.mul(x, x, "x2");
    const auto x3 = function.mul(x2, x, "x3");
    const auto left = function.add(x3, function.mul(c1, x2), "left");
    const auto right = function.add(function.mul(c2, x), c3, "right");
    const auto duplicate = function.add(x3, function.mul(c1, x2), "duplicate");
    function.ret(function.add(function.add(left, right), duplicate));
    return function;
}

optiforge::Function build_reduction() {
    using namespace optiforge;
    Function function;
    const auto value = function.load("a0");
    const auto one = function.constant(1);
    const auto two = function.constant(2);
    const auto a = function.add(value, one);
    const auto b = function.add(value, one);
    const auto c = function.mul(a, two);
    const auto d = function.mul(b, two);
    function.ret(function.add(c, d));
    return function;
}

void run_case(const std::string& name, optiforge::Function function) {
    const auto report = optiforge::Optimizer{}.run_with_report(function);
    const double reduction =
        report.instructions_before == 0
            ? 0.0
            : 100.0 * static_cast<double>(report.instructions_before -
                                           report.instructions_after) /
                  static_cast<double>(report.instructions_before);
    std::cout << std::left << std::setw(14) << name
              << " before=" << std::setw(2) << report.instructions_before
              << " after=" << std::setw(2) << report.instructions_after
              << " reduction=" << std::fixed << std::setprecision(1)
              << std::setw(5) << reduction << "%"
              << " peak_live=" << report.peak_live_values
              << " folds=" << report.stats.folded_constants
              << " cse=" << report.stats.eliminated_common_subexpressions
              << " dce=" << report.stats.eliminated_instructions << '\n';
    std::cout << optiforge::emit_riscv(function) << '\n';
}

}  // namespace

int main() {
    std::cout << "OptiForge optimization benchmark\n";
    std::cout << "name           instruction metrics\n";
    run_case("polynomial", build_polynomial());
    run_case("reduction", build_reduction());
}
