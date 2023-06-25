#ifndef MSP430HAL_USCI_UART_H
#define MSP430HAL_USCI_UART_H

#include "usci.h"
#include "../util/math.h"

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

            static constexpr std::uint8_t instance_value = instance;
            static constexpr std::uint32_t baud_rate_value = baud_rate;
            static constexpr std::uint32_t brclk_value = brclk;
            static constexpr bool oversampling_baud_generator_value = (oversampling_baud_generator && division_factor() >= 16.);
            static constexpr bool enable_parity_value = enable_parity;
            static constexpr bool even_parity_value = even_parity;
            static constexpr bool msb_first_value = msb_first;
            static constexpr bool seven_bit_data_value = seven_bit_data;
            static constexpr bool two_stop_bits_value = two_stop_bits;
            static constexpr UARTMode mode_value = mode;
            static constexpr bool sync_mode_value = sync_mode;
            static constexpr UARTClockSource clock_source_value = clock_source;

            static constexpr std::uint16_t ucbr_value()
            {
                return (oversampling_baud_generator_value) ? static_cast<std::uint16_t>(division_factor() / 16.) : static_cast<std::uint16_t>(division_factor());
            }

            static constexpr std::uint8_t ucbrs_value()
            {
                return round_t<std::uint8_t, double>((division_factor() - static_cast<std::uint16_t>(division_factor())) * 8);
            }

            static constexpr std::uint8_t ucbrf_value()
            {
                return round_t<std::uint8_t, double>((division_factor()/16. - static_cast<std::uint16_t>(division_factor()/16.)) * 16.);
            }

            static constexpr std::uint8_t modulation_control_value()
            {
                return (ucbrf_value() << 4) | (ucbrs_value() << 3) | oversampling_baud_generator_value;
            }

            static constexpr std::uint16_t ucbr = ucbr_value();
            static constexpr std::uint8_t modulation_control = modulation_control_value();

            static void init()
            {
                Usci::disableModule();

                *Usci::ctl_0 = (enable_parity_value << 7) | (even_parity_value << 6) | (msb_first_value << 5) |
                        (seven_bit_data_value << 4) | (two_stop_bits_value << 3) | mode_value | sync_mode_value;
                *Usci::ctl_1 = clock_source | 0x1;
                //in theory we could perform one single write to br_0 as 16 bit register
                *Usci::br_0 = static_cast<std::uint8_t>(0x00ff & ucbr);
                *Usci::br_1 = static_cast<std::uint8_t>((0xff00 & ucbr) >> 8);
                *Usci::mctl = modulation_control;

            }

            static void enableErroneousCharacterInterrupt()
            {
                *Usci::ctl_1 |= UCRXEIE;
            }

            static void enableBreakCharacterInterrupt()
            {
                *Usci::ctl_1 |= UCBRKIE;
            }

            static void disableErroneousCharacterInterrupt()
            {
                *Usci::ctl_1 &= ~UCRXEIE;
            }

            static void disableBreakCharacterInterrupt()
            {
                *Usci::ctl_1 &= ~UCBRKIE;
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

            static bool readParityError()
            {
                return *Usci::stat & UCPE;
            }

            static bool parityError()
            {
                bool error = *Usci::stat & UCPE;
                *Usci::stat &= ~UCPE;
                return error;
            }

            static bool receiveError()
            {
                return *Usci::stat & UCRXERR;
            }

            static bool busy()
            {
                return *Usci::stat & UCBUSY;
            }

            //TODO handle break condition

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
}

#endif //MSP430HAL_USCI_UART_H
