#pragma once

#include <string.h>
#include "../plog/Log.h"

namespace onyxup {
    
    class Buffer;
    
    using PtrBuffer = Buffer *;
    
    namespace buffer {
        PtrBuffer bufferFactory(size_t in_len, size_t out_len);
    }

    class Buffer {
        friend PtrBuffer buffer::bufferFactory(size_t in_len, size_t out_len);
    private:
        char * inputBuffer;
        char * outputBuffer;
        size_t posOutputBuffer;
        size_t posInputBuffer;
        size_t numberBytesToSend;

        size_t  inputBufferLength;
        size_t  outputBufferLength;

        Buffer(){}
    public:
        
        ~Buffer();

        void clear();

        inline char* getInputBuffer() {
            return inputBuffer;
        }
        
        inline char* getOutputBuffer() const {
            return outputBuffer;
        }

        inline size_t getPosInputBuffer() const {
            return posInputBuffer;
        }
        
        inline size_t getPosOutputBuffer() const {
            return posOutputBuffer;
        }
        
        inline size_t getBytesToSend() const {
            return numberBytesToSend;
        }
        
        inline void setPosOutputBuffer(size_t pos) {
            posOutputBuffer = pos;
        }
        
        inline void setBytesToSend(size_t n) {
            numberBytesToSend = n;
        }

        void addDataToInputBuffer(const char * data, size_t n);
        void addDataToOutputBuffer(const char * data, size_t n);
    };

    

}