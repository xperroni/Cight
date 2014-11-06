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

#ifndef CIGHT_VISUAL_MATCHER_HPP
#define CIGHT_VISUAL_MATCHER_HPP

#include <cight/interest_map.hpp>
#include <cight/interest_selector.hpp>
#include <cight/interpolator.hpp>
#include <cight/memory.hpp>
#include <cight/sensor_stream.hpp>
#include <cight/stream_matcher.hpp>

#include <clarus/core/list.hpp>

namespace cight {
    class VisualMatcher;
}

class cight::VisualMatcher: public StreamMatcher {
    /** \brief Teach (pre-recorded) video stream. */
    SensorStream::P teachStream;

    /** \brief Replay (real-time) video stream. */
    SensorStream::P replayStream;

    /** \brief Memory buffer for unadulterated teach stream frames. */
    clarus::List<cv::Mat> teachFrames;

    /** \brief Memory buffer for the teach stream edge maps. */
    clarus::List<cv::Mat> teachEdges;

    /** \brief Memory buffer for replay stream frames. */
    clarus::List<cv::Mat> replayFrames;

    /** \brief Memory buffer for replay stream interest maps. */
    clarus::List<InterestMap> replayMaps;

    /** \brief Function used to select interest regions. */
    Selector selector;

    /** \brief Padding for interest regions. */
    int padding;

    /** \brief Range around each interest region to search for good matches. */
    int searchRange;

    /** \brief Function used to interpolate a stream matching line over the current similarity map. */
    Interpolator interpolator;

    /** \brief Similarity map relating teach (columns) and replay (rows) frames. */
    cv::Mat similarities;

    /** \brief Index of the next matching to return. */
    int index;

    /** \brief First match for the current similarity matrix. */
    cv::Point p0;

    /** \brief Ratio used to compute matches after the first. */
    float tan;

    /** \brief Last matched replay frame. */
    int yn;

    /**
    \brief Read a frame from the teach stream.

    Lists containing teach frame data are updated, and older frames discarded as appropriate.
    */
    void readTeachStream();

    /**
    \brief Read a frame from the replay stream.

    Lists containing replay frame data are updated, and older frames discarded as appropriate.
    */
    void readReplayStream();

    /**
    \brief Update the similarity map.
    */
    void computeSimilarityMap();

public:
    /**
    \brief Creates a new stream matcher.

    \param teach Teach (pre-recorded) video stream.

    \param replay Replay (real-time) video stream.

    \param window Size of the memory buffers for teach (width) and replay (height) streams.

    \param interpolator Function used to interpolate a stream matching line over the current similarity map.

    \param selector Function used to select interest regions.

    \param padding Padding of interest regions.

    \param range Range around each interest region to search for good matches.
    */
    VisualMatcher(
        SensorStream::P teach,
        SensorStream::P replay,
        const cv::Size &window,
        Interpolator interpolator,
        Selector selector,
        int padding,
        int range
    );

    // See cight::StreamMatcher:: operator() ()
    clarus::List<cv::Mat> operator() ();

    // See cight::StreamMatcher::more()
    bool more() const;
};

#endif
