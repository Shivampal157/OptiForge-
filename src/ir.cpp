#include "optiforge/ir.h"

#include <sstream>
#include <stdexcept>

namespace optiforge {

namespace {
ValueId make_binary(std::vector<Instruction>& instructions, ValueId& next_id,
                    Op op, ValueId lhs, ValueId rhs, std::string name) {
    if (lhs == InvalidValue || rhs == InvalidValue) {
        throw std::invalid_argument("binary instruction requires two values");
    }
    const ValueId result = next_id++;
    instructions.push_back({result, op, lhs, rhs, 0, std::move(name), true});
    return result;
}
}  // namespace

ValueId Function::constant(std::int64_t value) {
    const ValueId result = next_id_++;
    instructions_.push_back({result, Op::Constant, InvalidValue, InvalidValue,
                              value, {}, true});
    return result;
}

ValueId Function::add(ValueId lhs, ValueId rhs, std::string name) {
    return make_binary(instructions_, next_id_, Op::Add, lhs, rhs, std::move(name));
}

ValueId Function::sub(ValueId lhs, ValueId rhs, std::string name) {
    return make_binary(instructions_, next_id_, Op::Sub, lhs, rhs, std::move(name));
}

ValueId Function::mul(ValueId lhs, ValueId rhs, std::string name) {
    return make_binary(instructions_, next_id_, Op::Mul, lhs, rhs, std::move(name));
}

ValueId Function::load(std::string name) {
    const ValueId result = next_id_++;
    instructions_.push_back({result, Op::Load, InvalidValue, InvalidValue,
                              0, std::move(name), true});
    return result;
}

void Function::store(ValueId value, std::string name) {
    if (value == InvalidValue) {
        throw std::invalid_argument("store requires a value");
    }
    instructions_.push_back({InvalidValue, Op::Store, value, InvalidValue,
                              0, std::move(name), true});
}

void Function::ret(ValueId value) {
    if (value == InvalidValue) {
        throw std::invalid_argument("return requires a value");
    }
    instructions_.push_back({InvalidValue, Op::Return, value, InvalidValue,
                              0, {}, true});
}

const Instruction* Function::find(ValueId id) const {
    for (const auto& instruction : instructions_) {
        if (instruction.result == id) {
            return &instruction;
        }
    }
    return nullptr;
}

Instruction* Function::find(ValueId id) {
    for (auto& instruction : instructions_) {
        if (instruction.result == id) {
            return &instruction;
        }
    }
    return nullptr;
}

std::string Function::dump() const {
    std::ostringstream output;
    for (const auto& instruction : instructions_) {
        if (!instruction.live) {
            continue;
        }
        if (instruction.result != InvalidValue) {
            output << "%" << instruction.result << " = ";
        }
        switch (instruction.op) {
            case Op::Constant:
                output << "const " << instruction.immediate;
                break;
            case Op::Add:
            case Op::Sub:
            case Op::Mul:
                output << op_name(instruction.op) << " %" << instruction.lhs
                       << ", %" << instruction.rhs;
                break;
            case Op::Load:
                output << "load " << instruction.name;
                break;
            case Op::Store:
                output << "store %" << instruction.lhs << ", " << instruction.name;
                break;
            case Op::Return:
                output << "return %" << instruction.lhs;
                break;
        }
        output << '\n';
    }
    return output.str();
}

const char* op_name(Op op) {
    switch (op) {
        case Op::Constant: return "const";
        case Op::Add: return "add";
        case Op::Sub: return "sub";
        case Op::Mul: return "mul";
        case Op::Load: return "load";
        case Op::Store: return "store";
        case Op::Return: return "return";
    }
    return "unknown";
}

bool has_result(Op op) {
    return op == Op::Constant || op == Op::Add || op == Op::Sub ||
           op == Op::Mul || op == Op::Load;
}

}  // namespace optiforge
