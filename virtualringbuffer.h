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

#ifndef VIRTUALRINGBUFFER_H
#define VIRTUALRINGBUFFER_H

#include <stdlib.h>

/**
 * Implementation of ring buffer with virtual mapping trick
 */
class VirtualRingBuffer
{
public:
    VirtualRingBuffer(size_t capacity);
    virtual ~VirtualRingBuffer();

    size_t capacity() const { return m_bufferSize; }

    void *writePointer();
    void commit(size_t count);

    void *readPointer();
    void decommit(size_t count);

    ssize_t readFromDescriptor(int fd, size_t count);
    ssize_t writeToDescriptor(int fd, size_t count);

    size_t size() const;

private:
    char *m_buffer;
    size_t m_bufferSize;
    int m_writeOffset;
    int m_readOffset;
};

#endif //VIRTUALRINGBUFFER_H
