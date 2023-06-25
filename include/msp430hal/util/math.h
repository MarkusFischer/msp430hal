#ifndef MSP430HAL_UTIL_MATH_H
#define MSP430HAL_UTIL_MATH_H

template<typename int_type, typename floating_type>
constexpr int_type round_t(floating_type value)
{
    return (value >= 0.0) ? static_cast<int_type>(value + 0.5) : static_cast<int_type>(value - 0.5);
}

#endif //MSP430HAL_UTIL_MATH_H
