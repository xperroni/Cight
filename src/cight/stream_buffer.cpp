/*
Copyright (c) Helio Perroni Filho <xperroni@gmail.com>

This file is part of Cight.

Cight is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Cight is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cight. If not, see <http://www.gnu.org/licenses/>.
*/

#include <cight/stream_buffer.hpp>
using cight::StreamBuffer;

StreamBuffer::StreamBuffer() {
    size = 0;
}

StreamBuffer::StreamBuffer(SensorStream _stream, size_t _size):
    stream(_stream),
    size(_size)
{
    // Nothing to do.
}

void StreamBuffer::pop() {
    frames.remove(0);
}

bool StreamBuffer::read() {
    cv::Mat frame = stream();
    if (frame.empty()) {
        return false;
    }

    frames.append(frame);
    if (frames.size() > size) {
        pop();
    }

    return true;
}
