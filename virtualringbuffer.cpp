/*
 *  Copyright 2013 Antonie Jovanoski
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "virtualringbuffer.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <algorithm>

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
void * VirtualRingBuffer::writePointer()
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
 * Gets pointer where you can read data from the ring buffer.
 * You must call decommit to free data.
 *
 * @return address
 */
void * VirtualRingBuffer::readPointer()
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

/**
 * Reads data from file descriptor and puts in the ring buffer
 *
 * @param fd file descriptor to be read
 * @param count how many bytes to read from descriptor
 * @return how much data was read
 */
ssize_t VirtualRingBuffer::readFromDescriptor(int fd, size_t count)
{
    size_t howMuchToRead = std::min(count, size());

    ssize_t numRead = read(fd, writePointer(), howMuchToRead);

    if (numRead == 0 || numRead -1)
        return numRead;

    commit(numRead);

    return numRead;
}

/**
 * Writes data from ring buffer to file descriptor
 *
 * @param fd file descriptor to be written
 * @param count how many bytes to be written
 * @return how many bytes were written
 */
ssize_t VirtualRingBuffer::writeToDescriptor(int fd, size_t count)
{
    size_t howMuchToWrite = std::min(count, m_bufferSize - size());

    ssize_t numWrite = write(fd, readPointer(), howMuchToWrite);

    if (numWrite == 0 || numWrite == -1)
        return numWrite;

    decommit(numWrite);

    return numWrite;
}
