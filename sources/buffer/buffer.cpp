#include <new>

#include "buffer.h"

onyxup::PtrBuffer onyxup::buffer::factoryBuffer(size_t in_buffer_size, size_t out_buffer_size) {
    PtrBuffer buffer = new (std::nothrow) Buffer;
    if (buffer) {
        buffer->m_in_buffer_size = in_buffer_size;
        buffer->m_out_buffer_size = out_buffer_size;
        buffer->m_in_buffer = new (std::nothrow) char[in_buffer_size];
        buffer->m_out_buffer = new (std::nothrow) char[out_buffer_size];
        if (buffer->m_in_buffer && buffer->m_out_buffer) {
            buffer->m_out_buffer_position = 0;
            buffer->m_in_buffer_position = 0;
            buffer->m_number_bytes_to_send = 0;
            return buffer;
        } else {
            delete buffer;
        }
    }
    return nullptr;
}

void onyxup::Buffer::clear() {
    m_out_buffer_position = 0;
    m_in_buffer_position = 0;
    m_number_bytes_to_send = 0;
}

void onyxup::Buffer::appendInBuffer(const char* data, size_t size) {
    memcpy(m_in_buffer + m_in_buffer_position, data, size);
    m_in_buffer_position += size;
}

void onyxup::Buffer::appendOutBuffer(const char * data, size_t size) {
    memcpy(m_out_buffer + m_out_buffer_position, data, size);
    m_number_bytes_to_send += size;
}

onyxup::Buffer::~Buffer() {
    delete [] m_in_buffer;
    delete [] m_out_buffer;
}