// Copyright 2013 Antonie Jovanoski
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ringbuffer.h"
#include <algorithm>
#include <string.h>

RingBuffer::RingBuffer(size_t capacity)
    : m_begin(0),
      m_end(0),
      m_size(0),
      m_capacity(capacity)
{
    m_data = new char[capacity];
}

RingBuffer::~RingBuffer()
{
    delete []m_data;
}

size_t RingBuffer::write(const char *data, size_t size)
{
    if (size == 0)
        return 0;

    size_t capacity = m_capacity;
    size_t bytesToWrite = std::min(size, capacity - m_size);

    if (bytesToWrite <= (capacity - m_end)) {
        memcpy(m_data + m_end, data, bytesToWrite);
        m_end += bytesToWrite;

        if (m_end == capacity)
            m_end = 0;
    } else {
        size_t sizeFirstPart = capacity - m_end;
        memcpy(m_data + m_end, data, sizeFirstPart);

        size_t sizeSecondPart = bytesToWrite - sizeFirstPart;
        memcpy(m_data, data + sizeFirstPart, sizeSecondPart);

        m_end = sizeSecondPart;
    }

    m_size += bytesToWrite;

    return bytesToWrite;
}

size_t RingBuffer::read(char *data, size_t size)
{
    if (size == 0)
        return 0;

    size_t bytesToRead = std::min(size, m_size);

    if (bytesToRead <= (m_capacity - m_begin)) {
        memcpy(data, m_data + m_begin, bytesToRead);
        m_begin += bytesToRead;

        if (m_begin == m_capacity)
            m_begin = 0;
    } else {
        size_t sizeFirstPart = m_capacity - m_begin;
        memcpy(data, m_data + m_begin, sizeFirstPart);

        size_t sizeSecondPart = bytesToRead - sizeFirstPart;
        memcpy(data + sizeFirstPart, m_data, sizeSecondPart);

        m_begin = sizeSecondPart;
    }

    m_size -= bytesToRead;

    return bytesToRead;
}
