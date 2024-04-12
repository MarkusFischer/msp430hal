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
            /// \brief Return the right FSSEL configuration bits depending on the clock that will be used.
            ///
            /// \param clock Clock that will be used.
            /// \return FSSEL bits for clock.
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

        /// \brief The password that has to be set in the upper byte when writing to flash.
        const std::uint16_t flash_password = FWKEY;

        /// \brief Select the clock that will be used by the flash controller.
        ///
        /// \param clock The clock that will be used.
        inline void selectFlashClock(Clock clock)
        {
            FCTL2 = flash_password | clockFSSELValue(clock) | (FCTL2 & 0x3f);
        }

        /// \brief Select the clock that will be used by the flash controller. The clock speed will be divided by a pre scaler.
        ///
        /// \param clock The clock that will be used.
        /// \param divider The prescaler value resulting out of the lower 6 bits of divider + 1. The clock speed will be divided by this value.
        inline void selectFlashClock(Clock clock, std::uint8_t divider)
        {
            FCTL2 = flash_password | clockFSSELValue(clock) | (divider & 0x3f);
        }

        /// \brief set the prescaler for the flashcontroller.
        ///
        /// \param divider The prescaler value resulting out of the lower 6 bits of divider + 1. The clock speed will be divided by this value.
        inline void setFlashClockDivider(std::uint8_t divider)
        {
            FCTL2 = flash_password | (FCTL2 & 0x00c0) | (divider & 0x3f);
        }


        /// \brief Erase a whole segment at the address
        ///
        /// \param segment_address The address of the segment that will be erased.
        /// \param watchdog_active Is the watchdog active? If true, it will be stopped and (re)enabled afterwards.
        /// \return false if some error occurred, true otherwise
        inline bool eraseSegment(std::uint16_t segment_address, bool watchdog_active = false)
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

        /// \brief Write arbitrary data to the flash at a given address.
        ///
        /// \tparam T The type of the data which should be written to flash for size determination.
        /// \param address The address where the write operation should occure.
        /// \param data The data which should be written to flash.
        /// \param watchdog_active Is the watchdog active? If true, it will be stopped and (re)enabled afterwards.
        /// \return false if some error occurred, true otherwise
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

        /// \brief Write a byte to the flash memory.
        ///
        /// \param address The address where the write operation should occure.
        /// \param byte The byte which should be written to flash.
        /// \param watchdog_active Is the watchdog active? If true, it will be stopped and (re)enabled afterwards.
        /// \return false if some error occurred, true otherwise
        inline bool writeByte(std::uint16_t address, std::uint8_t byte, bool watchdog_active = false)
        {
            if (watchdog_active)
                timer::stopWatchdog();

            FCTL1 = flash_password | (FCTL1 & 0x0018) | WRT;
            FCTL3 = flash_password;

            *reinterpret_cast<std::uint8_t*>(address) = byte;

            bool error = FCTL3 & (ACCVIFG | FAIL);
            FCTL3 = flash_password | LOCK;
            FCTL1 = flash_password | (FCTL1 & 0x0018);

            if (watchdog_active)
                timer::startWatchdog();

            if (error)
                return false;
            return true;
        }

        /// \brief Write a word to the flash memory.
        ///
        /// \param address The address where the write operation should occure.
        /// \param word The word which should be written to flash.
        /// \param watchdog_active Is the watchdog active? If true, it will be stopped and (re)enabled afterwards.
        /// \return false if some error occurred, true otherwise
        inline bool writeWord(std::uint16_t address, std::uint16_t word, bool watchdog_active = false)
        {
            if (watchdog_active)
                timer::stopWatchdog();

            FCTL1 = flash_password | (FCTL1 & 0x0018) | WRT;
            FCTL3 = flash_password;

            *reinterpret_cast<std::uint16_t*>(address) = word;

            bool error = FCTL3 & (ACCVIFG | FAIL);
            FCTL3 = flash_password | LOCK;
            FCTL1 = flash_password | (FCTL1 & 0x0018);

            if (watchdog_active)
                timer::startWatchdog();

            if (error)
                return false;
            return true;
        }


    }
}

#endif //MSP430HAL_CPU_FLASH_CONTROLLER_H
