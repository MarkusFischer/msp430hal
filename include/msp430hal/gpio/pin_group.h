#ifndef MSP430HAL_GPIO_PIN_GROUP_H
#define MSP430HAL_GPIO_PIN_GROUP_H

#include <cstdint>
#include <initializer_list>

#include "_gpio_registers.h"
#include "pin.h"

namespace msp430hal
{
    namespace gpio
    {
        struct PinGroupElement
        {
            Pin pin;
            Port port;
        };

        template<std::size_t size>
        class PinGroup
        {
        private:
            PinGroupElement m_elements[size];
        public:

            PinGroup(std::initializer_list<PinGroupElement> elements)
            {
                std::size_t index = 0;
                for (auto element : elements)
                {
                    m_elements[index] = element;
                    index++;
                    if (index > size)
                        break;
                }
            }

            inline void bind(std::initializer_list<PinGroupElement> elements)
            {
                std::size_t index = 0;
                for (auto element : elements)
                {
                    m_elements[index] = element;
                    index++;
                    if (index > size)
                        break;
                }
            }

            inline void bind(std::size_t index, PinGroupElement element)
            {
                m_elements[index] = element;
            }

            inline void set(RegisterType reg_type, std::size_t index)
            {
                *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) |= m_elements[index].pin;
            }

            inline void set(std::size_t index)
            {
                set(RegisterType::out, index);
            }


            inline void clear(RegisterType reg_type, std::size_t index)
            {
                *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) &= ~m_elements[index].pin;
            }

            inline void clear(std::size_t index)
            {
                clear(RegisterType::out, index);
            }

            inline void toggle(RegisterType reg_type, std::size_t index)
            {
                *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) ^= m_elements[index].pin;
            }

            inline void toggle(std::size_t index)
            {
                return toggle(RegisterType::out, index);
            }

            inline std::uint8_t get(RegisterType reg_type, std::size_t index)
            {
                return *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) & m_elements[index].pin;
            }

            inline std::uint8_t inputLevel(std::size_t index)
            {
                return get(RegisterType::in, index);
            }



        };
    }
}

#endif //MSP430HAL_GPIO_PIN_GROUP_H
