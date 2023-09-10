#ifndef MSP430HAL_GPIO_PIN_H
#define MSP430HAL_GPIO_PIN_H

#include <msp430.h>
#include <cstdint>
#include <type_traits>

namespace msp430hal
{
    namespace gpio
    {

        enum Pin : std::uint_fast8_t
        {
            p_0 = 0x01,
            p_1 = 0x02,
            p_2 = 0x04,
            p_3 = 0x08,
            p_4 = 0x10,
            p_5 = 0x20,
            p_6 = 0x40,
            p_7 = 0x80
        };

        enum Port : std::uint_fast8_t
        {
            port_1 = 0x0,
            port_2 = 0x1,
            port_3 = 0x2,
            port_4 = 0x3
        };

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

        namespace
        {
            static constexpr volatile std::uint8_t* gpio_registers[][9] = {
                    {&P1IN, &P1OUT, &P1DIR, &P1IFG, &P1IES, &P1IE, &P1SEL, &P1SEL2, &P1REN},
#ifdef __MSP430_HAS_PORT2_R__
                    {&P2IN, &P2OUT, &P2DIR, &P2IFG, &P2IES, &P2IE, &P2SEL, &P2SEL2, &P2REN},
#endif
#ifdef __MSP430_HAS_PORT3_R__
                    {&P3IN, &P3OUT, &P3DIR, nullptr, nullptr, nullptr, &P3SEL, &P3SEL2, &P3REN},
#endif
#ifdef __MSP430_HAS_PORT4_R__
                    {&P4IN, &P4OUT, &P4DIR, nullptr, nullptr, nullptr, &P4SEL, &P4SEL2, &P4REN},
#endif
            };

            template<Port port>
            constexpr volatile std::uint8_t* getGPIORegister(int reg_no)
            {
                return gpio_registers[port][reg_no];
            }
        }


        //TODO: GPIO Pin group over multiple ports
        template<Port port, std::uint8_t pins, Mode mode = Mode::output, PinResistors resistor = PinResistors::internal_pullup>
        struct GPIOPins
        {
            static constexpr volatile std::uint8_t* in = getGPIORegister<port>(0);
            static constexpr volatile std::uint8_t* out = getGPIORegister<port>(1);
            static constexpr volatile std::uint8_t* dir = getGPIORegister<port>(2);
            static constexpr volatile std::uint8_t* ifg = getGPIORegister<port>(3);
            static constexpr volatile std::uint8_t* ies = getGPIORegister<port>(4);
            static constexpr volatile std::uint8_t* ie = getGPIORegister<port>(5);
            static constexpr volatile std::uint8_t* sel = getGPIORegister<port>(6);
            static constexpr volatile std::uint8_t* sel2 = getGPIORegister<port>(7);
            static constexpr volatile std::uint8_t* ren = getGPIORegister<port>(8);

            static const Port port_value = port;
            static const std::uint8_t pins_value = pins;
            static const Mode mode_value = mode;
            static const PinResistors resistor_value = resistor;

            static inline void init()
            {
                // if constexpr would be nice; However, the initialization is usually called only once, hence we could rely on compiler optimization because the worst case has no big impact
                if (mode == Mode::output)
                    *dir |= pins;
                else
                {
                    *dir &= ~pins;
                    if (resistor == PinResistors::internal_pullup || resistor == PinResistors::internal_pulldown)
                    {
                        if (resistor == PinResistors::internal_pullup)
                            *out |= pins;
                        else
                            *out &= ~pins;
                        *ren |= pins;
                    }
                    else
                    {
                        if (resistor == PinResistors::external_pullup)
                            *out |= pins;
                        else
                            *out &= ~pins;
                        *ren &= ~pins;
                    }
                }
            }

            static inline void init(PinFunction function)
            {
                switch (function)
                {
                    case PinFunction::io:
                        *sel &= ~pins;
                        *sel2 &= ~pins;
                        init();
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

            static inline std::uint8_t inputLevel()
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

            static inline void toggle()
            {
                *out ^= pins;
            }

            static inline void set()
            {
                *out |= pins;
            }

            static inline void clear()
            {
                *out &= ~pins;
            }

            template<typename T = Port>
            static inline void enableInterrupt(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                *ie |= pins;
            }

            template<typename T = Port>
            static inline void disableInterrupt(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                if (port == Port::port_1 || port == Port::port_2)
                {
                    *ie &= ~pins;
                }
            }

            template<typename T = Port>
            static inline void setInterruptEdge(InterruptEdge edge, typename std::enable_if<(port <= 1), T>::type* = 0)
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
            static inline std::uint_fast8_t interruptFlag(typename std::enable_if<(port <= 1), T>::type* = 0)
            {
                if (port == Port::port_1 || port == Port::port_2)
                {
                    return *ifg & pins;
                }
                return 0;
            }
        };
    }
}

#endif //MSP430HAL_GPIO_PIN_H
