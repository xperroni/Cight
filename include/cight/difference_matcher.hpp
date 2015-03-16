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

#ifndef CIGHT_DIFFERENCE_MATCHER_HPP
#define CIGHT_DIFFERENCE_MATCHER_HPP

#include <cight/interpolator.hpp>
#include <cight/stream_teach.hpp>
#include <cight/stream_replay.hpp>
#include <cight/similarity_map.hpp>

#include <clarus/core/list.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    struct DifferenceMatcher;
}

/**
\brief A sensor stream that computes differences between successive inputs.

A differential stream is a form of stream buffer that tries to ensure successive
frames are different by at least a given amount. Frames that don't make the cut
are discarded.
*/
struct cight::DifferenceMatcher {
    /** \brief Function used to interpolate a stream matching line over the current similarity map. */
    Interpolator interpolator;

    /** \brief Difference stream from the teach step. */
    StreamTeach teach;

    /** \brief Difference stream from the replay step. */
    StreamReplay replay;

    /** \brief Similarity map relating teach (columns) and replay (rows) frames. */
    SimilarityMap similarities;

    /** \brief Current matching line. */
    cv::Point3f line;

    /** \brief Index of the next matching to return. */
    int index;

    /**
    \brief Default constructor.
    */
    DifferenceMatcher();

    /**
    \brief Creates a new differential stream.

    The pipeline is bound to the given input stream, and internal buffers store
    items up to the given size.
    */
    DifferenceMatcher(const Interpolator &interpolator, const StreamTeach &teach, const StreamReplay &replay);

    /**
    \brief Returns a pair of matched difference images.
    */
    clarus::List<cv::Mat> operator () ();

    /**
    \brief Compute the matching trend between streams.
    */
    bool computeMatching();
};

#endif
