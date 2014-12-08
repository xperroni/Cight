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

#ifndef CIGHT_STREAM_MATCHER_HPP
#define CIGHT_STREAM_MATCHER_HPP

#include <clarus/core/list.hpp>

#include <boost/smart_ptr.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    struct StreamMatcher;
}

struct cight::StreamMatcher {
    /** \brief Reference-counted smart pointer type. */
    struct P: boost::shared_ptr<StreamMatcher> {
        P():
            boost::shared_ptr<StreamMatcher>()
        {
            // Nothing to do.
        }

        P(StreamMatcher *p):
            boost::shared_ptr<StreamMatcher>(p)
        {
            // Nothing to do.
        }

        clarus::List<cv::Mat> operator () () {
            StreamMatcher &matcher = *get();
            return matcher();
        }
    };

    /**
    \brief Returns a pair of matched frames from the teach (first) and replay (second) streams.
    */
    virtual clarus::List<cv::Mat> operator () () = 0;

    /**
    \brief Collect data from the teach stream.

    Stream matchers should implement this method so that it performs any required buffer
    initialization tasks related to the teach stream (e.g. loading a first batch of
    images). This enables user code to intersperse setup tasks with their own required
    initial operations.
    */
    virtual void fillTeachBuffer() = 0;

    /**
    \brief Collect data from the replay stream.

    Stream matchers should implement this method so that it performs any required buffer
    initialization tasks related to the replay stream (e.g. loading a first batch of
    images). This enables user code to intersperse setup tasks with their own required
    initial operations.
    */
    virtual void fillReplayBuffer() = 0;

    /**
    \brief Compute an initial matching between streams.

    Stream matchers should implement this method so that it computes any required parameters
    for matching the streams. It should rely on \c fillBuffers() for the initialization
    of any required buffers, calling it automatically if it hadn't yet been done manually.
    This enables user code to intersperse setup tasks with their own required initial
    operations.

    After the first call, implementations should automatically update computed parameters
    as required.
    */
    virtual void computeMatching() = 0;

    /**
    \brief Returns whether the stream matcher is still active.
    */
    virtual bool more() const = 0;
};

#endif
