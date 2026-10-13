#pragma once

#include "optiforge/ir.h"

#include <string>

namespace optiforge {

std::string emit_riscv(const Function& function);

}  // namespace optiforge
