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

#ifndef CIGHT_SIMILARITY_MAP_HPP
#define CIGHT_SIMILARITY_MAP_HPP

#include <cight/stream_teach.hpp>
#include <cight/stream_replay.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    struct SimilarityMap;
}

struct cight::SimilarityMap: public cv::Mat {
    /**
    \brief Default constructor.
    */
    SimilarityMap();

    /**
    \brief Creates a new similarity map of given dimensions.
    */
    SimilarityMap(int rows, int cols);

    /**
    \brief Creates a new similarity map of given dimensions.
    */
    SimilarityMap(const cv::Size &size);

    /**
    \brief Updates the similarity map with data from the given streams.
    */
    bool update(StreamTeach &teach, StreamReplay &replay);
};

#endif
