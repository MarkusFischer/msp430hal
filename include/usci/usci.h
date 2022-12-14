//
// Created by markus on 12/13/22.
//

#ifndef MSP430HAL_USCI_USCI_H
#define MSP430HAL_USCI_USCI_H

#include <cstdint>

#ifndef __MSP430_HAS_USCI__
#error The choosen MCU has no usci peripheral but you tried using it!
#endif

namespace msp430hal::usci
{
    enum class UsciModule
    {
        usci_a, usci_b
    };

    namespace
    {

        static constexpr volatile uint8_t* usci_a_reg[][7] = {
                {&UCA0CTL0, &UCA0CTL1, &UCA0BR0, &UCA0BR1, &UCA0STAT, &UCA0RXBUF, &UCA0TXBUF},
        #ifdef __MSP430_HAS_USCI_AB1__
                {&UCA1CTL0, &UCA1CTL1, &UCA1BR0, &UCA1BR1, &UCA1STAT, &UCA1RXBUF, &UCA1TXBUF},
        #endif
        };

        static constexpr volatile uint8_t* usci_b_reg[][7] = {
                {&UCB0CTL0, &UCB0CTL1, &UCB0BR0, &UCB0BR1, &UCB0STAT, &UCB0RXBUF, &UCB0TXBUF},
        #ifdef __MSP430_HAS_USCI_AB1__
                {&UCB1CTL0, &UCB1CTL1, &UCB1BR0, &UCB1BR1, &UCB1STAT, &UCB1RXBUF, &UCB1TXBUF},
        #endif
        };

        template<UsciModule module, int instance>
        constexpr volatile uint8_t* getUsciRegister(int reg_no)
        {
            if constexpr(module == UsciModule::usci_a)
                return usci_a_reg[instance][reg_no];
            else
                return usci_b_reg[instance][reg_no];
        }
    }



    template<const UsciModule usci_module, const int instance>
    struct Usci_t
    {
        static constexpr volatile std::uint8_t* ctl_0 = getUsciRegister<usci_module, instance>(0);
        static constexpr volatile std::uint8_t* ctl_1 = getUsciRegister<usci_module, instance>(1);
        static constexpr volatile std::uint8_t* br_0 = getUsciRegister<usci_module, instance>(2);
        static constexpr volatile std::uint8_t* br_1 = getUsciRegister<usci_module, instance>(3);
        static constexpr volatile std::uint8_t* stat = getUsciRegister<usci_module, instance>(4);
        static constexpr volatile std::uint8_t* rx_buf = getUsciRegister<usci_module, instance>(5);
        static constexpr volatile std::uint8_t* tx_buf = getUsciRegister<usci_module, instance>(6);

        static inline void enableModule()
        {
            __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
            : [ucxctl1] "=m"(*ctl_1)
            : [ucswrst] "i"(UCSWRST)
            );
        }

        static inline void disableModule()
        {
            __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
            : [ucxctl1] "=m"(*ctl_1)
            : [ucswrst] "i"(UCSWRST)
            );
        }

        static inline void enableRxInterrupt()
        {
            if constexpr(instance == 0)
            {
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA0RXIE;
                    else
                        return UCB0RXIE;
                    })
                );
            }
            #ifdef __MSP430_HAS_USCI_AB1__
            else
            {
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA1RXIE;
                    else
                        return UCB1RXIE;
                })
                );
            }
            #endif
        }

        static inline void disableRxInterrupt()
        {
            if constexpr(instance == 0)
            {
                __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA0RXIE;
                    else
                        return UCB0RXIE;
                })
                );
            }
            #ifdef __MSP430_HAS_USCI_AB1__
            else
            {
                __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA1RXIE;
                    else
                        return UCB1RXIE;
                })
                );
            }
            #endif
        }

        static inline void enableTxInterrupt()
        {
            if constexpr(instance == 0)
            {
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA0TXIE;
                    else
                        return UCB0TXIE;
                })
                );
            }
            #ifdef __MSP430_HAS_USCI_AB1__
            else
            {
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA1TXIE;
                    else
                        return UCB1TXIE;
                })
                );
            }
            #endif
        }

        static void disableTxInterrupt()
        {
            if constexpr(instance == 0)
            {
                __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA0TXIE;
                    else
                        return UCB0TXIE;
                })
                );
            }
            #ifdef __MSP430_HAS_USCI_AB1__
            else
            {
                __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    if constexpr(usci_module == UsciModule::usci_a)
                        return UCA1TXIE;
                    else
                        return UCB1TXIE;
                })
                );
            }
            #endif
        }
    };

}
#endif //MSP430HAL_USCI_USCI_H
