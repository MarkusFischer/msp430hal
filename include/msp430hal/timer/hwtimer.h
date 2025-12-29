#ifndef MSP430HAL_TIMER_HWTIMER_H
#define MSP430HAL_TIMER_HWTIMER_H

#include <msp430.h>
#include <cstdint>
#include <type_traits>

namespace msp430hal
{
    namespace timer
    {
        /// \brief Specifies the available clock sources for the timer.
        enum TimerClockSource : std::uint16_t
        {
            txclk = TASSEL_0, ///< External timer X clock.
            aclk = TASSEL_1, ///< Internal auxillary clock.
            smclk = TASSEL_2, ///< Internal sub-main clock.
            inclk = TASSEL_3 ///< External device-specific clock source. Usually TBCLK.
        };

        /// \brief Specifies the input divider factor of the timer clock.
        enum TimerClockInputDivider : std::uint16_t
        {
            times_1 = ID_0, ///< Divide by 1.
            times_2 = ID_1, ///< Divide by 2.
            times_4 = ID_2, ///< Divide by 4.
            times_8 = ID_3 ///< Divide by 8.
        };

        /// \brief Specifies the working mode of the timer.
        enum TimerMode : std::uint16_t
        {
            stop = MC_0, ///< Timer stopped.
            up = MC_1, ///< Timer counts up til the content of TxCCR0/TxCL0
            continuous = MC_2, ///< Timer counts up to the maximum possible value.
            up_down = MC_3 ///< Timer count up to the value of TxCCR0/TxCL0 and then down to 0.
        };

        /// \brief Specifies the mode of the capture module.
        enum TimerCaptureMode : std::uint16_t
        {
            none = CM_0, ///< Disable capture mode.
            rising_edge = CM_1, ///< Capture at a rising edge.
            falling_edge = CM_2, ///< Capture at a falling edge.
            edge = CM_3 ///< Capture at any edge.
        };

        /// \brief Selects the input signal for the capture unit.
        enum CaptureCompareInputSelect : std::uint16_t
        {
            ccixa = CCIS_0, ///< CCIxA
            ccixb = CCIS_1, ///< CCIxB
            gnd = CCIS_2, ///< GND (for software initiated captures)
            vcc = CCIS_3 ///< VCC (for software initiated captures)
        };

        /// \brief specifies the timer compare output mode.
        enum TimerOutputMode : std::uint16_t
        {
            output = OUTMOD_0, ///< The output is determined by the value of the OUT bit.
            set = OUTMOD_1, ///< The output is set when the timer counts to the TACCRx value. It remains set until a reset of the timer, or until another output mode is selected and affects the output.
            toggle_reset = OUTMOD_2, ///< The output is toggled when the timer counts to the TACCRx value. It is reset when the timer counts to the TACCR0 value.
            set_reset = OUTMOD_3, ///< The output is set when the timer counts to the TACCRx value. It is reset when the timer counts to the TACCR0 value.
            toggle = OUTMOD_4, ///< The output is toggled when the timer counts to the TACCRx value. The output period is double the timer period.
            reset = OUTMOD_5, ///< The output is reset when the timer counts to the TACCRx value. It remains reset until another output mode is selected and affects the output.
            toggle_set = OUTMOD_6, ///< The output is toggled when the timer counts to the TACCRx value. It is set when the timer counts to the TACCR0 value.
            reset_set = OUTMOD_7 ///< The output is reset when the timer counts to the TACCRx value. It is set when the timer counts to the TACCR0 value.
        };

        /// \brief Specifies the Timer modules.
        enum TimerModule
        {
            timer_a, ///< Use Timer A.
            timer_b ///< Use Timer B.
        };

        //
        namespace
        {

            //Arrays with the configuration registers for each available timer and template functions to access them easily.
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

            template<const TimerModule timer_module, const std::uint_fast8_t instance>
            constexpr volatile unsigned int* getTimerRegister(std::uint_fast8_t reg_no)
            {
                return (timer_module == TimerModule::timer_a) ? timer_a_registers[instance][reg_no] : timer_b_registers[instance][reg_no];
            }
#endif

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

        }

        /// \brief This class gives a unified interface for the different timer modules.
        ///
        /// \tparam module The timer module type that will be used.
        /// \tparam instance The actual timer instance of that timer module.
        template<const TimerModule module,
                const int instance>
        struct Timer_t
        {
            static constexpr volatile unsigned int* ctl = getTimerRegister<module, instance>(0);
            static constexpr volatile unsigned int* counter = getTimerRegister<module, instance>(1);
            static constexpr volatile unsigned int* tiv = getTimerRegister<module, instance>(16);

            using capture_control_registers = CaptureControlRegisters<module, instance>;


            /// \brief Reset the timer.
            ///
            /// Clears the TAR register, sets the clock divider to 1 and stops the timer.
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

            /// \brief Checks if a timer overflow interrupt is pending.
            ///
            /// \return The value of the timer overflow flag TAIFG.
            static bool isInterruptPending()
            {
                return (*ctl & TAIFG);
            }

            /// \brief Enables the timer overflow interrupt.
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

            /// \brief Sets working mode (count direction) for the timer.
            ///
            /// \param mode The working mode (count direction) which will be used.
            static void setMode(TimerMode mode)
            {
                *ctl &= 0xffcf;
                *ctl |= mode;
            }

            /// \brief Sets the input divider for the timer clock source.
            ///
            /// \param divider The input dividier used by the timer.
            static void setInputDivider(TimerClockInputDivider divider)
            {
                *ctl &= 0xff3f;
                *ctl |= divider;
            }

            /// \brief Selects the clock that will be used by the timer.
            ///
            /// \param clock_source The clock used by the timer.
            static void selectClockSource(TimerClockSource clock_source)
            {
                *ctl &= 0xfcff;
                *ctl |= clock_source;
            }

            /// \brief Initialize the timer by selecting the mode, clock source and the input divider.
            ///
            /// \param mode The working mode (count direction) which will be used.
            /// \param clock_source The clock used by the timer.
            /// \param divider The input dividier used by the timer.
            static void init(TimerMode mode, TimerClockSource clock_source, TimerClockInputDivider divider)
            {
                *ctl &= 0xfc07;
                *ctl = mode + divider + clock_source;
            }

            /// \brief Configures the mode (on which edge should be captured?) for a specific capture unit.
            ///
            /// \tparam capture_unit The capture unit that will be configured.
            /// \param mode The mode for the capture unit.
            template<std::uint_fast8_t capture_unit>
            static void setCaptureMode(TimerCaptureMode mode)
            {
                *capture_control_registers::data[capture_unit][0] &= 0x3fff;
                *capture_control_registers::data[capture_unit][0] |= mode;
            }

            /// \brief Configures the input for a specific capture unit.
            ///
            /// On which input should the capture unit wait for captures (changing edges)
            ///
            /// \tparam capture_unit The capture unit that will be configured.
            /// \param input The capture/compare input to be used.
            template<std::uint_fast8_t capture_unit>
            static void selectCaptureCompareInput(CaptureCompareInputSelect input)
            {
                *capture_control_registers::data[capture_unit][0] &= 0xcfff;
                *capture_control_registers::data[capture_unit][0] |= input;
            }

            /// \brief Enables synchronous captures.
            ///
            /// The current timer value is copied and interrupt flag after a capture are set the next timer clock cycle:
            ///
            /// \tparam capture_unit The capture that will be configured.
            template<std::uint_fast8_t capture_unit>
            static void synchronousCapture()
            {
                *capture_control_registers::data[capture_unit][0] |= 0x0800;
            }

            /// \brief Enables asynchronous captures.
            ///
            /// The current timer value is copied and interrupt flag after a capture are set independent of the timer clock cycle:
            ///
            /// \tparam capture_unit The capture that will be configured.
            template<std::uint_fast8_t capture_unit>
            static void asynchronousCapture()
            {
                *capture_control_registers::data[capture_unit][0] &= 0xf7ff;
            }

            /// \brief Enable compare mode.
            ///
            /// This mode can be used to generate PWM signals or interrupt at specific time intervals.
            /// When the timer counts to the value of TACCRx the following happens:
            /// Interrupt flag CCIFG is set
            // Internal signal EQUx = 1 is set, which affects the output according to the output mode
            // The input signal CCI is latched into SCCI
            ///
            /// \tparam capture_unit The capture unit that will be configured.
            template<std::uint_fast8_t capture_unit>
            static void compareMode()
            {
                *capture_control_registers::data[capture_unit][0] |= 0xfeff;
            }

            /// \brief Enable capture mode.
            ///
            /// When a capture (changing edge) occurs the following happens:
            /// The current timer value is copied into the TACCRx register
            /// The interrupt flag CCIFG is set
            //
            /// \tparam capture_unit The capture unit that will be configured.
            template<std::uint_fast8_t capture_unit>
            static void captureMode()
            {
                *capture_control_registers::data[capture_unit][0] |= 0x0100;
            }

            /// \brief Configures the output mode for the time configured in compare mode.
            ///
            /// The modes are  the following:
            /// output The output is determined by the value of the OUT bit.
            /// Set The output is set when the timer counts to the TACCRx value. It remains set until a reset of the timer, or until another output mode is selected and affects the output.
            /// Toggle/Reset The output is toggled when the timer counts to the TACCRx value. It is reset when the timer counts to the TACCR0 value.
            /// Set/Reset The output is set when the timer counts to the TACCRx value. It is reset when the timer counts to the TACCR0 value.
            /// Toggle The output is toggled when the timer counts to the TACCRx value. The output period is double the timer period.
            /// Reset The output is reset when the timer counts to the TACCRx value. It remains reset until another output mode is selected and affects the output.
            /// Toggle/Set The output is toggled when the timer counts to the TACCRx value. It is set when the timer counts to the TACCR0 value.
            /// Reset/Set The output is reset when the timer counts to the TACCRx value. It is set when the timer counts to the TACCR/ va34e,
            ///
            /// \tparam capture_unit The capture unit for which the output behaviour should be configured.
            /// \param mode The mode used by the capture unit.
            template<std::uint_fast8_t capture_unit>
            static void setOutputMode(TimerOutputMode mode)
            {
                *capture_control_registers::data[capture_unit][0] &= 0xff1f;
                *capture_control_registers::data[capture_unit][0] |= mode;
            }

            /// \brief Writes the value in the TACCRx register.
            ///
            /// \tparam capture_unit The capture unit for which the TACCRx register should be modified.
            /// \param compare The value written to the the TACCRx register.
            template<std::uint_fast8_t capture_unit>
            static void setCompareValue(std::uint16_t compare)
            {
                *capture_control_registers::data[capture_unit][1] = compare;
            }

            /// \brief Reads the value of the TACCRx register.
            ///
            /// \tparam capture_unit The capture unit for which the TACCRx register should be read.
            /// \return The value of the TACCRx register.
            template<std::uint_fast8_t capture_unit>
            static std::uint16_t getCaptureValue()
            {
                return *capture_control_registers::data[capture_unit][1];
            }

            /// \brief Enables that a interrupt is thrown when a capture or compare event occurs.
            ///
            /// \tparam capture_unit The capture unit for which the interrupt should be thrown.
            template<std::uint_fast8_t capture_unit>
            static void enableCaptureCompareInterrupt()
            {
                *capture_control_registers::data[capture_unit][0] |= 0x0010;
            }


            /// \brief Disable that a interrupt is thrown when a capture or compare event occurs.
            ///
            /// \tparam capture_unit The capture unit for which the interrupt should be thrown.
            template<std::uint_fast8_t capture_unit>
            static void disableCaptureCompareInterrupt()
            {
                *capture_control_registers::data[capture_unit][0] &= 0xffef;
            }

            /// \brief Reads the capture/compare interrupt flag CCIFG.
            ///
            /// \tparam capture_unit The capture/compare unit for which the CCIFG flag should be read.
            /// \return True if CCIFG is set, false otherwise.
            template<std::uint_fast8_t capture_unit>
            static bool isCaptureCompareInterruptPending()
            {
                return *capture_control_registers::data[capture_unit][0] & 0x0001;
            }

            /// \brief For compare output mode 0 (Output) this sets the output to 1
            ///
            /// \tparam capture_unit The capture unit for which the out bit should be modified.
            template<std::uint_fast8_t capture_unit>
            static void setOutput()
            {
                *capture_control_registers::data[capture_unit][0] |= 0x0004;
            }

            /// \brief For compare output mode 0 (Output) this sets the output to 0
            ///
            /// \tparam capture_unit The capture unit for which the out bit should be modified.
            template<std::uint_fast8_t capture_unit>
            static void clearOutput()
            {
                *capture_control_registers::data[capture_unit][0] &= 0xfffb;
            }

            /// \brief Check if the capture overflow flag is set (COV).
            ///
            /// Returns the value of the COV flag. This flag is set when a second capture happened before the value of the first was read.
            ///
            /// \tparam capture_unit The capture unit for which the COV flag should be checked.
            /// \return The value of the COV flag for capture unit.
            template<std::uint_fast8_t capture_unit>
            static bool captureOverflowOccurred()
            {
                return *capture_control_registers::data[capture_unit][0] & 0x0002;
            }

            /// \brief Clear the capture overflow flag (COV)
            ///
            /// \tparam capture_unit the capture unit for which the COV flag should be cleared.
            template<std::uint_fast8_t capture_unit>
            static void clearCaptureOverflowFlag()
            {
                *capture_control_registers::data[capture_unit][0] &= 0xfffd;
            }

        };
    }
}

#endif //MSP430HAL_TIMER_HWTIMER_H
