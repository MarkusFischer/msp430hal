#ifndef MSP430HAL_MULTITASKING_INTERRUPT_MUTEX_H
#define MSP430HAL_MULTITASKING_INTERRUPT_MUTEX_H

#include <msp430.h>

namespace msp430hal
{
    namespace multitasking
    {
        /// \brief a simple guard that disables interrupts at creation and reenables them when destroyed
        struct InterruptGuard
        {
            InterruptGuard()
            {
                __disable_interrupt();
            }

            ~InterruptGuard()
            {
                __enable_interrupt();
            }

            void operator=(InterruptGuard& other) = delete;
        };
    }
}

#endif //MSP430HAL_MULTITASKING_INTERRUPT_MUTEX_H
