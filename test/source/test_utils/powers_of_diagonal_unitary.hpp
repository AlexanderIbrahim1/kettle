#pragma once

#include <sstream>

/*
    This file contains the streams need to build the gates corresponding
    to various powers of a 4x4 diagonal unitary matrix.
*/

auto get_gate_pow_1_stream() -> std::stringstream;
auto get_gate_pow_2_stream() -> std::stringstream;
auto get_gate_pow_4_stream() -> std::stringstream;
auto get_gate_pow_8_stream() -> std::stringstream;
auto get_gate_pow_16_stream() -> std::stringstream;
auto get_gate_pow_32_stream() -> std::stringstream;
