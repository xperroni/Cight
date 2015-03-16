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

#ifndef CIGHT_DIFFERENCE_STREAM_HPP
#define CIGHT_DIFFERENCE_STREAM_HPP

#include <cight/stream_buffer.hpp>

#include <clarus/core/list.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    struct DifferenceStream;
}

/**
\brief A sensor stream that computes differences between successive inputs.

A differential stream is a form of stream buffer that tries to ensure successive
frames are different by at least a given amount. Frames that don't make the cut
are discarded.
*/
struct cight::DifferenceStream: public StreamBuffer {
    /** \brief Difference image buffer. */
    clarus::List<cv::Mat> diffs;

    /** \brief Indices of frames retrieved from the parent stream. */
    clarus::List<int> indices;

    /** \brief Minimal difference between frames. */
    double threshold;

    /**
    \brief Default constructor.
    */
    DifferenceStream();

    /**
    \brief Creates a new differential stream.

    The pipeline is bound to the given input stream, and internal buffers store
    items up to the given size.
    */
    DifferenceStream(SensorStream stream, size_t size, double threshold);

    /**
    \brief Returns the next difference image computed from the underlying sensor stream.

    If it's not possible to compute another difference image, an empty image is returned.
    */
    cv::Mat operator () ();

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
