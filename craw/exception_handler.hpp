#pragma once

#include <map>
#include "debug_registers.hpp"

void set_own_context(debug_register_vector const & data);
bool install_exception_handler();
