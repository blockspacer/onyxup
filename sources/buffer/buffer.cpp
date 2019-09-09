#include <new>

#include "buffer.h"

onyxup::PtrBuffer onyxup::buffer::bufferFactory(size_t in_len, size_t out_len) {
    PtrBuffer buffer = new (std::nothrow) Buffer;
    if (buffer) {
        buffer->inputBufferLength = in_len;
        buffer->outputBufferLength = out_len;
        buffer->inputBuffer = new (std::nothrow) char[in_len];
        buffer->outputBuffer = new (std::nothrow) char[out_len];
        if (buffer->inputBuffer && buffer->outputBuffer) {
            buffer->posOutputBuffer = 0;
            buffer->posInputBuffer = 0;
            buffer->numberBytesToSend = 0;
            return buffer;
        } else {
            delete buffer;
        }
    }
    return nullptr;
}

void onyxup::Buffer::clear() {
    posOutputBuffer = 0;
    posInputBuffer = 0;
    numberBytesToSend = 0;
}

void onyxup::Buffer::addDataToInputBuffer(const char* data, size_t n) {
    memcpy(inputBuffer + posInputBuffer, data, n);
    posInputBuffer += n;
}

void onyxup::Buffer::addDataToOutputBuffer(const char * data, size_t n) {
    memcpy(outputBuffer + posOutputBuffer, data, n);
    numberBytesToSend += n;
}

onyxup::Buffer::~Buffer() {
    delete [] inputBuffer;
    delete [] outputBuffer;
}