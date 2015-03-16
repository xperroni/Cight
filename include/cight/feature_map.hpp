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

#include <cight/feature_point.hpp>
#include <cight/feature_selector.hpp>
#include <cight/settings.hpp>

#include <clarus/core/list.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    class FeatureMap;
}

class cight::FeatureMap {
    /** \brief Feature points collected in this map. */
    clarus::List<FeaturePoint> features;

public:
    /**
    \brief Creates a new feature map of given configuration.

    The given selector is used to extract a number of feature points from the given
    image.
    */
    FeatureMap(Selector selector, const cv::Mat &bgr, int padding);

    /**
    \brief Creates a new feature map containing the given features.
    */
    FeatureMap(const clarus::List<FeaturePoint> &features);

    /**
    \brief Evaluates the similarity between this feature map and the given image list.

    Images are expected to be preprocessed in a format appropriate for feature point
    comparison. Currently this means a single-channel representation of any primitive
    type.

    Each feature point in this map is compared to each image in the given list, within
    the given padding around its original position (plus the padding defining its own
    associated patch). A similarity vector (actually a single-column matrix) and a more
    detailed similarity map are concurrently constructed and returned.

    The similarity vector is computed by counting, for each given image, how many feature
    points have yielded the best response: this will likely result in a sparse vector,
    as some images will collect several best matches while many will receive none.

    The similarity map stores the similarity values returned by each feature point
    for each given image. Each row represents a feature point, and each column, an
    evaluated image.
    */
    clarus::List<cv::Mat> operator () (const clarus::List<cv::Mat> &images, int padding, int i0 = 0, int n = 0) const;

    /**
    \brief Returns the number of feature points contained by this map.
    */
    size_t size() const;
};

#endif
