#include "optiforge/analysis.h"
#include "optiforge/optimizer.h"

#include <cassert>

int main() {
    using namespace optiforge;
    Function function;
    const auto a = function.load("a0");
    const auto b = function.constant(5);
    const auto c = function.constant(7);
    const auto d = function.constant(4);
    const auto folded = function.add(c, d);
    (void)folded;
    const auto first = function.add(a, b);
    const auto duplicate = function.add(a, b);
    const auto answer = function.mul(first, duplicate);
    function.ret(answer);

    const auto stats = Optimizer{}.run(function);
    assert(stats.folded_constants >= 1);
    assert(stats.eliminated_common_subexpressions >= 1);
    assert(function.dump().find("return") != std::string::npos);

    const auto liveness = analyze_liveness(function);
    assert(liveness.peak_live_values > 0);
}
