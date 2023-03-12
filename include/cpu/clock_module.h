//
// Created by markus on 3/9/23.
//

#ifndef MSP430HAL_CPU_CLOCK_MODULE_H
#define MSP430HAL_CPU_CLOCK_MODULE_H

namespace msp430hal
{
    namespace cpu
    {
        enum Clock
        {
            mclk,
            smclk,
            aclk
        };

        enum ClockSource
        {
            dcoclk,
            xt2clk,
            xt1clk
        };

        enum Divider
        {
            times_1 = 0b00,
            times_2 = 0b01,
            times_4 = 0b10,
            times_8 = 0b11
        };

        enum HighFrequencyRange
        {
            range_0MHz4_to_1MHz = 0b00,
            range_1MHz_to_3MHz = 0b01,
            range_3MHz_to_16MHZ = 0b10,
            range_external = 0b11
        };

        enum LowFrequencySource
        {
            watch_crystal = 0,
            vloclk = 0x20,
            external_clock = 0x30
        };

        enum OscillatorCapacitor
        {
            approx_1pF = 0,
            approx_6pF = 0x04,
            approx_10pF = 0x08,
            approx_12pF5 = 0x0c
        };

        template<const Clock clock>
        inline void selectClockSource(ClockSource source)
        {}

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

        template<>
        inline void selectClockSource<Clock::smclk>(ClockSource source)
        {
            if (source == ClockSource::dcoclk)
                BCSCTL2 &= 0xf7;
            else
                BCSCTL2 |= 0x08;
        }

        inline void turnXT2Off()
        {
            BCSCTL1 |= 0x80;
        }

        inline void turnXT2On()
        {
            BCSCTL1 &= 0x7f;
        }

        template<const ClockSource source>
        inline void setFrequencyRange(HighFrequencyRange range)
        {}

        template<>
        inline void setFrequencyRange<ClockSource::xt2clk>(HighFrequencyRange range)
        {
            BCSCTL3 &= 0x3f;
            BCSCTL3 |= range << 6;
        }

        template<>
        inline void setFrequencyRange<ClockSource::xt1clk>(HighFrequencyRange range)
        {
            BCSCTL1 |= 0x40;
            BCSCTL3 &= 0xcf;
            BCSCTL3 |= range << 4;
        }

        inline void setLowFrequencySource(LowFrequencySource source)
        {
            BCSCTL1 &= 0xbf;
            BCSCTL3 &= 0xcf;
            BCSCTL3 |= source;
        }

        inline void selectOscillatorCapacitor(OscillatorCapacitor capacitor)
        {
            BCSCTL3 &= 0xf3;
            BCSCTL3 |= capacitor;
        }

        template<const Clock clock>
        inline void setInputDivider(Divider divider)
        {}

        template<>
        inline void setInputDivider<Clock::mclk>(Divider divider)
        {
            BCSCTL2 &= 0xcf;
            BCSCTL2 |= divider << 4;
        }

        template<>
        inline void setInputDivider<Clock::smclk>(Divider divider)
        {
            BCSCTL2 &= 0xf9;
            BCSCTL2 |= divider << 1;
        }

        template<>
        inline void setInputDivider<Clock::aclk>(Divider divider)
        {
            BCSCTL1 &= 0xcf;
            BCSCTL1 |= divider << 4;
        }

#ifndef __DisableCalData
        enum CalibratedDCOFrequency
        {
            calibrated_1MHz,
            calibrated_8MHz,
            calibrated_12MHz,
            calibrated_16MHz
        };

        template<CalibratedDCOFrequency frequency>
        inline void setCalibratedFrequency()
        {}

        template<>
        inline void setCalibratedFrequency<calibrated_1MHz>()
        {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_1MHZ;
            DCOCTL = CALDCO_1MHZ;
        }

        template<>
        inline void setCalibratedFrequency<calibrated_8MHz>()
        {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_8MHZ;
            DCOCTL = CALDCO_8MHZ;
        }

        template<>
        inline void setCalibratedFrequency<calibrated_12MHz>()
        {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_12MHZ;
            DCOCTL = CALDCO_12MHZ;
        }

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
