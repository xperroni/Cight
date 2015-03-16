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

#ifndef CIGHT_STREAM_TEACH_HPP
#define CIGHT_STREAM_TEACH_HPP

#include <cight/difference_stream.hpp>

namespace cight {
    struct StreamTeach;
}

/**
\brief Teach step memory pipeline.
*/
struct cight::StreamTeach: public DifferenceStream {
    /** \brief Additional padding to search for good matches. */
    int padding;

    /**
    \brief Default constructor.
    */
    StreamTeach();

    /**
    \brief Creates a new teach step memory pipeline.
    */
    StreamTeach(SensorStream stream, size_t size, double threshold, int padding);
};

#endif
