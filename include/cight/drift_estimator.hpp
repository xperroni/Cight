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

#ifndef CIGHT_DRIFT_ESTIMATOR_H
#define CIGHT_DRIFT_ESTIMATOR_H

#include <cight/memory.hpp>
#include <cight/stream_matcher.hpp>
#include <cight/settings.hpp>

#include <clarus/core/list.hpp>

#include <string>

namespace cight {
    class Estimator;
}

class cight::Estimator {
    Memory teach;

    Memory replay;

    StreamMatcher matcher;

    int bins;

    int window;

public:
    /*
    Creates a new drift estimator, using the giving folder for storage.
    */
    Estimator(int bins, int window, size_t range, StreamMatcher matcher);

    /*
    Virtual destructor. Enforces polymorphism. Do not remove.
    */
    virtual ~Estimator();

    /*
    Calculates a vector of drift estimate likelihoods.
    */
    cv::Mat operator () ();

    /*
    Erases the estimator's memory, forcing it to rebuild its image base on the next
    call.
    */
    void reset();
};

#endif
