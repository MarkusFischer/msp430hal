#ifndef MSP430HAL_TIMER_WATCHDOG_TIMER_H
#define MSP430HAL_TIMER_WATCHDOG_TIMER_H

#include <msp430.h>
#include "../cpu/clock_module.h"

namespace msp430hal
{
    namespace timer
    {
        /// \brief Specifies the watchdog interval by a appropiate pre divider.
        enum WatchdogDivider
        {
            times_32768 = 0b00, ///< Watchdog interval is 32768 cycles.
            times_8192 = 0b01, ///< Watchdog interval is 8192 cycles.
            times_512 = 0b10, ///< Watchdog interval is 512 cycles.
            times_64 = 0b11 ///< Watchdog interval is 64 cycles.
        };

        /// \brief Stop the watchdog timer.
        inline void stopWatchdog()
        {
            WDTCTL = WDTPW | ((WDTCTL & 0xff) | WDTHOLD);
        }

        /// \brief Start the watchdog timer.
        inline void startWatchdog()
        {
            WDTCTL = WDTPW | ((WDTCTL & ~WDTHOLD) & 0xff);
        }

        /// \brief Triggers a NMI interrupt at a rising edge at the NMI/Reset pin.
        inline void setWatchdogNMIEdgeRising()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xbf);
        }

        /// \brief Triggers a NMI interrupt at a falling edge at the NMI/Reset pin.
        inline void setWatchdogNMIEdgeFalling()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xff) | WDTNMIES;
        }

        /// \brief Configure the NMI/Reset pin in reset mode.
        ///
        /// A low signal at the reset pin will trigger a power on reset.
        inline void setWatchdogNMIPinReset()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xdf);
        }
        /// \brief Configure the NMI/Reset pin in NMI (non maskable interrupt) mode.
        inline void setWatchdogNMIPinNMI()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xff) | WDTNMI;
        }

        /// \brief Configure the Watchdog timer in watchdog mode.
        ///
        /// The user has to calm the watchdog regularly to prevent a PUC reset.
        inline void setWatchdogModeWatchdog()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xef);
        }

        /// \brief Configures the watchdog timer in interval mode.
        ///
        /// This mode can be used to provide periodic interrupts.
        /// In interval timer mode, the WDTIFG flag is set at the expiration of the selected time interval.
        inline void setWatchdogModeInterval()
        {
            WDTCTL = WDTPW | (WDTCTL & 0xff) | WDTTMSEL;
        }

        /// \brief Calm the watchdog by setting the WDTCNTCL flag.
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

        /// \brief Selects the clock that will be used by the watchdog timer.
        ///
        /// \param clock The clock used by the watchdog timer.
        inline void selectWatchdogClock(cpu::Clock clock)
        {
            if (clock == cpu::Clock::smclk)
                WDTCTL = WDTPW | ((WDTCTL & ~WDTSSEL) & 0xff);
            else if (clock == cpu::Clock::aclk)
                WDTCTL = WDTPW | ((WDTCTL & 0xff) | WDTSSEL);
        }

        /// \brief Selectst the watchdog clock divider and thus the interval in which the watchdog has to be calmed.
        ///
        /// \param divider The pre-divider/time interval for the watchdog timer.
        inline void selectWatchdogDivider(WatchdogDivider divider)
        {
            WDTCTL = WDTPW | ((WDTCTL & 0xfc) | divider);
        }

        // TODO watchdog interrupts
    }
}

#endif //MSP430HAL_TIMER_WATCHDOG_TIMER_H
