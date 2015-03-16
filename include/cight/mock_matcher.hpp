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

#ifndef CIGHT_MOCK_MATCHER_HPP
#define CIGHT_MOCK_MATCHER_HPP

#include <cight/stream_teach.hpp>
#include <cight/stream_replay.hpp>

#include <clarus/core/list.hpp>

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>

namespace cight {
    struct MockMatcher;
}

/**
\brief An image matcher that relies on a predefined data.
*/
struct cight::MockMatcher {
    /** \brief Difference stream from the teach step. */
    StreamTeach teach;

    /** \brief Difference stream from the replay step. */
    StreamReplay replay;

    /** \brief Text stream containing the image pairings. */
    boost::shared_ptr<std::ifstream> pairings;

    /** \brief Offset into the difference image range. */
    int offset;

    /**
    \brief Default constructor.
    */
    MockMatcher();

    /**
    \brief Creates a new mock stream matcher.
    */
    MockMatcher(const std::string &path, const StreamTeach &teach, const StreamReplay &replay);

    /**
    \brief Returns a pair of matched difference images.
    */
    clarus::List<cv::Mat> operator () ();
};

#endif
