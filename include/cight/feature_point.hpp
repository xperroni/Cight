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

#ifndef CIGHT_FEATURE_POINT_HPP
#define CIGHT_FEATURE_POINT_HPP

#include <opencv2/opencv.hpp>

namespace cight {
    struct FeaturePoint;
}

struct cight::FeaturePoint {
    /** \brief Bounds of the patch around the feature point. */
    cv::Rect bounds;

    /** \brief Feature point's coordinates. */
    cv::Point center;

    /** \brief Contents of the patch around the feature point. */
    cv::Mat patch;

    /** \brief The feature point's relevance value. */
    float strength;

    /**
    \brief Creates a new, blank feature point.
    */
    FeaturePoint();

    /**
    \brief Creates a new feature point at or close to the given coordinates.

    A patch of side <tt>2 * padding + 1</tt> is extracted from the given image and
    associated to the feature point. The feature point's coordinates may be moved towards
    the center of the image, if part of the patch would otherwise fall outside image
    borders.
    */
    FeaturePoint(int x, int y, const cv::Mat &image, int padding);

    FeaturePoint(const cv::KeyPoint &point, const cv::Mat &image, int padding);

    /**
    \brief Cross-correlates the teach and replay patches around this feature point.
    */
    cv::Mat operator () (const cv::Mat &image, int padding) const;
};

#endif
