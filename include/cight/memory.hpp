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

#ifndef CIGHT_MEMORY_HPP
#define CIGHT_MEMORY_HPP

#include <clarus/core/list.hpp>

#include <opencv2/opencv.hpp>

#include <string>

namespace cight {
    class Memory;
};

/*
Visual working memory. Stores a sequence of images up to a given range, discarding
older entries as newer ones are added.
*/
class cight::Memory: public clarus::List<cv::Mat> {
    /* Maximum image buffer size. */
    size_t range;

public:
    /*
    Creates a new eye buffer with set size.
    */
    Memory(size_t range);

    /*
    Virtual destructor. Ensures the class is compiled as polymorphic. Do not remove.
    */
    virtual ~Memory();

    /*
    Returns the number of records that can still be added to this memory before it
    starts scrapping older records.
    */
    size_t idle() const;

    /*
    Adds an image to the end of the buffer, If the buffer is already at its maximum
    size, the oldest image is discarded.
    */
    void record(const cv::Mat &image);
};

namespace cight {
    cv::Mat change_count(const Memory &memory);

    cv::Mat change_average(const Memory &memory);

    /*
    Returns the sum of absolute differences between each image in the buffer and the
    next.
    */
    cv::Mat drift_color(const Memory &memory);

    /*
    Returns the difference count between the Sobel edge maps of each image in the buffer
    and the next.
    */
    cv::Mat drift_edges(const Memory &memory);

    /*
    Returns the difference count between the Sobel edge maps of each image in the buffer
    and the next, averaged over the number of images in the working memory.
    */
    cv::Mat average_edges(const Memory &memory);
}

#endif
