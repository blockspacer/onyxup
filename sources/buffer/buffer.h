#pragma once

#include <string.h>
#include "../plog/Log.h"

namespace onyxup {
    
    class Buffer;
    
    using PtrBuffer = Buffer *;
    
    namespace buffer {
        PtrBuffer factoryBuffer(size_t in_buffer_size, size_t out_buffer_size);
    }

    class Buffer {
        friend PtrBuffer buffer::factoryBuffer(size_t in_buffer_size, size_t out_buffer_size);
    private:
        char * m_in_buffer;
        char * m_out_buffer;
        size_t m_out_buffer_position;
        size_t m_in_buffer_position;
        size_t m_number_bytes_to_send;

        size_t m_in_buffer_size;
        size_t m_out_buffer_size;

        Buffer(){}
    public:
        
        ~Buffer();

        void clear();

        inline char* getInBuffer() {
            return m_in_buffer;
        }
        
        inline char* getOutBuffer() const {
            return m_out_buffer;
        }

        inline size_t getInBufferPosition() const {
            return m_in_buffer_position;
        }
        
        inline size_t getOutBufferPosition() const {
            return m_out_buffer_position;
        }
        
        inline size_t getBytesToSend() const {
            return m_number_bytes_to_send;
        }
        
        inline void setOutBufferPosition(size_t out_buffer_position) {
            m_out_buffer_position = out_buffer_position;
        }
        
        inline void setBytesToSend(size_t nbytes_to_send) {
            m_number_bytes_to_send = nbytes_to_send;
        }

        
        void appendInBuffer(const char * data, size_t size);
        void appendOutBuffer(const char * data, size_t size);
    };

    

}