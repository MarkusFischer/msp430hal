#ifndef IR_TRAIN_DETECTOR_I2C_H
#define IR_TRAIN_DETECTOR_I2C_H

#include <msp430.h>
#include <cstdint>
#include "usci.h"
#include "../memory/ring_buffer.h"

namespace msp430hal
{
    namespace usci
    {
        namespace
        {
            static constexpr volatile unsigned int* usci_b_i2c_reg[][2] = {
                    {&UCB0I2COA, &UCB0I2CSA},
#ifdef __MSP430_HAS_USCI_AB1__
                    {&UCB1I2COA, &UCB1I2CSA}
#endif
            };

            template<int instance>
            constexpr volatile unsigned int* getUsciI2CRegister(int reg_no)
            {
                return usci_b_i2c_reg[instance][reg_no];
            }
        }

        enum I2CAdressingMode : std::uint8_t
        {
            addressing_7 = 0x00,
            addressing_10 = 0x01
        };

        enum I2CMode
        {
            transmit,
            receive
        };

        template<std::uint8_t instance,
                 UsciClockSource clock_source = uclk,
                 std::uint16_t ucbr = 0x0001,
                 bool master_mode = true,
                 I2CAdressingMode own_addressing_mode = addressing_7,
                 bool multi_master_env = false,
                 std::uint16_t own_address = 0x0000>
        struct I2C_t
        {
            typedef Usci_t<UsciModule::usci_b, instance> Usci;

            static constexpr volatile unsigned int* i2coa = getUsciI2CRegister<instance>(0);
            static constexpr volatile unsigned int* i2csa = getUsciI2CRegister<instance>(1);

            static void init()
            {
                Usci::disableModule();

                *Usci::ctl_0 = (own_addressing_mode << 7) | (multi_master_env << 5) |
                                (master_mode << 3) | 0x07;
                *Usci::ctl_1 = clock_source | 0x1;

                if (own_addressing_mode == addressing_7)
                    *i2coa |= own_address & 0x007f;
                else
                    *i2coa |= own_address & 0x02ff;

                *Usci::br_0 = static_cast<std::uint8_t>(ucbr & 0x00ff);
                *Usci::br_1 = static_cast<std::uint8_t>((ucbr & 0xff00) >> 8);
            }

            static void prepareTransmit(std::uint16_t slave_address, I2CAdressingMode slave_adressing_mode = I2CAdressingMode::addressing_7)
            {
                Usci::disableModule();

                if (slave_adressing_mode == I2CAdressingMode::addressing_7)
                {
                    *Usci::ctl_0 |= (slave_adressing_mode << 6);
                    *i2csa |= slave_address & 0x007f;
                }
                else
                {
                    *Usci::ctl_0 &= ~(slave_adressing_mode << 6);
                    *i2csa |= slave_address & 0x02ff;
                }
                setMode(I2CMode::transmit);

                Usci::enableModule();
            }

            static void prepareReceive(std::uint16_t slave_address, I2CAdressingMode slave_adressing_mode = I2CAdressingMode::addressing_7)
            {
                Usci::disableModule();

                if (slave_adressing_mode == I2CAdressingMode::addressing_7)
                {
                    *Usci::ctl_0 |= (slave_adressing_mode << 6);
                    *i2csa |= slave_address & 0x007f;
                }
                else
                {
                    *Usci::ctl_0 &= ~(slave_adressing_mode << 6);
                    *i2csa |= slave_address & 0x02ff;
                }
                setMode(I2CMode::receive);

                Usci::enableModule();
            }

            static void enable()
            {
                Usci::enableModule();
            }

            static void disable()
            {
                Usci::disableModule();
            }

            static void generateStartCondition()
            {
                *Usci::ctl_1 |= UCTXSTT;
            }

            static bool getStartCondition()
            {
                return *Usci::ctl_1 & UCTXSTT;
            }

            static void generateStopCondition()
            {
                *Usci::ctl_1 |= UCTXSTP;
            }

            static bool getStopCondition()
            {
                return *Usci::ctl_1 & UCTXSTP;
            }

            static bool getTransmitInterruptFlag()
            {
                return *Usci::ifg & UCB0TXIFG;
            }

            static bool getReceiveInterruptFlag()
            {
                return *Usci::ifg & UCB0RXIFG;
            }

            static void setMode(I2CMode mode)
            {
                if (mode == I2CMode::transmit)
                    *Usci::ctl_1 |= UCTR;
                else
                    *Usci::ctl_1 &= ~UCTR;
            }

        };

        template<std::uint8_t instance,
                std::size_t buffer_capacity,
                UsciClockSource clock_source = uclk,
                std::uint16_t ucbr = 0x0001,
                bool master_mode = true,
                I2CAdressingMode own_addressing_mode = addressing_7,
                bool multi_master_env = false,
                std::uint16_t own_address = 0x0000>
        struct BufferedBlockingI2CMaster
        {
            typedef I2C_t<instance, clock_source, ucbr, true, own_addressing_mode, multi_master_env, own_address> I2C;

            memory::byte_ring_buffer<buffer_capacity> buffer;

            BufferedBlockingI2CMaster()
            {
                I2C::init();
            }

            void writeByte(std::uint16_t slave_address, std::uint8_t byte, I2CAdressingMode slave_addressing_mode = addressing_7)
            {
                I2C::prepareTransmit(slave_address, slave_addressing_mode);
                I2C::generateStartCondition();
                while (!I2C::getTransmitInterruptFlag());
                *I2C::Usci::tx_buf = byte;

                I2C::generateStopCondition();
            }

            void writeBytes(std::uint16_t slave_address, std::uint8_t* start, std::size_t bytes, I2CAdressingMode slave_addressing_mode = addressing_7)
            {
                I2C::prepareTransmit(slave_address, slave_addressing_mode);
                I2C::generateStartCondition();
                std::size_t index = 0;
                while (index < bytes)
                {
                    while(!I2C::getTransmitInterruptFlag());
                    *I2C::Usci::tx_buf = *(start + index);
                    ++index;
                }
                I2C::generateStopCondition();
            }

            void writeWord(std::uint16_t slave_address, std::uint16_t word, I2CAdressingMode slave_addressing_mode = addressing_7)
            {
                I2C::prepareTransmit(slave_address, slave_addressing_mode);
                I2C::generateStartCondition();
                while (!I2C::getTransmitInterruptFlag());
                *I2C::Usci::tx_buf = static_cast<std::uint8_t>(word & 0x00ff);
                while (!I2C::getTransmitInterruptFlag());
                *I2C::Usci::tx_buf = static_cast<std::uint8_t>(word & 0xff00 >> 8);
                I2C::generateStopCondition();
            }


            void startBurstWrite(std::uint16_t slave_address, I2CAdressingMode slave_addressing_mode = addressing_7)
            {
                I2C::prepareTransmit(slave_address, slave_addressing_mode);
                I2C::generateStartCondition();
            }

            void burstWrite(std::uint8_t byte)
            {
                while (!I2C::getTransmitInterruptFlag());
                *I2C::Usci::tx_buf = byte;
            }

            void stopBurstWrite()
            {
                I2C::generateStopCondition();
            }

            void writeRegister(std::uint16_t slave_address, std::uint8_t reg_addr, std::uint8_t* data_start, std::size_t count, I2CAdressingMode slave_addressing_mode = addressing_7)
            {
                I2C::prepareTransmit(slave_address, slave_addressing_mode);
                I2C::generateStartCondition();
                while (!I2C::getTransmitInterruptFlag());
                *I2C::Usci::tx_buf = reg_addr;

                std::size_t index = 0;
                while (index < count)
                {
                    while(!I2C::getTransmitInterruptFlag());
                    *I2C::Usci::tx_buf = *(data_start + index);
                    ++index;
                }
            }

            void readRegister(std::uint16_t slave_address, std::uint8_t reg_addr, std::size_t count, I2CAdressingMode slave_addressing_mode = addressing_7)
            {
                I2C::prepareTransmit(slave_address, slave_addressing_mode);
                I2C::generateStartCondition();
                while (!I2C::getTransmitInterruptFlag());
                *I2C::Usci::tx_buf = reg_addr;
                I2C::setMode(I2CMode::receive);
                std::size_t read = 0;
                if (count == 1)
                {
                    I2C::generateStartCondition();
                    while (I2C::getStartCondition());
                    I2C::generateStopCondition();
                }
                while (read < count)
                {
                    while (!I2C::getReceiveInterruptFlag());
                    buffer.insert(*I2C::Usci::rx_buf);
                    ++count;
                }
                I2C::generateStopCondition();
            }

        };
    }
}

#endif //IR_TRAIN_DETECTOR_I2C_H
