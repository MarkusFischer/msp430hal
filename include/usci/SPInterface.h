//
// Created by markus on 12/11/22.
//
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

    enum SPIClockSource : std::uint8_t
    {
        aclk = 0x40,
        smclk = 0x80
    };

    template<const UsciModule module,
            const int instance,
            typename data_type = std::uint8_t,
            SPIMode mode = master_3pin,
            SPICharacterFormat format = msb_8bit,
            SPIClockMode clock_mode = active_high_leading_edge,
            SPIClockSource clock_source = smclk,
            std::uint16_t pre_scale_factor = 1>
    struct BlockingSpi
    {
        using Usci = Usci_t<module, instance>;

        void init()
        {
            Usci::disableModule();

            *Usci::ctl_0 = clock_mode | format | mode | 0x01;
            *Usci::ctl_1 |= clock_source;
            *Usci::br_0 = (std::uint8_t) (0x00ff & pre_scale_factor);
            *Usci::br_1 = (std::uint8_t) ((0xff00 & pre_scale_factor) >> 8);

            Usci::enableModule();
        }

        void sendByte(std::uint8_t byte)
        {
            while (!Usci::isTxInterruptPending());
            *Usci::tx_buf = byte;
        }

        std::uint8_t readByte()
        {
            while (!Usci::isRxInterruptPending());
            return *Usci::rx_buf;
        }

        void enable()
        {
            Usci::enableModule();
        }

        void disable()
        {
            Usci::disableModule();
        }
    };

    template<typename data_type,
            volatile std::uint8_t* port_sel_0,
            volatile std::uint8_t* port_sel_1,
            volatile std::uint8_t* usci_ctl_0,
            volatile std::uint8_t* usci_ctl_1,
            volatile std::uint8_t* usci_br_0,
            volatile std::uint8_t* usci_br_1,
            volatile std::uint8_t* usci_stat,
            volatile std::uint8_t* read_buf,
            volatile std::uint8_t* write_buf,
            volatile std::uint8_t* interrupt_flag_register,
            std::uint8_t mosi_miso_pin_bitmask,
            std::uint8_t rx_interrupt_bit,
            std::uint8_t tx_interrupt_bit,
            SPIMode mode = master_3pin,
            SPICharacterFormat format = msb_8bit,
            SPIClockMode clock_mode = active_high_leading_edge,
            SPIClockSource clock_source = smclk,
            std::uint16_t pre_scale_factor = 1>
    struct USCISPI
    {
        void init()
        {
            __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
            : [ucxctl1] "=m"(*reinterpret_cast<volatile std::uint8_t*>(usci_ctl_1))
            : [ucswrst] "i"(UCSWRST)
            );

            *reinterpret_cast<volatile std::uint8_t*>(usci_ctl_0) = clock_mode | format | mode | 0x01;
            *reinterpret_cast<volatile std::uint8_t*>(usci_ctl_1) |= clock_source;
            *reinterpret_cast<volatile std::uint8_t*>(usci_br_0) = (uint8_t) (0x00ff & pre_scale_factor);
            *reinterpret_cast<volatile std::uint8_t*>(usci_br_1) = (uint8_t) ((0xff00 & pre_scale_factor) >> 8);

            *reinterpret_cast<volatile std::uint8_t*>(port_sel_0) |= mosi_miso_pin_bitmask;
            *reinterpret_cast<volatile std::uint8_t*>(port_sel_1) |= mosi_miso_pin_bitmask;

            __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
            : [ucxctl1] "=m"(*reinterpret_cast<volatile std::uint8_t*>(usci_ctl_1))
            : [ucswrst] "i"(UCSWRST)
            );
        }

        void send(data_type data)
        {
            //TODO implement
        }

        inline void sendByte(std::uint8_t byte)
        {
            //TODO non-blocking
            while (!(*reinterpret_cast<volatile std::uint8_t*>(interrupt_flag_register) & tx_interrupt_bit));
            *reinterpret_cast<volatile std::uint8_t*>(write_buf) = byte;
        }

        data_type read()
        {
            //TODO implement
        }

        std::uint8_t readByte()
        {
            while (!(*reinterpret_cast<volatile std::uint8_t*>(interrupt_flag_register) &
                     rx_interrupt_bit))
            {} //TODO non-blocking
            return *reinterpret_cast<volatile std::uint8_t*>(read_buf);
        }

        void disable()
        {
            __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
            : [ucxctl1] "=m"(*reinterpret_cast<volatile std::uint8_t*>(usci_ctl_1))
            : [ucswrst] "i"(UCSWRST)
            );
        }

        void enable()
        {
            __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
            : [ucxctl1] "=m"(*reinterpret_cast<volatile std::uint8_t*>(usci_ctl_1))
            : [ucswrst] "i"(UCSWRST)
            );
        }
    };
}


#endif //MSP430HAL_USCI_SPINTERFACE_H
