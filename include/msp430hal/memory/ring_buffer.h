#ifndef IR_TRAIN_DETECTOR_RING_ALLOCATOR_H
#define IR_TRAIN_DETECTOR_RING_ALLOCATOR_H

#include <cstdint>

namespace msp430hal
{
    namespace memory
    {
        template<std::size_t capacity>
        struct byte_ring_buffer
        {
            static std::uint8_t m_buffer[capacity];

            void insert(std::uint8_t byte)
            {
                // If we write at the location of the next read, we have a overflow
                if (m_next_write_index == m_next_read_index)
                    m_status_flags |= 0x01;
                // If we never wrote something in the buffer the read pointer is located after the entries
                if (m_next_read_index > capacity)
                    m_next_read_index = 0;
                m_buffer[m_next_write_index] = byte;
                m_next_write_index = (m_next_write_index + 1) % capacity;
            }

            std::uint8_t get()
            {
                // For security reasons: if we never wrote something we should return 0
                if (m_next_read_index > capacity)
                    return 0;
                std::uint8_t temp = m_buffer[m_next_read_index];
                m_next_read_index = (m_next_read_index + 1) % capacity;
                return temp;
            }

            bool overflow() const
            {
                return m_status_flags & 0x01;
            }

            void clear()
            {
                m_next_read_index = capacity + 1;
                m_next_write_index = 0;
                m_status_flags = 0;
            }

            void clear_overflow()
            {
                m_status_flags &= 0xfe;
            }

            bool empty() const
            {
                return (m_next_write_index == m_next_read_index && !overflow()) || m_next_read_index > capacity;
            }
        private:
            std::size_t m_next_read_index = capacity + 1;
            std::size_t m_next_write_index = 0;


            std::uint8_t m_status_flags = 0x00;
            //Status bits
            /*
             * Bit 0    Overflow
             * Bit 1
             * Bit 2
             * Bit 3
             * Bit 4
             * Bit 5
             * Bit 6
             * Bit 7
             */
        };
    }
}

#endif //IR_TRAIN_DETECTOR_RING_ALLOCATOR_H
