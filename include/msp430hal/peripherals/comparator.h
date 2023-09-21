#ifndef MSP430HAL_PERIPHERALS_COMPERATOR_H
#define MSP430HAL_PERIPHERALS_COMPERATOR_H

#include <msp430.h>

namespace msp430hal
{
    namespace peripherals
    {
        enum ComparatorInput : std::uint_fast8_t
        {
            ca_0 = 0,
            ca_1,
            ca_2,
            ca_3,
            ca_4,
            ca_5,
            ca_6,
            ca_7,
            vcc_025,
            vcc_05,
            diode_reference,
            no_connection
        };

        class Comparator
        {
            ComparatorInput m_inverting_input;
            ComparatorInput m_non_inverting_input;
        public:

            void setInvertingInput(ComparatorInput input)
            {
                switch(input)
                {
                    case ComparatorInput::ca_0:
                        CACTL1 |= CAEX;
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

            void setNonInvertingInput(ComparatorInput input)
            {
                switch(input)
                {
                    case ComparatorInput::ca_0:
                        CACTL1 &= ~CAEX;
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

            void enable()
            {
                CACTL1 |= CAON;
            }

            void disable()
            {
                CACTL1 &= ~CAON;
            }

            void toggleShortInputs()
            {
                CACTL2 ^= CASHORT;
            }

            void exchangeInputs()
            {
                CACTL1 ^= CAEX;
            }

            void interruptOnRisingEdge()
            {
                CACTL1 &= ~CAIES;
            }

            void interruptOnFallingEdge()
            {
                CACTL1 |= CAIES;
            }

            void enableInterrupt()
            {
                CACTL1 |= CAIE;
            }

            void disableInterrupt()
            {
                CACTL1 &= ~CAIE;
            }

            bool isInterruptPending()
            {
                return CACTL1 & CAIFG;
            }

            void enableOutputFilter()
            {
                CACTL2 |= CAF;
            }

            void disableOutputFilter()
            {
                CACTL2 &= ~CAF;
            }

            bool getOutput()
            {
                return CACTL2 & CAOUT;
            }
        };
    }
}

#endif //MSP430HAL_PERIPHERALS_COMPERATOR_H
