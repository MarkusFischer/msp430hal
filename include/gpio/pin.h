//
// Created by markus on 2/7/23.
//
#ifndef MSP430HAL_GPIO_PIN_H
#define MSP430HAL_GPIO_PIN_H

#include <cstdint>

namespace msp430hal::gpio
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

    enum class Port
    {
        port_1,
        port_2,
        port_3,
        port_4
    };

    enum class Mode
    {
        input,
        output
    };

    namespace
    {
        static constexpr volatile uint8_t* gpio_registers[][9] = {
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
        constexpr volatile uint8_t* getGPIORegister(int reg_no)
        {
            if constexpr(port == Port::port_1)
                return gpio_registers[0][reg_no];
            else if constexpr (port == Port::port_2)
                return gpio_registers[1][reg_no];
            else if constexpr (port == Port::port_3)
                return gpio_registers[2][reg_no];
            else
                return gpio_registers[3][reg_no];
        }
    }


    template<Port port, Pin pin, Mode mode>
    class GPIOPin
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

    public:
        static inline void init()
        {
            if constexpr (mode == Mode::input)
            {

            }
            else
            {
                *dir |= pin;
            }
        }

        static inline void toggle()
        {
            *out ^= pin;
        }

        static inline void set()
        {
            *out |= pin;
        }

        static inline void clear()
        {
            *out &= ~pin;
        }
    };
}

#endif //MSP430HAL_GPIO_PIN_H
