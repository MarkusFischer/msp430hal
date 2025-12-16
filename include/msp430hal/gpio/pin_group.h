#ifndef MSP430HAL_GPIO_PIN_GROUP_H
#define MSP430HAL_GPIO_PIN_GROUP_H

#include <cstdint>
#include <initializer_list>
#include <string.h>

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

            void bind(std::initializer_list<PinGroupElement> elements)
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

            void bind(std::size_t index, PinGroupElement element)
            {
                m_elements[index] = element;
            }

            void set(RegisterType reg_type, std::size_t index)
            {
                *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) |= m_elements[index].pin;
            }

            [[deprecated("use PinGroup_t")]]
            void set(std::size_t index)
            {
                set(RegisterType::out, index);
            }


            void clear(RegisterType reg_type, std::size_t index)
            {
                *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) &= ~m_elements[index].pin;
            }

            [[deprecated("use PinGroup_t")]]
            void clear(std::size_t index)
            {
                clear(RegisterType::out, index);
            }

            void toggle(RegisterType reg_type, std::size_t index)
            {
                *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) ^= m_elements[index].pin;
            }

            [[deprecated("use PinGroup_t")]]
            void toggle(std::size_t index)
            {
                return toggle(RegisterType::out, index);
            }

            std::uint8_t get(RegisterType reg_type, std::size_t index)
            {
                return *_gpio_registers::getGPIORegister(reg_type, m_elements[index].port) & m_elements[index].pin;
            }

            [[deprecated("use PinGroup_t")]]
            std::uint8_t inputLevel(std::size_t index)
            {
                return get(RegisterType::in, index);
            }
        };

        template<typename Pin, typename... Pins>
        struct PinGroup_t
        {
            static_assert(is_GPIO_Pin_v<Pin>, "Pin type must be GPIO Pin");
            using Pin_type = Pin;
            using rest = PinGroup_t<Pins...>;


            static void init()
            {
                Pin_type::init();
                rest::init();
            }

            template<std::size_t index>
            static void set()
            {
                if constexpr (index == 0)
                    Pin_type::set();
                else
                    rest::template set<index - 1>();
            }

            static void set()
            {
                Pin_type::set();
                rest::set();
            }

            template<std::size_t index>
            static void clear()
            {
                if constexpr (index == 0)
                    Pin_type::clear();
                else
                    rest::template clear<index - 1>();
            }

            static void clear()
            {
                Pin_type::clear();
                rest::clear();
            }

            template<std::size_t index>
            static void toggle()
            {
                if constexpr (index == 0)
                    Pin_type::toggle();
                else
                    rest::template toggle<index - 1>();
            }

            static void toggle()
            {
                Pin_type::toggle();
                rest::toggle();
            }

            template<std::size_t index>
            static std::uint8_t inputLevel()
            {
                if constexpr (index == 0)
                    return Pin_type::inputLevel();
                else
                    return rest::template inputLevel<index - 1>();
            }

            template<std::size_t index>
            static void switchFunction(PinFunction function)
            {
                if constexpr (index == 0)
                    Pin_type::switchFunction(function);
                else
                    rest::template switchFunction<index - 1>(function);
            }

            static void switchFunction(PinFunction function)
            {
                Pin_type::switchFunction(function);
                rest::switchFunction(function);
            }

            [[nodiscard]]
            static constexpr std::size_t size() { return 1 + rest::size(); }


        };


        template<typename Pin>
        struct PinGroup_t<Pin>
        {
            static_assert(is_GPIO_Pin_v<Pin>, "Pin type must be GPIO Pin");
            using Pin_type = Pin;

            static void init()
            {
                Pin_type::init();
            }

            template<std::size_t index>
            static void set()
            {
                static_assert(index == 0, "Pin index must be 0 for pin group with a single element");
                Pin_type::set();
            }

            static void set()
            {
                Pin_type::set();
            }

            template<std::size_t index>
            static void clear()
            {
                static_assert(index == 0, "Pin index must be 0 for pin group with a single element");
                Pin_type::clear();
            }

            static void clear()
            {
                Pin_type::clear();
            }

            template<std::size_t index>
            static void toggle()
            {
                static_assert(index == 0, "Pin index must be 0 for pin group with a single element");
                Pin_type::toggle();
            }

            static void toggle()
            {
                Pin_type::toggle();
            }

            template<std::size_t index>
            static std::uint8_t inputLevel()
            {
                static_assert(index == 0, "Pin index must be 0 for pin group with a single element");
                return Pin_type::inputLevel();
            }

            template<std::size_t index>
            static void switchFunction(PinFunction function)
            {
                static_assert(index == 0, "Pin index must be 0 for pin group with a single element");
                Pin_type::switchFunction(function);
            }

            static void switchFunction(PinFunction function)
            {
                Pin_type::switchFunction(function);
            }

            [[nodiscard]]
            static constexpr std::size_t size() { return 1; }
        };
    }
}

#endif //MSP430HAL_GPIO_PIN_GROUP_H
