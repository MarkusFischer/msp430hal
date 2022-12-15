//
// Created by markus on 12/15/22.
//
#ifndef MSP430HAL_TIMER_HWTIMER_H
#define MSP430HAL_TIMER_HWTIMER_H

#include <cstdint>
#include <type_traits>

namespace msp430hal::timer
{
    enum TimerClockSource : std::uint8_t
    {
        taclk = 0b00,
        aclk = 0b01,
        smclk = 0b10,
        inclk = 0b11
    };

    enum TimerClockInputDivider : std::uint8_t
    {
        times_1 = 0b00,
        times_2 = 0b01,
        times_4 = 0b10,
        times_8 = 0b11
    };

    enum TimerMode : std::uint8_t
    {
        stop = 0b00,
        up = 0b01,
        continuous = 0b10,
        up_down = 0b11
    };

    enum TimerCaptureMode : std::uint8_t
    {
        none = 0b00,
        rising_edge = 0b01,
        falling_edge = 0b10,
        edge = 0b11
    };

    enum CaptureCompareInputSelect : std::uint8_t
    {
        ccixa = 0b00,
        ccixb = 0b01,
        gnd = 0b10,
        vcc = 0b11
    };

    enum TimerOutputMode : std::uint8_t
    {
        output = 0b000,
        set = 0b001,
        toggle_reset = 0b010,
        set_reset = 0b011,
        toggle = 0b100,
        reset = 0b101,
        toggle_set = 0b110,
        reset_set = 0b111
    };

    enum class TimerModule
    {
        timer_a,
        timer_b
    };

    //
    namespace
    {
        constexpr volatile unsigned int* timer_a_registers[][17]{
#ifdef __MSP430_HAS_TA3__
                {&TA0CTL, &TA0R, &TA0CCTL0, &TA0CCR0, &TA0CCTL1, &TA0CCR1, &TA0CCTL2, &TA0CCR2, nullptr, nullptr,
                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &TA0IV},
#elif __MSP430_HAS_TA2__
                {&TA0CTL, &TA0R, &TA0CCTL0, &TA0CCR0, &TA0CCTL1, &TA0CCR1, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &TA0IV},
#endif
#ifdef __MSP430_HAS_T1A3__
                {&TA1CTL, &TA1R, &TA1CCTL0, &TA1CCR0, &TA1CCTL1, &TA1CCR1, &TA1CCTL2, &TA1CCR2, nullptr, nullptr,
                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &TA1IV},
#elif __MSP430_HAS_T1A2__
                {&TA1CTL, &TA1R, &TA1CCTL0, &TA1CCR0, &TA1CCTL1, &TA1CCR1, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &TA1IV},
#endif
#ifdef __MSP430_HAS_T2A3__
                {&TA2CTL, &TA2R, &TA2CCTL0, &TA2CCR0, &TA2CCTL1, &TA2CCR1, &TA2CCTL2, &TA2CCR2, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &TA2IV},
#elif __MSP430_HAS_T2A2__
                {&TA2CTL, &TA2R, &TA2CCTL0, &TA2CCR0, &TA2CCTL1, &TA2CCR1, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &TA2IV},
#endif
        };

#ifdef __MSP430_HAS_TB7__
        constexpr volatile unsigned int* timer_b_registers[][17]{
            {&TB0CTL, &TB0R, &TB0CCTL0, &TB0CCR0, &TB0CCTL1, &TB0CCR1, &TB0CCTL2, &TB0CCR2, &TB0CCTL3, &TB0CCR3, &TB0CCTL4, &TB0CCR4, &TB0CCTL5, &TB0CCR5, &TB0CCTL6, &TB0CCR6, &TB0IV},
        };
#else
        constexpr volatile unsigned int*** timer_b_registers = nullptr;
#endif

        template<const TimerModule module, const std::uint8_t instance>
        constexpr volatile unsigned int* getTimerRegister(std::uint8_t reg_no)
        {
            if constexpr(module == TimerModule::timer_a)
            {
                return timer_a_registers[instance][reg_no];
            }
            else
            {
                return timer_b_registers[instance][reg_no];
            }
        }
    }
    template<const TimerModule module,
            const int instance>
    struct Timer_t
    {
        static constexpr volatile unsigned int* ctl = getTimerRegister<module, instance>(0);
        static constexpr volatile unsigned int* counter = getTimerRegister<module, instance>(1);
        static constexpr volatile unsigned int* tiv = getTimerRegister<module, instance>(16);

        template<TimerModule tm, bool dummy>
        struct capture_control_registers {};

        template<bool dummy>
        struct capture_control_registers<TimerModule::timer_a, dummy>
        {
            static constexpr volatile unsigned int* data[3][2]{
                    {getTimerRegister<module, instance>(2), getTimerRegister<module, instance>(3)},
                    {getTimerRegister<module, instance>(4), getTimerRegister<module, instance>(5)},
                    {getTimerRegister<module, instance>(6), getTimerRegister<module, instance>(7)},
            };
        };

        template<bool dummy>
        struct capture_control_registers<TimerModule::timer_b, dummy>
        {
            static constexpr volatile unsigned int* data[3][2]{
                    {getTimerRegister<module, instance>(2),  getTimerRegister<module, instance>(3)},
                    {getTimerRegister<module, instance>(4),  getTimerRegister<module, instance>(5)},
                    {getTimerRegister<module, instance>(6),  getTimerRegister<module, instance>(7)},
                    {getTimerRegister<module, instance>(8),  getTimerRegister<module, instance>(9)},
                    {getTimerRegister<module, instance>(10), getTimerRegister<module, instance>(11)},
                    {getTimerRegister<module, instance>(12), getTimerRegister<module, instance>(13)},
                    {getTimerRegister<module, instance>(14), getTimerRegister<module, instance>(15)},
            };
        };

        static void reset()
        {
            __asm__ __volatile__ ( "BIS.W %[ucswrst], %[ucxctl1]"
            : [ucxctl1] "=m"(*ctl)
            : [ucswrst] "i"(TACLR)
            );
        }

        static bool isInterruptPending()
        {
            return (*ctl & TAIFG);
        }

        static void setMode(TimerMode mode)
        {
            *ctl = (mode << 4) | (*ctl & 0xffcf);
        }

        static void setInputDivider(TimerClockInputDivider divider)
        {
            *ctl = (divider << 6) | (*ctl & 0xff3f);
        }

        static void selectClockSource(TimerClockSource clock_source)
        {
            *ctl = (clock_source << 8) | (*ctl & 0xfcff);
        }

        static void init(TimerMode mode, TimerClockSource clock_source, TimerClockInputDivider divider)
        {
            *ctl = (mode << 4) | (divider << 6) | (clock_source << 8) | (*ctl & 0xfc07);
        }

        template<std::uint8_t capture_unit>
        static void setCaptureMode(TimerCaptureMode mode)
        {
            using cctl = capture_control_registers<module, true>;
            *cctl::data[capture_unit][0] = (mode << 14) | (*cctl::data[capture_unit][0] & 0x3fff);
        }

        template<std::uint8_t capture_unit>
        static void selectCaptureCompareInput(CaptureCompareInputSelect input)
        {
            using cctl = capture_control_registers<module, true>;
            *cctl::data[capture_unit][0] = (input << 12) | (*cctl::data[capture_unit][0] & 0xcfff);
        }

        template<std::uint8_t capture_unit>
        static void setOutputMode(TimerOutputMode mode)
        {
            using cctl = capture_control_registers<module, true>;
            *cctl::data[capture_unit][0] = (mode << 5) | (*cctl::data[capture_unit][0] & 0xff1f);
        }
    };
}

#endif //MSP430HAL_TIMER_HWTIMER_H
