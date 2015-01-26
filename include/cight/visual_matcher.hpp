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

#include <cight/feature_map.hpp>
#include <cight/feature_selector.hpp>
#include <cight/interpolator.hpp>
#include <cight/memory.hpp>
#include <cight/sensor_stream.hpp>
#include <cight/settings.hpp>
#include <cight/stream_buffer.hpp>

#include <clarus/core/list.hpp>

namespace cight {
    struct StreamTeach;

    struct StreamReplay;

    struct SimilarityMap;

    struct VisualMatcher;
}

/**
\brief Teach step memory pipeline.
*/
struct cight::StreamTeach: public StreamBuffer {
    /** \brief Memory buffer for the teach stream edge maps. */
    clarus::List<cv::Mat> edges;

    /** \brief Additional padding to search for good matches. */
    int padding;

    /**
    \brief Default constructor.
    */
    StreamTeach();

    /**
    \brief Creates a new teach step memory pipeline.

    The pipeline is bound to the given input stream, and internal buffers store
    items up to the given size.
    */
    StreamTeach(SensorStream stream, size_t size, int padding);

    /**
    Discards the first item of each internal buffer.
    */
    virtual void pop();

    /**
    \brief Read a single frame from the input stream.

    After the frame is read, both the frame and edge map lists are updated. If the
    maximum buffer size is reached, the first item of each buffer is discarded.
    */
    virtual bool read();
};

/**
\brief Replay step memory pipeline.
*/
struct cight::StreamReplay: public StreamBuffer {
    /** \brief Memory buffer for replay stream feature maps. */
    clarus::List<FeatureMap> maps;

    /** \brief Memory buffer for replay stream shift maps. */
    clarus::List<cv::Mat> shifts;


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
    StreamReplay(SensorStream stream, size_t size, Selector selector, int padding);

    /**
    \brief Returns the similarities between the given replay image and the current contents of the teach buffer.
    */
    clarus::List<cv::Mat> operator () (int j, StreamTeach &teach);

    /**
    Discards the first item of each internal buffer.
    */
    virtual void pop();

    /**
    \brief Read a single frame from the input stream.

    After the frame is read, both the frame and edge map lists are updated. If the
    maximum buffer size is reached, the first item of each buffer is discarded.
    */
    virtual bool read();

    /**
    \brief Returns the shift vector between replay image \c i and teach image \c j.
    */
    cv::Mat shifted(int i, int j) const;
};

struct cight::SimilarityMap: public cv::Mat {
    /**
    \brief Default constructor.
    */
    SimilarityMap();

    /**
    \brief Creates a new similarity map of given dimensions.
    */
    SimilarityMap(const cv::Size &size);

    /**
    \brief Updates the similarity map with data from the given streams.
    */
    bool update(StreamTeach &teach, StreamReplay &replay);
};

struct cight::VisualMatcher {
    /** \brief Teach step memory pipeline. */
    StreamTeach teach;

    /** \brief Replay step memory pipeline. */
    StreamReplay replay;

    /** \brief Similarity map relating teach (columns) and replay (rows) frames. */
    SimilarityMap similarities;

    /** \brief Function used to interpolate a stream matching line over the current similarity map. */
    Interpolator interpolator;

    /** \brief Current matching line. */
    cv::Point3f line;

    /** \brief Index of the next matching to return. */
    int index;

    /**
    \brief Default constructor.
    */
    VisualMatcher();

    /**
    \brief Creates a new stream matcher.

    \param teach Teach (pre-recorded) video stream.

    \param replay Replay (real-time) video stream.

    \param window Size of the memory buffers for teach (height) and replay (width) streams.

    \param selector Function used to select interest regions.

    \param padding_a Padding around feature points, defining the area of patches.

    \param padding_b Additional padding for teach image patches.

    \param interpolator Function used to interpolate a stream matching line over the current similarity map.
    */
    VisualMatcher(
        SensorStream teach,
        SensorStream replay,
        const cv::Size &window,
        Selector selector,
        int padding_a,
        int padding_b,
        Interpolator interpolator
    );

    // See cight::StreamMatcher
    clarus::List<cv::Mat> operator() ();

    /**
    \brief Compute the matching trend between streams.
    */
    bool computeMatching();
};

#endif
