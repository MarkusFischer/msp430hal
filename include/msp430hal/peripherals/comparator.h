#ifndef MSP430HAL_PERIPHERALS_COMPERATOR_H
#define MSP430HAL_PERIPHERALS_COMPERATOR_H

#include <msp430.h>

namespace msp430hal
{
    namespace peripherals
    {
        /// \brief Specifies all possible (external and internal) input sources for the Comparator B Peripheral
        enum ComparatorInput : std::uint_fast8_t
        {
            ca_0 = 0, ///< Pin CA0
            ca_1, ///< Pin CA1
            ca_2, ///< Pin CA2
            ca_3, ///< Pin CA3
            ca_4, ///< Pin CA4
            ca_5, ///< Pin CA5
            ca_6, ///< Pin CA6
            ca_7, ///< Pin CA7
            vcc_025, ///< internal 0.25 * VCC reference
            vcc_05, ///< internal 0.5 * VCC reference
            diode_reference, ///< internal 0.7 V reference (diode)
            no_connection ///< no connection at all
        };

        /// \brief This class represents the Comparator B Peripheral.
        class Comparator
        {
            ComparatorInput m_inverting_input;
            ComparatorInput m_non_inverting_input;
        public:

            /// \brief Set the input source of the inverting input of the comparator.
            ///
            /// Set the input source of the inverting input to the specified input. All possible internal or external sources can be used.
            /// When the internal voltage reference gets used or was used before, this method enables/disables the reference voltage generator automatically.
            /// Additionally, this method ensures, that both the inverting and the non-inverting input are configured valid e.g. by swapping the inputs if necessary.
            /// This includes that inputs will be disabled if the input sources for the inverting and non-inverting input are on the same multiplexer.
            ///
            /// \param input The input source for the inverting input.
            void setInvertingInput(ComparatorInput input)
            {
                switch(input)
                {
                    case ComparatorInput::ca_0:
                        CACTL1 |= CAEX;
                        if (m_non_inverting_input >= ComparatorInput::vcc_025 && m_non_inverting_input <= ComparatorInput::diode_reference)
                            CACTL1 &= ~CARSEL;
                        CACTL2 &= 0xbb;
                        CACTL2 |= 0x04;
                        //Make sure non-inverting input gets signal from second multiplexer
                        if (m_non_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= 0x08;
                        }
                        else if (m_non_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= 0x10;
                        }
                        else if (m_non_inverting_input == ComparatorInput::ca_0)
                        {
                            CACTL2 &= 0xc7;
                            m_non_inverting_input = ComparatorInput::no_connection;
                        }
                        break;
                    case ComparatorInput::ca_1:
                    case ComparatorInput::ca_2:
                        if (CACTL1 & CAEX)
                        {
                            //CAEX set -> inverting input behind first multiplexer
                            CACTL2 &= 0xbb;
                            if (input == ComparatorInput::ca_1)
                                CACTL2 |= 0x40;
                            else
                                CACTL2 |= 0x44;
                        }
                        else
                        {
                            //CAEX not set -> inverting input behind second multiplexer
                            CACTL2 &= 0xc7;
                            CACTL2 |= input << 3;
                        }
                        break;
                    case ComparatorInput::ca_3:
                    case ComparatorInput::ca_4:
                    case ComparatorInput::ca_5:
                    case ComparatorInput::ca_6:
                    case ComparatorInput::ca_7:
                        CACTL1 &= ~CAEX;
                        if (m_non_inverting_input >= ComparatorInput::vcc_025 && m_non_inverting_input <= ComparatorInput::diode_reference)
                            CACTL1 |= CARSEL;
                        //TODO take care if reference voltage is applied to other input
                        CACTL2 &= 0xc7;
                        CACTL2 |= input << 3;
                        //Make sure non-inverting input gets input from first multiplexer
                        if (m_non_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0xbb;
                            CACTL2 |= 0x40;
                        }
                        else if (m_non_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0xbb;
                            CACTL2 |= 0x44;
                        }
                        else if (m_non_inverting_input >= ComparatorInput::ca_3 && m_inverting_input <= ComparatorInput::ca_7)
                        {
                            CACTL2 &= 0xbb;
                            m_non_inverting_input = ComparatorInput::no_connection;
                        }
                        break;
                    case ComparatorInput::vcc_025:
                        if (CACTL1 & CAEX)
                            CACTL2 &= 0xbb;
                        else
                            CACTL2 &= 0xc7;
                        CACTL1 &= 0xcf;
                        CACTL1 |= (~(CACTL1 >> 1) & 0x40) | 0x10;
                        break;
                    case ComparatorInput::vcc_05:
                        if (CACTL1 & CAEX)
                            CACTL2 &= 0xbb;
                        else
                            CACTL2 &= 0xc7;
                        CACTL1 &= 0xcf;
                        CACTL1 |= (~(CACTL1 >> 1) & 0x40) | 0x20;
                        break;
                    case ComparatorInput::diode_reference:
                        if (CACTL1 & CAEX)
                            CACTL2 &= 0xbb;
                        else
                            CACTL2 &= 0xc7;
                        CACTL1 &= 0xcf;
                        CACTL1 |= (~(CACTL1 >> 1) & 0x40) | 0x30;
                        break;
                    case ComparatorInput::no_connection:
                        if (CACTL1 & CAEX)
                            CACTL2 &= 0xbb;
                        else
                            CACTL2 &= 0xc7;
                        break;
                }
                //Disable internal reference if not needed
                if (!(m_inverting_input >= ComparatorInput::vcc_025 && m_inverting_input <= ComparatorInput::diode_reference) &&
                    !(input >= ComparatorInput::vcc_025 && input <= ComparatorInput::diode_reference))
                    CACTL1 &= 0xcf;
                m_inverting_input = input;
            }

            /// \brief Set the input source of the non-inverting input of the comparator.
            ///
            /// Set the input source of the non-inverting input to the specified input. All possible internal or external sources can be used.
            /// When the internal voltage reference gets used or was used before, this method enables/disables the reference voltage generator automatically.
            /// Additionally, this method ensures, that both the inverting and the non-inverting input are configured valid e.g. by swapping the inputs if necessary.
            /// This includes that inputs will be disabled if the input sources for the inverting and non-inverting input are on the same multiplexer.
            ///
            /// \param input The input source for the non-inverting input.
            void setNonInvertingInput(ComparatorInput input)
            {
                switch(input)
                {
                    case ComparatorInput::ca_0:
                        CACTL1 &= ~CAEX;
                        if (m_inverting_input >= ComparatorInput::vcc_025 && m_inverting_input <= ComparatorInput::diode_reference)
                            CACTL1 |= CARSEL;
                        CACTL2 &= 0xbb;
                        CACTL2 |= 0x04;
                        //Make sure inverting input gets signal from second multiplexer
                        if (m_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= 0x08;
                        }
                        else if (m_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= 0x10;
                        }
                        else if (m_inverting_input == ComparatorInput::ca_0)
                        {
                            //Disable other input
                            CACTL2 &= 0xc7;
                            m_inverting_input = ComparatorInput::no_connection;
                        }
                        break;
                    case ComparatorInput::ca_1:
                    case ComparatorInput::ca_2:
                        if (CACTL1 & CAEX)
                        {
                            //CAEX set -> non-inverting input behind second multiplexer
                            CACTL2 &= 0xc7;
                            CACTL2 |= input << 3;
                        }
                        else
                        {
                            //CAEX not set -> non-inverting input behind first multiplexer
                            CACTL2 &= 0xbb;
                            if (input == ComparatorInput::ca_1)
                                CACTL2 |= 0x40;
                            else
                                CACTL2 |= 0x44;
                        }
                        break;
                    case ComparatorInput::ca_3:
                    case ComparatorInput::ca_4:
                    case ComparatorInput::ca_5:
                    case ComparatorInput::ca_6:
                    case ComparatorInput::ca_7:
                        CACTL1 |= CAEX;
                        if (m_inverting_input >= ComparatorInput::vcc_025 && m_inverting_input <= ComparatorInput::diode_reference)
                            CACTL1 &= ~CARSEL;
                        CACTL2 &= 0xc7;
                        CACTL2 |= input << 3;
                        //Make sure inverting input gets input from first multiplexer
                        if (m_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0xbb;
                            CACTL2 |= 0x40;
                        }
                        else if (m_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0xbb;
                            CACTL2 |= 0x44;
                        }
                        else if (m_inverting_input >= ComparatorInput::ca_3 && m_inverting_input <= ComparatorInput::ca_7)
                        {
                            CACTL2 &= 0xbb;
                            m_inverting_input = no_connection;
                        }
                        break;
                    case ComparatorInput::vcc_025:
                        if (CACTL2 & CAEX)
                            CACTL2 &= 0xc7;
                        else
                            CACTL2 &= 0xbb;
                        CACTL1 &= 0xcf;
                        CACTL1 |= ((CACTL1 >> 1) & 0x40) | 0x10;
                        break;
                    case ComparatorInput::vcc_05:
                        if (CACTL2 & CAEX)
                            CACTL2 &= 0xc7;
                        else
                            CACTL2 &= 0xbb;
                        CACTL1 &= 0xcf;
                        CACTL1 |= ((CACTL1 >> 1) & 0x40) | 0x20;
                        break;
                    case ComparatorInput::diode_reference:
                        if (CACTL2 & CAEX)
                            CACTL2 &= 0xc7;
                        else
                            CACTL2 &= 0xbb;
                        CACTL1 &= 0xcf;
                        CACTL1 |= ((CACTL1 >> 1) & 0x40) | 0x30;
                        break;
                    case ComparatorInput::no_connection:
                        if (CACTL1 & CAEX)
                            CACTL2 &= 0xc7;
                        else
                            CACTL2 &= 0xbb;
                        break;
                }
                //Disabl internal reference if not needed
                if (!(m_non_inverting_input >= ComparatorInput::vcc_025 && m_non_inverting_input <= ComparatorInput::diode_reference) &&
                    !(input >= ComparatorInput::vcc_025 && input <= ComparatorInput::diode_reference))
                    CACTL1 &= 0xcf;
                m_non_inverting_input = input;
            }

            /// \brief Enable the comparator peripheral.
            void enable()
            {
                CACTL1 |= CAON;
            }

            /// \brief Disable the comparator peripheral.
            void disable()
            {
                CACTL1 &= ~CAON;
            }

            /// \brief Toggle if the inputs are shorted or not.
            void toggleShortInputs()
            {
                CACTL2 ^= CASHORT;
            }

            /// \brief Exchange the input sources of the inverting and the non-inverting input
            void exchangeInputs()
            {
                CACTL1 ^= CAEX;
            }

            /// \brief Enables, that an interrupt is thrown (or the corresponding status bit is set) when the output of the comparator transition from low to high.
            void interruptOnRisingEdge()
            {
                CACTL1 &= ~CAIES;
            }

            /// \brief Enables, that an interrupt is thrown (or the corresponding status bit is set) when the output of the comparator transition from high to low.
            void interruptOnFallingEdge()
            {
                CACTL1 |= CAIES;
            }

            /// \brief Enable that the comparator peripherals throw interrupts.
            void enableInterrupt()
            {
                CACTL1 |= CAIE;
            }

            /// \brief Disable interrupts from the comparator peripheral.
            void disableInterrupt()
            {
                CACTL1 &= ~CAIE;
            }

            bool isInterruptPending()
            {
                return CACTL1 & CAIFG;
            }

            /// \brief Enable the internal low-pass filter on the output.
            void enableOutputFilter()
            {
                CACTL2 |= CAF;
            }

            /// \brief Disable the internal low-pass filter on the output.
            void disableOutputFilter()
            {
                CACTL2 &= ~CAF;
            }

            /// \brief get the current output state of the comparator
            ///
            /// \return true if the non-inverting input has a higher voltage level than the inverting input. Otherwise false.
            bool getOutput()
            {
                return CACTL2 & CAOUT;
            }
        };
    }
}

#endif //MSP430HAL_PERIPHERALS_COMPERATOR_H
