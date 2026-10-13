#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace optiforge {

using ValueId = std::uint32_t;
constexpr ValueId InvalidValue = 0;

enum class Op { Constant, Add, Sub, Mul, Load, Store, Return };

struct Instruction {
    ValueId result{InvalidValue};
    Op op{Op::Constant};
    ValueId lhs{InvalidValue};
    ValueId rhs{InvalidValue};
    std::int64_t immediate{0};
    std::string name;
    bool live{true};
};

class Function {
public:
    ValueId constant(std::int64_t value);
    ValueId add(ValueId lhs, ValueId rhs, std::string name = {});
    ValueId sub(ValueId lhs, ValueId rhs, std::string name = {});
    ValueId mul(ValueId lhs, ValueId rhs, std::string name = {});
    ValueId load(std::string name);
    void store(ValueId value, std::string name);
    void ret(ValueId value);

    const std::vector<Instruction>& instructions() const { return instructions_; }
    std::vector<Instruction>& instructions() { return instructions_; }
    const Instruction* find(ValueId id) const;
    Instruction* find(ValueId id);
    std::string dump() const;

private:
    ValueId next_id_{1};
    std::vector<Instruction> instructions_;
};

const char* op_name(Op op);
bool has_result(Op op);

}  // namespace optiforge
