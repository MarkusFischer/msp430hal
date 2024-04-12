#ifndef MSP430HAL_CPU_CLOCK_MODULE_H
#define MSP430HAL_CPU_CLOCK_MODULE_H

#include <msp430.h>

namespace msp430hal
{
    namespace cpu
    {
        /// \brief This enumeration encodes the available clocks that can be configured with functions in this namespace.
        enum Clock
        {
            mclk, ///< main clock
            smclk, ///< sub-main clock
            aclk ///< auxillary clock
        };

        /// \brief Encodes the possible sources for the clock.
        enum ClockSource
        {
            dcoclk, ///< Internal digitally controlled oscillator (DCO).
            xt2clk, ///< Optional high-frequency oscillator
            xt1clk ///< external low-frequency/high-frequency oscillator or external clock
        };

        /// \brief Specifies the pre-scaler used by the clock
        enum Divider
        {
            times_1 = 0b00, ///< divide speed of clock source by 1
            times_2 = 0b01, ///< divide speed of clock source by 2
            times_4 = 0b10, ///< divide speed of clock source by 4
            times_8 = 0b11 ///< divide speed of clock source by 8
        };

        /// \brief Encodes the frequency range of a external HF clock source.
        enum HighFrequencyRange
        {
            range_0MHz4_to_1MHz = 0b00, ///< 400 kHz - 1 MHz crystal or resonator
            range_1MHz_to_3MHz = 0b01, ///< 1 MHz - 3 MHz crystal or resonator
            range_3MHz_to_16MHZ = 0b10, ///< 3 MHz - 16 MHz crystal or resonator
            range_external = 0b11 ///< Digital external clock source 400 kHz - 16 MHz
        };

        /// \brief Encodes the external low frequency clock source.
        enum LowFrequencySource
        {
            watch_crystal = 0, ///< external 32768 Hz crystal
            vloclk = 0x20, ///< internal very low-power low-frequency oscillator
            external_clock = 0x30 ///< digital external clock source
        };

        /// \brief Encodeds which internal oscillator load capacitors can be used together with a external low frequency crystal
        enum OscillatorCapacitor
        {
            approx_1pF = 0, ///< approximately 1 pF
            approx_6pF = 0x04, ///< approximately 6 pF
            approx_10pF = 0x08, ///< approximately 10 pF
            approx_12pF = 0x0c ///< approximately 12 pF
        };

        /// \brief Basic template for clock source selection
        ///
        /// \tparam clock the clock for which the clock source should be selected
        /// \param source the clock source that should be used by clock
        template<const Clock clock>
        inline void selectClockSource(ClockSource source)
        {}

        /// \brief template specialization for clock source selection of the MCLK
        ///
        /// \param source the clock source that should be used by the MCLK
        template<>
        inline void selectClockSource<Clock::mclk>(ClockSource source)
        {
            if (source == ClockSource::dcoclk)
                BCSCTL2 &= 0x3f;
            else if (source == ClockSource::xt2clk)
            {
                BCSCTL2 &= 0x3f;
                BCSCTL2 |= 0x40;
            }
            else
            {
                BCSCTL2 &= 0x3f;
                BCSCTL2 |= 0xc0;
            }
        }

        /// \brief template specialization for clock source selection of the SMCLK
        ///
        /// \param source the clock source that should be used by the SMCLK
        template<>
        inline void selectClockSource<Clock::smclk>(ClockSource source)
        {
            if (source == ClockSource::dcoclk)
                BCSCTL2 &= 0xf7;
            else
                BCSCTL2 |= 0x08;
        }

        /// \brief Deactivate the optional XT2 oscillator.
        inline void turnXT2Off()
        {
            BCSCTL1 |= 0x80;
        }

        /// \brief Activate the optional XT2 oscillator.
        inline void turnXT2On()
        {
            BCSCTL1 &= 0x7f;
        }

        /// \brief Template dummy to select the frequency range for a given high frequency clock.
        ///
        /// \tparam source the clock source for which the frequency range should be specified
        /// \param range the frequency range of the clock source
        template<const ClockSource source>
        inline void setFrequencyRange(HighFrequencyRange range)
        {}

        /// \brief Template specialization for the XT2CLK to select the frequency range.
        ///
        /// \param range The frequency range of XT2CLK.
        template<>
        inline void setFrequencyRange<ClockSource::xt2clk>(HighFrequencyRange range)
        {
            BCSCTL3 &= 0x3f;
            BCSCTL3 |= range << 6;
        }

        /// \brief Template specialization for the XT1CLK to select the frequency range.
        ///
        /// \param range The frequency range of XT1CLK.
        template<>
        inline void setFrequencyRange<ClockSource::xt1clk>(HighFrequencyRange range)
        {
            BCSCTL1 |= 0x40;
            BCSCTL3 &= 0xcf;
            BCSCTL3 |= range << 4;
        }

        /// \brief select the clock source for the low frequency range.
        ///
        /// \param source The low frequency clock source.
        inline void setLowFrequencySource(LowFrequencySource source)
        {
            BCSCTL1 &= 0xbf;
            BCSCTL3 &= 0xcf;
            BCSCTL3 |= source;
        }

        /// \brief Select which internal load capacitors should be used with the low frequency clock.
        ///
        /// \param capacitor the capacitor value to be used
        inline void selectOscillatorCapacitor(OscillatorCapacitor capacitor)
        {
            BCSCTL3 &= 0xf3;
            BCSCTL3 |= capacitor;
        }

        /// \brief Template dummy to select the pre-scaler value for the clock.
        ///
        /// \tparam clock the clock for which the prescaler should be modified.
        /// \param divider Specifies the prescaler value.
        template<const Clock clock>
        inline void setInputDivider(Divider divider)
        {}

        /// \brief Template specialization to select prescaler for the MCLK.
        ///
        /// \param divider Prescaler value used by MCLK
        template<>
        inline void setInputDivider<Clock::mclk>(Divider divider)
        {
            BCSCTL2 &= 0xcf;
            BCSCTL2 |= divider << 4;
        }

        /// \brief Template specialization to select prescaler for the SMCLK.
        ///
        /// \param divider Prescaler value used by SMCLK
        template<>
        inline void setInputDivider<Clock::smclk>(Divider divider)
        {
            BCSCTL2 &= 0xf9;
            BCSCTL2 |= divider << 1;
        }

        /// \brief Template specialization to select prescaler for the ACLK.
        ///
        /// \param divider Prescaler value used by ACLK
        template<>
        inline void setInputDivider<Clock::aclk>(Divider divider)
        {
            BCSCTL1 &= 0xcf;
            BCSCTL1 |= divider << 4;
        }

#ifndef __DisableCalData
        /// \brief possible calibrated frequency values that can be used by the internal DCO.
        enum CalibratedDCOFrequency
        {
            calibrated_1MHz, ///< 1 MHz
            calibrated_8MHz, ///< 8 MHz
            calibrated_12MHz, ///< 12 MHz
            calibrated_16MHz ///< 16 MHz
        };

        /// \brief Template dummy to set the calibrated frequency used by the internal DCO.
        template<CalibratedDCOFrequency frequency>
        inline void setCalibratedFrequency()
        {}

        /// \brief Set the Frequency used by the internal DCO to 1 MHz.
        template<>
        inline void setCalibratedFrequency<calibrated_1MHz>()
        {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_1MHZ;
            DCOCTL = CALDCO_1MHZ;
        }

        /// \brief Set the Frequency used by the internal DCO to 8 MHz.
        template<>
        inline void setCalibratedFrequency<calibrated_8MHz>()
        {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_8MHZ;
            DCOCTL = CALDCO_8MHZ;
        }

        /// \brief Set the Frequency used by the internal DCO to 12 MHz.
        template<>
        inline void setCalibratedFrequency<calibrated_12MHz>()
        {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_12MHZ;
            DCOCTL = CALDCO_12MHZ;
        }

        /// \brief Set the Frequency used by the internal DCO to 16 MHz.
        template<>
        inline void setCalibratedFrequency<calibrated_16MHz>()
        {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_16MHZ;
            DCOCTL = CALDCO_16MHZ;
        }
#endif

    }
}

#endif //MSP430HAL_CPU_CLOCK_MODULE_H
