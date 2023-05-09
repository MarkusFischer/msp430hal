//
// Created by markus on 12/13/22.
//

#ifndef MSP430HAL_USCI_USCI_H
#define MSP430HAL_USCI_USCI_H

#include <cstdint>
#include <type_traits>

#ifndef __MSP430_HAS_USCI__
#error The choosen MCU has no usci peripheral but you tried using it!
#endif

namespace msp430hal
{
    namespace usci
    {

        enum class UsciModule
        {
            usci_a,
            usci_b
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
            constexpr volatile uint8_t* getUsciRegister(int reg_no, typename std::enable_if<(module == UsciModule::usci_a), UsciModule>::type* = 0)
            {
                return usci_a_reg[instance][reg_no];
            }

            template<UsciModule module, int instance>
            constexpr volatile uint8_t* getUsciRegister(int reg_no, typename std::enable_if<(module == UsciModule::usci_b), UsciModule>::type* = 0)
            {
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
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(*ctl_1)
                : [ucswrst] "i"(UCSWRST)
                );
    #else
                *ctl_1 &= ~UCSWRST;
    #endif
            }

            static inline void disableModule()
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(*ctl_1)
                : [ucswrst] "i"(UCSWRST)
                );
    #else
                *ctl_1 |= UCSWRST;
    #endif
            }

            template<const int inst = instance>
            static inline void enableRxInterrupt(typename std::enable_if<(inst == 0), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                        return (usci_module == UsciModule::usci_a) ? UCA0RXIE : UCB0RXIE;
                    }));
    #else
                IE2 |= (usci_module == UsciModule::usci_a) ? UCA0RXIE : UCB0RXIE;
    #endif
            }

    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static inline void enableRxInterrupt(typename std::enable_if<(inst == 1), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA1RXIE : UCB1RXIE;
                }));
    #else
                UC1IE |= (usci_module == UsciModule::usci_a) ? UCA1RXIE : UCB1RXIE;
    #endif
            }
    #endif

            template<const int inst = instance>
            static inline void enableTxInterrupt(typename std::enable_if<(inst == 0), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA0TXIE : UCB0TXIE;
                }));
    #else
                IE2 |= (usci_module == UsciModule::usci_a) ? UCA0TXIE : UCB0TXIE;
    #endif
            }

    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static inline void enableTxInterrupt(typename std::enable_if<(inst == 1), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA1TXIE : UCB1TXIE;
                }));
    #else
                UC1IE |= (usci_module == UsciModule::usci_a) ? UCA1TXIE : UCB1TXIE;
    #endif
            }
    #endif

            template<const int inst = instance>
            static inline void enableInterrupts(typename std::enable_if<(inst == 0), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA0RXIE | UCA0TXIE : UCB0RXIE | UCB0TXIE;
                }));
    #else
                IE2 |= (usci_module == UsciModule::usci_a) ? UCA0RXIE | UCA0TXIE : UCB0RXIE | UCB0TXIE;
    #endif
            }

    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static inline void enableInterrupts(typename std::enable_if<(inst == 1), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA1RXIE | UCA1TXIE : UCB1RXIE | UCB1TXIE;
                }));
    #else
                UC1IE |= (usci_module == UsciModule::usci_a) ? UCA1RXIE | UCA1TXIE : UCB1RXIE | UCB1TXIE;
    #endif
            }
    #endif

            template<const int inst = instance>
            static inline void disableRxInterrupt(typename std::enable_if<(inst == 0), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIC.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA0RXIE : UCB0RXIE;
                }));
    #else
                IE2 &= ~((usci_module == UsciModule::usci_a) ? UCA0RXIE : UCB0RXIE);
    #endif
            }

    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static inline void disableRxInterrupt(typename std::enable_if<(inst == 1), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA1RXIE : UCB1RXIE;
                }));
    #else
                UC1IE &= ~((usci_module == UsciModule::usci_a) ? UCA1RXIE : UCB1RXIE);
    #endif
            }
    #endif

            template<const int inst = instance>
            static inline void disableTxInterrupt(typename std::enable_if<(inst == 0), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA0TXIE : UCB0TXIE;
                }));
    #else
                IE2 &= ~((usci_module == UsciModule::usci_a) ? UCA0TXIE : UCB0TXIE);
    #endif
            }

    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static inline void disableTxInterrupt(typename std::enable_if<(inst == 1), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA1TXIE : UCB1TXIE;
                }));
    #else
                UC1IE &= ~((usci_module == UsciModule::usci_a) ? UCA1TXIE : UCB1TXIE);
    #endif
            }
    #endif

            template<const int inst = instance>
            static inline void disableInterrupts(typename std::enable_if<(inst == 0), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(IE2)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA0RXIE | UCA0TXIE : UCB0RXIE | UCB0TXIE;
                }));
    #else
                IE2 &= ~((usci_module == UsciModule::usci_a) ? UCA0RXIE | UCA0TXIE : UCB0RXIE | UCB0TXIE);
    #endif
            }

    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static inline void disableInterrupts(typename std::enable_if<(inst == 1), void>::type* = nullptr)
            {
    #ifdef __GNUC__
                __asm__ __volatile__ ( "BIS.B %[ucswrst], %[ucxctl1]"
                        : [ucxctl1] "=m"(UC1IE)
                : [ucswrst] "i"([&] {
                    return (usci_module == UsciModule::usci_a) ? UCA1RXIE | UCA1TXIE : UCB1RXIE | UCB1TXIE;
                }));
    #else
                UC1IE &= ~((usci_module == UsciModule::usci_a) ? UCA1RXIE | UCA1TXIE : UCB1RXIE | UCB1TXIE);
    #endif
            }
    #endif

            template<const int inst = instance>
            static bool isRxInterruptPending(typename std::enable_if<(instance == 0), void>::type* = nullptr)
            {
                return IFG2 & ((usci_module == UsciModule::usci_a) ? UCA0RXIFG : UCB0RXIFG);
            }
    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static bool isRxInterruptPending(typename std::enable_if<(instance == 1), void>::type* = nullptr)
            {
                return UC1IFG & (usci_module == UsciModule::usci_a) ? UCA1RXIFG : UCB1RXIFG;
            }
    #endif

            template<const int inst = instance>
            static bool isTxInterruptPending(typename std::enable_if<(instance == 0), void>::type* = nullptr)
            {
                return IFG2 & ((usci_module == UsciModule::usci_a) ? UCA0TXIFG : UCB0TXIFG);
            }
    #ifdef __MSP430_HAS_USCI_AB1__
            template<const int inst = instance>
            static bool isTxInterruptPending(typename std::enable_if<(instance == 1), void>::type* = nullptr)
            {
                return UC1IFG & (usci_module == UsciModule::usci_a) ? UCA1TXIFG : UCB1TXIFG;
            }
    #endif
        };
    }
}
#endif //MSP430HAL_USCI_USCI_H
