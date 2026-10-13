#include "optiforge/analysis.h"
#include "optiforge/optimizer.h"
#include "optiforge/riscv_emitter.h"

#include <iostream>

int main() {
    using namespace optiforge;
    Function function;
    const auto input = function.load("a0");
    const auto three = function.constant(3);
    const auto five = function.add(input, three, "five");
    const auto duplicate = function.add(input, three, "duplicate");
    const auto product = function.mul(five, duplicate, "product");
    const auto dead = function.mul(three, three, "dead");
    (void)dead;
    function.ret(product);

    std::cout << "=== Before optimization ===\n" << function.dump();
    const auto stats = Optimizer{}.run(function);
    const auto liveness = analyze_liveness(function);
    std::cout << "\n=== After optimization ===\n" << function.dump();
    std::cout << "\n=== RISC-V-like output ===\n" << emit_riscv(function);
    std::cout << "folded_constants=" << stats.folded_constants
              << " cse=" << stats.eliminated_common_subexpressions
              << " dead_code=" << stats.eliminated_instructions
              << " peak_live_values=" << liveness.peak_live_values << '\n';
}
