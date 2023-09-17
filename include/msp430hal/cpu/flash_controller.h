#ifndef MSP430HAL_CPU_FLASH_CONTROLLER_H
#define MSP430HAL_CPU_FLASH_CONTROLLER_H

#include <msp430.h>
#include <cstdint>
#include "clock_module.h"
#include "../timer/watchdog_timer.h"

namespace msp430hal
{
    namespace cpu
    {
        namespace
        {
            inline constexpr std::uint16_t clockFSSELValue(Clock clock)
            {
                if (clock == Clock::aclk)
                    return 0;
                else if (clock == Clock::mclk)
                    return 0x0040;
                else
                    return 0x0080;
            }
        }


        const std::uint16_t flash_password = FWKEY;

        inline void selectFlashClock(Clock clock)
        {
            FCTL2 = flash_password | clockFSSELValue(clock) | (FCTL2 & 0x3f);
        }

        inline void selectFlashClock(Clock clock, std::uint8_t divider)
        {
            FCTL2 = flash_password | clockFSSELValue(clock) | (divider & 0x3f);
        }

        inline void setFlashClockDivider(std::uint8_t divider)
        {
            FCTL2 = flash_password | (FCTL2 & 0x00c0) | (divider & 0x3f);
        }


        bool eraseSegment(std::uint16_t segment_address, bool watchdog_active = false)
        {
            if (watchdog_active)
                timer::stopWatchdog();

            // Setup flash controller
            FCTL1 = flash_password | (FCTL1 & 0x0018) | ERASE;
            //Clear Lock Bit (INFO A is not effected and flags are cleared)
            FCTL3 = flash_password;

            //Dummy write
            *reinterpret_cast<std::uint16_t*>(segment_address) = 0;

            bool error = FCTL3 & (ACCVIFG | FAIL);
            //Set Lock Bit
            FCTL3 = flash_password | LOCK;
            FCTL1 = flash_password | (FCTL1 & 0x0018);

            if (watchdog_active)
                timer::startWatchdog();

            if (error)
                return false;

            return true;
        }

        template<typename T>
        bool write(std::uint16_t address, T data, bool watchdog_active = false)
        {
            if (watchdog_active)
                timer::stopWatchdog();

            FCTL1 = flash_password | (FCTL1 & 0x0018) | WRT;
            FCTL3 = flash_password;

            *reinterpret_cast<T*>(address) = data;

            bool error = FCTL3 & (ACCVIFG | FAIL);
            FCTL3 = flash_password | LOCK;
            FCTL1 = flash_password | (FCTL1 & 0x0018);

            if (watchdog_active)
                timer::startWatchdog();

            if (error)
                return false;
            return true;
        }

        inline bool writeByte(std::uint16_t address, std::uint8_t byte, bool watchdog_active = false)
        {
            return write<std::uint8_t>(address, byte, watchdog_active);
        }

        inline bool writeWord(std::uint16_t address, std::uint16_t word, bool watchdog_active = false)
        {
            return write<std::uint16_t>(address, word, watchdog_active);
        }


    }
}

#endif //MSP430HAL_CPU_FLASH_CONTROLLER_H
