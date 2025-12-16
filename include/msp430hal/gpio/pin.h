#ifndef MSP430HAL_GPIO_PIN_H
#define MSP430HAL_GPIO_PIN_H

#include <msp430.h>
#include <cstdint>
#include <type_traits>

#include "_gpio_registers.h"
#include "../util/math.h"

namespace msp430hal
{
    namespace gpio
    {
        namespace
        {
            struct GPIOPins_base
            {};
        }

        enum class Mode
        {
            output,
            input
        };

        enum class PinResistors
        {
            internal_pullup,
            internal_pulldown,
            external_pullup,
            external_pulldown
        };

        enum class InterruptEdge
        {
            rising,
            falling
        };

        enum class PinFunction
        {
            io,
            primary_peripheral,
            device_specific,
            secondary_peripheral
        };

        //TODO: GPIO Pin group over multiple ports
        template<Port port, std::uint8_t pins, Mode mode = Mode::output, PinResistors resistor = PinResistors::internal_pullup>
        struct GPIOPins : GPIOPins_base
        {
            static constexpr volatile std::uint8_t* in = _gpio_registers::getGPIORegister(0, port);
            static constexpr volatile std::uint8_t* out = _gpio_registers::getGPIORegister(1, port);
            static constexpr volatile std::uint8_t* dir = _gpio_registers::getGPIORegister(2, port);
            static constexpr volatile std::uint8_t* ifg = _gpio_registers::getGPIORegister(3, port);
            static constexpr volatile std::uint8_t* ies = _gpio_registers::getGPIORegister(4, port);
            static constexpr volatile std::uint8_t* ie = _gpio_registers::getGPIORegister(5, port);
            static constexpr volatile std::uint8_t* sel = _gpio_registers::getGPIORegister(6, port);
            static constexpr volatile std::uint8_t* sel2 = _gpio_registers::getGPIORegister(7, port);
            static constexpr volatile std::uint8_t* ren = _gpio_registers::getGPIORegister(8, port);

            static const Port port_value = port;
            static const std::uint8_t pins_value = pins;
            static const Mode mode_value = mode;
            static const PinResistors resistor_value = resistor;

            static void init()
            {
                // if constexpr would be nice; However, the initialization is usually called only once, hence we could rely on compiler optimization because the worst case has no big impact
                if constexpr (mode == Mode::output)
                    *dir |= pins;
                else
                {
                    *dir &= ~pins;
                    if constexpr (resistor == PinResistors::internal_pullup || resistor == PinResistors::internal_pulldown)
                    {
                        if constexpr (resistor == PinResistors::internal_pullup)
                            *out |= pins;
                        else
                            *out &= ~pins;
                        *ren |= pins;
                    }
                    else
                    {
                        if constexpr (resistor == PinResistors::external_pullup)
                            *out |= pins;
                        else
                            *out &= ~pins;
                        *ren &= ~pins;
                    }
                }
            }

            static void init(PinFunction function)
            {
                init();
                switch (function)
                {
                    case PinFunction::io:
                        *sel &= ~pins;
                        *sel2 &= ~pins;
                        break;

                    case PinFunction::primary_peripheral:
                        *sel |= pins;
                        *sel2 &= ~pins;
                        break;

                    case PinFunction::device_specific:
                        *sel &= ~pins;
                        *sel2 |= pins;
                        break;

                    case PinFunction::secondary_peripheral:
                        *sel |= pins;
                        *sel2 |= pins;
                        break;
                }
            }

            static void switchFunction(PinFunction function)
            {
                switch (function)
                {
                    case PinFunction::io:
                        *sel &= ~pins;
                        *sel2 &= ~pins;
                        break;

                    case PinFunction::primary_peripheral:
                        *sel |= pins;
                        *sel2 &= ~pins;
                        break;

                    case PinFunction::device_specific:
                        *sel &= ~pins;
                        *sel2 |= pins;
                        break;

                    case PinFunction::secondary_peripheral:
                        *sel |= pins;
                        *sel2 |= pins;
                        break;
                }
            }

            static std::uint8_t inputLevel()
            {
                return *in & pins;
            }

            static bool input()
            {
                if ((*out & pins) > 0)
                    return (*in & pins) == 0;
                else
                    return (*in & pins) > 0;
            }

            static void toggle()
            {
                *out ^= pins;
            }

            static void set()
            {
                *out |= pins;
            }

            static void clear()
            {
                *out &= ~pins;
            }

            template<typename T = Port>
            static void enableInterrupt(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                *ie |= pins;
            }

            template<typename T = Port>
            static void disableInterrupt(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                if (port == Port::port_1 || port == Port::port_2)
                {
                    *ie &= ~pins;
                }
            }

            template<typename T = Port>
            static void setInterruptEdge(InterruptEdge edge, typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                if (port == Port::port_1 || port == Port::port_2)
                {
                    if (edge == InterruptEdge::rising)
                        *ies &= ~pins;
                    else
                        *ies |= pins;
                }
            }

            template<typename T = Port>
            static std::uint_fast8_t interruptFlag(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                if (port == Port::port_1 || port == Port::port_2)
                {
                    return *ifg & pins;
                }
                return 0;
            }

            template<typename T = Port>
            static std::uint_fast8_t clearInterruptFlag(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                if (port == Port::port_1 || port == Port::port_2)
                {
                    *ifg &= ~pins;
                }
                return 0;
            }

            template<typename T = Port>
            static std::uint_fast8_t setInterruptFlag(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                if (port == Port::port_1 || port == Port::port_2)
                {
                    *ifg |= pins;
                }
                return 0;
            }
        };

        template<Port port, std::uint8_t pin, Mode mode = Mode::output, PinResistors resistor = PinResistors::internal_pullup>
        struct GPIOPin : GPIOPins<port, pin, mode, resistor>
        {
            static_assert(is_power_of_two(pin), "Only a single pin can be specified");
        };

        template<typename T>
        struct is_GPIO_Pin
        {
            static constexpr bool value = std::is_base_of_v<GPIOPins_base, T>;
        };

        template<typename T>
        inline constexpr bool is_GPIO_Pin_v = is_GPIO_Pin<T>::value;
    }
}

#endif //MSP430HAL_GPIO_PIN_H
