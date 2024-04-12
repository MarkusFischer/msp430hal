#ifndef MSP430HAL_UTIL_MATH_H
#define MSP430HAL_UTIL_MATH_H

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

#endif //MSP430HAL_UTIL_MATH_H
