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

#ifndef CIGHT_INTEREST_MAP_HPP
#define CIGHT_INTEREST_MAP_HPP

#include <cight/interest_region.hpp>
#include <cight/interest_selector.hpp>

#include <clarus/core/list.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    class InterestMap;
}

class cight::InterestMap {
    /** \brief Interest regions collected in this map. */
    clarus::List<InterestRegion> regions;

public:
    /**
    \brief Creates a new interest map of given configuration.

    The given selector is used to extract a number of interest regions from the given
    image. Interest regions are square patches of side (2 * padding + 1).
    */
    InterestMap(Selector selector, const cv::Mat &bgr, int padding);

    /**
    \brief Evaluates the similarity between this interest map and the given image list.

    Images are expected to be preprocessed in a format appropriate for interest region
    comparison. Currently this means a single-channel representation of any primitive
    type.

    Each interest region in this map is compared to each image in the given list, within
    the given range of its original position. A similarity vector (actually a single-row
    matrix) and a more detailed similarity map are concurrently constructed and returned.

    The similarity vector is computed by counting, for each given image, how many regions
    have yielded the best response: this will likely result in a sparse vector, as some
    images will collect several best matches while many will receive none.

    The similarity map stores the similarity values returned by each interest region
    for each given image. Each row represents an interest region, and each column, an
    evaluated image.
    */
    clarus::List<cv::Mat> operator () (const clarus::List<cv::Mat> &images, int range) const;
};

#endif
