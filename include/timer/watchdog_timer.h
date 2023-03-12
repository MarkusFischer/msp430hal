//
// Created by markus on 3/10/23.
//

#ifndef MSP430HAL_TIMER_WATCHDOG_TIMER_H
#define MSP430HAL_TIMER_WATCHDOG_TIMER_H

#include "../cpu/clock_module.h"

namespace msp430hal
{
    namespace timer
    {
        enum WatchdogDivider
        {
            times_32768 = 0b00,
            times_8192 = 0b01,
            times_512 = 0b10,
            times_64 = 0b11
        };

        inline void stopWatchdog()
        {
            WDTCTL = WDTPW | ((WDTCTL & 0xff) | WDTHOLD);
        }

        inline void startWatchdog()
        {
            WDTCTL = WDTPW | ((WDTCTL & ~WDTHOLD) & 0xff);
        }

        inline void setWatchdogNMIEdgeRising()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xbf);
        }

        inline void setWatchdogNMIEdgeFalling()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xff) | WDTNMIES;
        }

        inline void setWatchdogNMIPinReset()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xdf);
        }

        inline void setWatchdogNMIPinNMI()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xff) | WDTNMI;
        }

        inline void setWatchdogModeWatchdog()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xef);
        }

        inline void setWatchdogModeInterval()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xff) | WDTTMSEL;
        }

        inline void calmWatchdog()
        {
            WDTCTL = WDTPW | ((WDTCTL & 0xff) | WDTCNTCL);
        }

#ifndef MSP430HAL_NO_EASTER_EGGS
        // Easter egg:
        inline void throwBone()
        {
            calmWatchdog();
        }
#endif

        inline void selectWatchdogClock(cpu::Clock clock)
        {
            if (clock == cpu::Clock::smclk)
                WDTCTL = WDTPW | ((WDTCTL & ~WDTSSEL) & 0xff);
            else if (clock == cpu::Clock::aclk)
                WDTCTL = WDTPW | ((WDTCTL & 0xff) | WDTSSEL);
        }

        inline void selectWatchdogDivider(WatchdogDivider divider)
        {
            WDTCTL = WDTPW | ((WDTCTL & 0xfc) | divider);
        }

    }
}

#endif //MSP430HAL_TIMER_WATCHDOG_TIMER_H
