#ifndef MSP430HAL_PERIPHERALS_COMPERATOR_H
#define MSP430HAL_PERIPHERALS_COMPERATOR_H

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
                //Prevent that the same input will be applied and make sure, the input lies on different multiplexers
                if (m_non_inverting_input != no_connection && (m_non_inverting_input == input ||
                    (m_non_inverting_input >= ComparatorInput::vcc_025 && input >= ComparatorInput::vcc_025) ||
                    (m_non_inverting_input >= ComparatorInput::ca_3 && m_non_inverting_input <= ComparatorInput::ca_7
                     && input >= ComparatorInput::ca_3 && input <= ComparatorInput::ca_7))) //TODO: exceptions?
                    return;

                switch(input)
                {
                    case ComparatorInput::ca_0:
                        CACTL1 |= CAEX;
                        if (m_non_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x08;
                        }
                        else if (m_non_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x10;
                        }
                        else
                            CACTL2 &= 0xbb;
                        break;
                    case ComparatorInput::ca_1:
                    case ComparatorInput::ca_2:
                        if (m_non_inverting_input == ComparatorInput::ca_0 || !(CACTL1 & CAEX))
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= input << 3;
                        }
                        else if (m_non_inverting_input >= ComparatorInput::ca_3 || CACTL1 & CAEX)
                        {
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
                        CACTL1 &= ~CAEX;
                        if (m_non_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x40 | (input << 3);
                        }
                        else if (m_non_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x44 | (input << 3);
                        }
                        else
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= input << 3;
                        }
                        break;
                    case ComparatorInput::vcc_025:
                        CACTL1 &= 0xcf;
                        CACTL1 |= (~(CACTL1 >> 1) & 0x40) | 0x10;
                        break;
                    case ComparatorInput::vcc_05:
                        CACTL1 &= 0xcf;
                        CACTL1 |= (~(CACTL1 >> 1) & 0x40) | 0x20;
                        break;
                    case ComparatorInput::diode_reference:
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
                if ((m_inverting_input >= ComparatorInput::vcc_025 && m_inverting_input <= ComparatorInput::diode_reference) &&
                    !(input >= ComparatorInput::vcc_025 && input <= ComparatorInput::diode_reference))
                    CACTL1 &= 0xcf;
                m_inverting_input = input;
            }

            void setNonInvertingInput(ComparatorInput input)
            {
                //Prevent that the same input will be applied and make sure, the input lies on different multiplexers
                if (m_inverting_input != no_connection && (m_inverting_input == input ||
                                                            (m_inverting_input >= ComparatorInput::vcc_025 && input >= ComparatorInput::vcc_025) ||
                                                            (m_inverting_input >= ComparatorInput::ca_3 && m_inverting_input <= ComparatorInput::ca_7
                                                                && input >= ComparatorInput::ca_3 && input <= ComparatorInput::ca_7))) //TODO: exceptions?
                    return;

                switch(input)
                {
                    case ComparatorInput::ca_0:
                        CACTL1 &= ~CAEX;
                        if (m_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x08;
                        }
                        else if (m_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x10;
                        }
                        else
                            CACTL2 &= 0xbb;
                        break;
                    case ComparatorInput::ca_1:
                    case ComparatorInput::ca_2:
                        if (m_inverting_input == ComparatorInput::ca_0 || (CACTL1 & CAEX))
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= input << 3;
                        }
                        else if (m_non_inverting_input >= ComparatorInput::ca_3 || !(CACTL1 & CAEX))
                        {
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
                        if (m_inverting_input == ComparatorInput::ca_1)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x40 | (input << 3);
                        }
                        else if (m_inverting_input == ComparatorInput::ca_2)
                        {
                            CACTL2 &= 0x83;
                            CACTL2 |= 0x44 | (input << 3);
                        }
                        else
                        {
                            CACTL2 &= 0xc7;
                            CACTL2 |= input << 3;
                        }
                        break;
                    case ComparatorInput::vcc_025:
                        CACTL1 &= 0xcf;
                        CACTL1 |= ((CACTL1 >> 1) & 0x40) | 0x10;
                        break;
                    case ComparatorInput::vcc_05:
                        CACTL1 &= 0xcf;
                        CACTL1 |= ((CACTL1 >> 1) & 0x40) | 0x20;
                        break;
                    case ComparatorInput::diode_reference:
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
                if ((m_non_inverting_input >= ComparatorInput::vcc_025 && m_non_inverting_input <= ComparatorInput::diode_reference) &&
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
