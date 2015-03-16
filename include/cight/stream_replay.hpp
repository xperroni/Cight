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

#ifndef CIGHT_STREAM_REPLAY_HPP
#define CIGHT_STREAM_REPLAY_HPP

#include <cight/difference_stream.hpp>
#include <cight/feature_map.hpp>
#include <cight/feature_selector.hpp>
#include <cight/stream_teach.hpp>

namespace cight {
    struct StreamReplay;
}

/**
\brief Replay step memory pipeline.
*/
struct cight::StreamReplay: public DifferenceStream {
    /** \brief Memory buffer for replay stream feature maps. */
    clarus::List<FeatureMap> features;

    /** \brief Function used to select interest regions. */
    Selector selector;

    /** \brief Padding for feature points. */
    int padding;

    /**
    \brief Default constructor.
    */
    StreamReplay();

    /**
    \brief Creates a new replay step memory pipeline.

    The pipeline is bound to the given input stream, and internal buffers store
    items up to the given size.
    */
    StreamReplay(SensorStream stream, size_t size, double threshold, Selector selector, int padding);

    // See cight::DifferenceStream::operator () ()
    cv::Mat operator () ();

    /**
    \brief Returns the similarities between the given replay image and the current contents of the teach buffer.
    */
    clarus::List<cv::Mat> operator () (int j, StreamTeach &teach);

    virtual void pop();

    virtual bool read();
};

#endif
