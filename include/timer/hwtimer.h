//
// Created by markus on 12/15/22.
//
#ifndef MSP430HAL_TIMER_HWTIMER_H
#define MSP430HAL_TIMER_HWTIMER_H

#include <cstdint>
#include <type_traits>

namespace msp430hal
{
    namespace timer
    {
        enum TimerClockSource : std::uint16_t
        {
            taclk = TASSEL_0,
            aclk = TASSEL_1,
            smclk = TASSEL_2,
            inclk = TASSEL_3
        };

        enum TimerClockInputDivider : std::uint16_t
        {
            times_1 = ID_0,
            times_2 = ID_1,
            times_4 = ID_2,
            times_8 = ID_3
        };

        enum TimerMode : std::uint16_t
        {
            stop = MC_0,
            up = MC_1,
            continuous = MC_2,
            up_down = MC_3
        };

        enum TimerCaptureMode : std::uint16_t
        {
            none = CM_0,
            rising_edge = CM_1,
            falling_edge = CM_2,
            edge = CM_3
        };

        enum CaptureCompareInputSelect : std::uint16_t
        {
            ccixa = CCIS_0,
            ccixb = CCIS_1,
            gnd = CCIS_2,
            vcc = CCIS_3
        };

        enum TimerOutputMode : std::uint16_t
        {
            output = OUTMOD_0,
            set = OUTMOD_1,
            toggle_reset = OUTMOD_2,
            set_reset = OUTMOD_3,
            toggle = OUTMOD_4,
            reset = OUTMOD_5,
            toggle_set = OUTMOD_6,
            reset_set = OUTMOD_7
        };

        enum TimerModule
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


#if __cplusplus >= 201703L
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
#else

            template<const TimerModule timer_module, const std::uint8_t instance>
            constexpr volatile unsigned int* getTimerRegister(std::uint8_t reg_no)
            {
                return (timer_module == TimerModule::timer_a) ? timer_a_registers[instance][reg_no] : timer_b_registers[instance][reg_no];
            }

            template<const TimerModule module, const std::uint_fast8_t instance>
            struct CaptureControlRegisters
            {
                static constexpr volatile unsigned int* data = nullptr;
            };

            template<const std::uint_fast8_t instance>
            struct CaptureControlRegisters<TimerModule::timer_a, instance>
            {
                static constexpr volatile unsigned int* data[3][2] = {
                        {getTimerRegister<TimerModule::timer_a, instance>(2), getTimerRegister<TimerModule::timer_a, instance>(3)},
                        {getTimerRegister<TimerModule::timer_a, instance>(4), getTimerRegister<TimerModule::timer_a, instance>(5)},
                        {getTimerRegister<TimerModule::timer_a, instance>(6), getTimerRegister<TimerModule::timer_a, instance>(7)},
                };
            };

            template<const std::uint_fast8_t instance>
            constexpr volatile unsigned int* CaptureControlRegisters<TimerModule::timer_a, instance>::data[3][2];

            template<const std::uint_fast8_t instance>
            struct CaptureControlRegisters<TimerModule::timer_b, instance>
            {
                static constexpr volatile unsigned int* data[7][2] = {
                        {getTimerRegister<TimerModule::timer_b, instance>(2), getTimerRegister<TimerModule::timer_b, instance>(3)},
                        {getTimerRegister<TimerModule::timer_b, instance>(4), getTimerRegister<TimerModule::timer_b, instance>(5)},
                        {getTimerRegister<TimerModule::timer_b, instance>(6), getTimerRegister<TimerModule::timer_b, instance>(7)},
                        {getTimerRegister<TimerModule::timer_b, instance>(8), getTimerRegister<TimerModule::timer_b, instance>(9)},
                        {getTimerRegister<TimerModule::timer_b, instance>(10), getTimerRegister<TimerModule::timer_b, instance>(11)},
                        {getTimerRegister<TimerModule::timer_b, instance>(12), getTimerRegister<TimerModule::timer_b, instance>(13)},
                        {getTimerRegister<TimerModule::timer_b, instance>(14), getTimerRegister<TimerModule::timer_b, instance>(15)},
                };
            };

            template<const std::uint_fast8_t instance>
            constexpr volatile unsigned int* CaptureControlRegisters<TimerModule::timer_b, instance>::data[7][2];


#endif
        }
        template<const TimerModule module,
                const int instance>
        struct Timer_t
        {
            static constexpr volatile unsigned int* ctl = getTimerRegister<module, instance>(0);
            static constexpr volatile unsigned int* counter = getTimerRegister<module, instance>(1);
            static constexpr volatile unsigned int* tiv = getTimerRegister<module, instance>(16);

            using capture_control_registers = CaptureControlRegisters<module, instance>;


            static void reset()
            {
#ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.W %[taclr], %[tctl]"
                        : [tctl] "=m"(*ctl)
                : [taclr] "i"(TACLR)
                );
#else
                *ctl |= TACLR;
#endif
            }

            static bool isInterruptPending()
            {
                return (*ctl & TAIFG);
            }

            static void enableInterrupt()
            {
#ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.W %[tie], %[tctl]"
                        : [tctl] "=m"(*ctl)
                : [tie] "i"(TAIE)
                );
#else
                *ctl |= TAIE;
#endif
            }

            static void setMode(TimerMode mode)
            {
                *ctl &= 0xffcf;
                *ctl |= mode;
            }

            static void setInputDivider(TimerClockInputDivider divider)
            {
                *ctl &= 0xff3f;
                *ctl |= divider;
            }

            static void selectClockSource(TimerClockSource clock_source)
            {
                *ctl &= 0xfcff;
                *ctl |= clock_source;
            }

            static void init(TimerMode mode, TimerClockSource clock_source, TimerClockInputDivider divider)
            {
                *ctl &= 0xfc07;
                *ctl = mode + divider + clock_source;
            }

            template<std::uint8_t capture_unit>
            static void setCaptureMode(TimerCaptureMode mode)
            {
                *capture_control_registers::data[capture_unit][0] &= 0x3fff;
                *capture_control_registers::data[capture_unit][0] |= mode;
            }

            template<std::uint8_t capture_unit>
            static void selectCaptureCompareInput(CaptureCompareInputSelect input)
            {
                *capture_control_registers::data[capture_unit][0] &= 0xcfff;
                *capture_control_registers::data[capture_unit][0] |= input;
            }

            template<std::uint8_t capture_unit>
            static void setOutputMode(TimerOutputMode mode)
            {
                *capture_control_registers::data[capture_unit][0] &= 0xff1f;
                *capture_control_registers::data[capture_unit][0] |= mode;
            }
        };
    }
}

#endif //MSP430HAL_TIMER_HWTIMER_H
