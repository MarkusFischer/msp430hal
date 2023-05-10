#ifndef MSP430HAL_USCI_UART_H
#define MSP430HAL_USCI_UART_H

#include "usci.h"
#include <cmath>

namespace msp430hal
{
    namespace usci
    {
        enum UARTMode : std::uint8_t
        {
            uart = 0x00,
            idle_line_multiprocessor = 0x02,
            address_bit_multiprocessor = 0x04,
            uart_automatic_baud = 0x06
        };

        enum UARTClockSource : std::uint8_t
        {
            uclk = 0x00,
            aclk = 0x40,
            smclk = 0x80
        };

        template<std::uint8_t instance,
                 std::uint32_t baud_rate,
                 std::uint32_t brclk,
                 UARTClockSource clock_source = uclk,
                 bool oversampling_baud_generator = false,
                 bool enable_parity = false,
                 bool even_parity = false,
                 bool msb_first = false,
                 bool seven_bit_data = false,
                 bool two_stop_bits = false,
                 UARTMode mode = uart,
                 bool sync_mode = false>

        struct UART_t
        {
            typedef Usci_t<UsciModule::usci_a, instance> Usci;

            static constexpr double division_factor()
            {
                return static_cast<double>(brclk)/static_cast<double>(baud_rate);
            }

            static const std::uint8_t instance_value = instance;
            static const std::uint32_t baud_rate_value = baud_rate;
            static const std::uint32_t brclk_value = brclk;
            static const bool oversampling_baud_generator_value = (oversampling_baud_generator && division_factor() >= 16.);
            static const bool enable_parity_value = enable_parity;
            static const bool even_parity_value = even_parity;
            static const bool msb_first_value = msb_first;
            static const bool seven_bit_data_value = seven_bit_data;
            static const bool two_stop_bits_value = two_stop_bits;
            static const UARTMode mode_value = mode;
            static const bool sync_mode_value = sync_mode;
            static const UARTClockSource clock_source_value = clock_source;


            static constexpr std::uint16_t ucbr_value()
            {
                return (oversampling_baud_generator_value) ? static_cast<std::uint16_t>(division_factor()) : static_cast<std::uint16_t>(division_factor() / 16.);
            }

            static constexpr std::uint8_t ucbrs_value()
            {
                return static_cast<std::uint8_t>(std::round((division_factor() - static_cast<std::uint16_t>(division_factor())) * 8));
            }

            static constexpr std::uint8_t ucbrf_value()
            {
                return static_cast<std::uint8_t>(std::round((division_factor()/16. - static_cast<std::uint16_t>(division_factor()/16.)) * 16.));
            }

            static constexpr std::uint8_t modulation_control_value()
            {
                return (ucbrf_value() << 4) | (ucbrs_value() << 3) | oversampling_baud_generator_value;
            }

            static void init()
            {
                Usci::disableModule();

                *Usci::ctl_0 = (enable_parity_value << 7) | (even_parity_value << 6) | (msb_first_value << 5) |
                        (seven_bit_data_value << 4) | (two_stop_bits_value << 3) | mode_value | sync_mode_value;
                *Usci::ctl_1 |= clock_source;
                //TODO: in theory we could perform one single write to br_0 as 16 bit register
                *Usci::br_0 = static_cast<std::uint8_t>(0x00ff & ucbr_value());
                *Usci::br_1 = static_cast<std::uint8_t>((0xff00 & ucbr_value()) >> 8);
                *Usci::mctl = modulation_control_value();

                Usci::enableModule();
            }

        };

    }
}

#endif //MSP430HAL_USCI_UART_H
