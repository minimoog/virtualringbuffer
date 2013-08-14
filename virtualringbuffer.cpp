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

#include "virtualringbuffer.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>

/**
 * Constructor
 *
 * @param capacity capacity of ring buffer, it will be rounded to nearest page size
 */
VirtualRingBuffer::VirtualRingBuffer(size_t capacity) :
    m_readOffset(0),
    m_writeOffset(0)
{
    int pageSize = sysconf(_SC_PAGESIZE);

    if (!capacity)
        capacity = 1;

    m_bufferSize = capacity + (pageSize - 1);
    m_bufferSize -= m_bufferSize & (pageSize - 1);

    m_buffer = (char *)mmap(0, 2 * m_bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (m_buffer == MAP_FAILED)
        return;

    // only Linux
    if (-1 == remap_file_pages((char *)m_buffer + m_bufferSize, m_bufferSize, 0, 0, 0)) {
        // ### TODO
        return;
    }
}

/**
 * Destructor
 */
VirtualRingBuffer::~VirtualRingBuffer()
{
    if (-1 == munmap(m_buffer, 2 * m_bufferSize)) {
        return;
    }
}

/**
 * Gets pointer where you can put data in the ring buffer.
 * After you write you need to call commit method
 *
 * @return address
 */
void * VirtualRingBuffer::reserve()
{
    return m_buffer + m_writeOffset;
}

/**
 * Commits data into the ring buffer
 *
 * @param count size of the commited data
 */
void VirtualRingBuffer::commit(size_t count)
{
    m_writeOffset += count;
}

/**
 * Gets data from end of the ring buffer.
 * You must call decommit to free data.
 *
 * @return address
 */
void * VirtualRingBuffer::take()
{
    return m_buffer + m_readOffset;
}

/**
 * Decommits (frees) data in the ring buffer
 *
 * @param count size of data to be freed
 */
void VirtualRingBuffer::decommit(size_t count)
{
    m_readOffset += count;

    if (m_readOffset >= m_bufferSize) {
        m_readOffset -= m_bufferSize;
        m_writeOffset -= m_bufferSize;
    }
}

/**
 * Gets the size of data in ring buffer
 *
 * @return size of the data
 */
size_t VirtualRingBuffer::size() const
{
    int amount = m_writeOffset - m_readOffset;

    if (amount < 0)
        amount += m_bufferSize;
    else if (amount > m_bufferSize)
        amount -= m_bufferSize;

    return amount;
}