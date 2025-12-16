#ifndef MSP430HAL_UTIL_MATH_H
#define MSP430HAL_UTIL_MATH_H

#include <type_traits>

/// \brief A constexpr utility function that rounds floating points to the nearest integer
///
/// \tparam int_type a integral type
/// \tparam floating_type a floating point type
/// \param value the value to round
/// \return value rounded to the nearest integer
template<typename int_type, typename floating_type>
constexpr int_type round_t(floating_type value)
{
    return (value >= 0.0) ? static_cast<int_type>(value + 0.5) : static_cast<int_type>(value - 0.5);
}

template<typename int_type>
constexpr bool is_power_of_two(int_type value, std::enable_if<std::is_integral_v<int_type>>* = nullptr)
{
    return (value & (value - 1)) == 0;
}
#endif //MSP430HAL_UTIL_MATH_H
