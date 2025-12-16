#ifndef MSP430HAL_USCI_SPINTERFACE_H
#define MSP430HAL_USCI_SPINTERFACE_H

#include <msp430.h>
#include <cstdint>
#include "usci.h"

namespace msp430hal::usci
{
    enum SPIMode : std::uint8_t
    {
        master_3pin = 0x08,
        slave_3pin = 0x00,
        master_4pin_active_low = 0x0c,
        master_4pin_active_high = 0x0a,
        slave_4pin_active_low = 0x04,
        slave_4pin_active_high = 0x02
    };

    enum SPICharacterFormat : std::uint8_t
    {
        lsb_8bit = 0x00,
        lsb_7bit = 0x10,
        msb_8bit = 0x20,
        msb_7bit = 0x30
    };

    enum SPIClockMode : std::uint8_t
    {
        active_high_leading_edge = 0x00,
        active_high_trailing_edge = 0x80,
        active_low_leading_edge = 0x40,
        active_low_trailing_edge = 0xc0
    };

    template<UsciModule module,
            std::uint_fast8_t instance,
            SPIMode mode = master_3pin,
            SPICharacterFormat format = msb_8bit,
            SPIClockMode clock_mode = active_high_leading_edge,
            UsciClockSource clock_source = uclk,
            std::uint16_t pre_scale_factor = 1>
    struct SPI_t
    {
        using Usci = Usci_t<module, instance>;

        static constexpr UsciModule module_value = module;
        static constexpr std::uint_fast8_t instance_value = instance;
        static constexpr SPIMode mode_value = mode;
        static constexpr SPICharacterFormat format_value = format;
        static constexpr SPIClockMode clock_mode_value = clock_mode;
        static constexpr UsciClockSource clock_source_value = clock_source;
        static constexpr std::uint16_t pre_scale_factor_value = pre_scale_factor;

        static void init()
        {
            Usci::disableModule();

            *Usci::ctl_0 = clock_mode | format | mode | 0x01;
            *Usci::ctl_1 |= clock_source;
            *Usci::br_0 = (std::uint8_t) (0x00ff & pre_scale_factor);
            *Usci::br_1 = (std::uint8_t) ((0xff00 & pre_scale_factor) >> 8);
            // TODO configure ports

            Usci::enableModule();
        }

        static void enableLoopback()
        {
            *Usci::stat |= UCLISTEN;
        }

        static void disableLoopback()
        {
            *Usci::stat &= ~UCLISTEN;
        }

        static bool loopback()
        {
            return *Usci::stat & UCLISTEN;
        }

        static bool readFramingError()
        {
            return *Usci::stat & UCFE;
        }

        static bool framingError()
        {
            bool error = *Usci::stat & UCFE;
            *Usci::stat &= ~UCFE;
            return error;
        }

        static bool readOverrunError()
        {
            return *Usci::stat & UCOE;
        }

        static bool overrunError()
        {
            return readOverrunError();
        }

        static void enable()
        {
            Usci::enableModule();
        }

        static void disable()
        {
            Usci::disableModule();
        }
    };
}


#endif //MSP430HAL_USCI_SPINTERFACE_H
