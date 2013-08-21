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

#include <stddef.h>

class RingBuffer
{
public:
    explicit RingBuffer(size_t capacity);
    ~RingBuffer();

    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }
    size_t write(const char *data, size_t size);
    size_t read(char *data, size_t size);

private:
    RingBuffer(const RingBuffer& );
    RingBuffer & operator=(const RingBuffer& );

    size_t m_begin;
    size_t m_end;
    size_t m_size;
    size_t m_capacity;

    char *m_data;
};
