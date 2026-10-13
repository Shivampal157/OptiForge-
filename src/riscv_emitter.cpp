#include "optiforge/riscv_emitter.h"

#include <sstream>
#include <unordered_map>

namespace optiforge {

std::string emit_riscv(const Function& function) {
    std::ostringstream output;
    std::unordered_map<ValueId, int> registers;
    int next_register = 5;
    output << "# OptiForge educational RV64-like output\n";
    for (const auto& instruction : function.instructions()) {
        if (!instruction.live) {
            continue;
        }
        auto reg = [&registers](ValueId value) {
            return registers.at(value);
        };
        if (instruction.result != InvalidValue) {
            registers[instruction.result] = next_register++;
        }
        switch (instruction.op) {
            case Op::Constant:
                output << "  li x" << reg(instruction.result) << ", "
                       << instruction.immediate << '\n';
                break;
            case Op::Add:
                output << "  add x" << reg(instruction.result) << ", x"
                       << reg(instruction.lhs) << ", x" << reg(instruction.rhs) << '\n';
                break;
            case Op::Sub:
                output << "  sub x" << reg(instruction.result) << ", x"
                       << reg(instruction.lhs) << ", x" << reg(instruction.rhs) << '\n';
                break;
            case Op::Mul:
                output << "  mul x" << reg(instruction.result) << ", x"
                       << reg(instruction.lhs) << ", x" << reg(instruction.rhs) << '\n';
                break;
            case Op::Load:
                output << "  ld x" << reg(instruction.result) << ", 0(" 
                       << instruction.name << ")\n";
                break;
            case Op::Store:
                output << "  sd x" << reg(instruction.lhs) << ", 0("
                       << instruction.name << ")\n";
                break;
            case Op::Return:
                output << "  mv a0, x" << reg(instruction.lhs) << "\n  ret\n";
                break;
        }
    }
    return output.str();
}

}  // namespace optiforge
