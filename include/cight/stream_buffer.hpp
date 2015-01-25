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

#ifndef CIGHT_STREAM_BUFFER_HPP
#define CIGHT_STREAM_BUFFER_HPP

#include <cight/sensor_stream.hpp>

#include <clarus/core/list.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    struct StreamBuffer;
}

struct cight::StreamBuffer {
    /** \brief Buffered stream. */
    SensorStream stream;

    /** Stream frame buffer. */
    clarus::List<cv::Mat> frames;

    /** Maximum size of the internal buffers. */
    size_t size;

    /**
    \brief Default constructor.
    */
    StreamBuffer();

    /**
    \brief Creates a new teach step memory pipeline.

    The pipeline is bound to the given input stream, and internal buffers store
    items up to the given size.
    */
    StreamBuffer(SensorStream stream, size_t size);

    /**
    \brief Discards the buffer's first item.
    */
    virtual void pop();

    /**
    \brief Read a single frame from the input stream.

    If the maximum buffer size is reached, the buffer's first item is discarded.
    */
    virtual bool read();
};

#endif
