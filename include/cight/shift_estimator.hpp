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

#ifndef CIGHT_SHIFT_ESTIMATOR_HPP
#define CIGHT_SHIFT_ESTIMATOR_HPP

#include <cight/stream_matcher.hpp>
#include <cight/settings.hpp>

#include <string>

namespace cight {
    struct ShiftEstimator;
}

struct cight::ShiftEstimator {
    /** \brief Number of bins used to compute the difference image histograms. */
    int bins;

    /** \brief Width of the sliding window used in the shift vector computation. */
    int width;

    /** \brief Matcher used to synchronize the two difference streams. */
    StreamMatcher matcher;

    /**
    \brief Creates a new shift estimator bound to the given stream matcher and algorithm parameters.
    */
    ShiftEstimator(int bins, int width, StreamMatcher matcher);

    /**
    \brief Virtual destructor. Enforces polymorphism. Do not remove.
    */
    virtual ~ShiftEstimator();

    /**
    \brief Calculates a shift likelihood vector.
    */
    cv::Mat operator () ();
};

#endif
