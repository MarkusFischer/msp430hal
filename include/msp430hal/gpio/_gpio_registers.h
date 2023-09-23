#ifndef MSP430HAL_GPIO__GPIO_REGISTERS_H
#define MSP430HAL_GPIO__GPIO_REGISTERS_H

#include <msp430.h>
#include <cstdint>

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

        enum RegisterType : std::uint_fast8_t
        {
            in = 0,
            out = 1,
            dir = 2,
            ifg = 3,
            ies = 4,
            ie = 5,
            sel = 6,
            sel2 = 7,
            ren = 8
        };

        namespace _gpio_registers
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

            constexpr volatile std::uint8_t* getGPIORegister(int reg_no, Port port)
            {
                return gpio_registers[port][reg_no];
            }
        }
    }
}

#endif //MSP430HAL_GPIO__GPIO_REGISTERS_H
