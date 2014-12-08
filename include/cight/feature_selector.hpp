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

#ifndef CIGHT_INTEREST_SELECTOR
#define CIGHT_INTEREST_SELECTOR

#include <cight/feature_point.hpp>

#include <clarus/core/list.hpp>

#include <boost/function.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    /**
    \brief Type of functions used to select interest regions in an input image.

    Selector functions receive a BGR image and padding length as input, and output a
    list of square interest regions of side <tt>2 * padding + 1</tt>.
    */
    typedef boost::function<clarus::List<FeaturePoint>(const cv::Mat&, int padding)> Selector;

    /**
    \brief Enforces an upper limit on the number of returned interest regions.

    If the number of interest regions returned by the given selector is greater than
    \c limit, excess regions are removed from the end of the list to make it fit.
    */
    clarus::List<FeaturePoint> selectAtMost(Selector selector, int limit, const cv::Mat &bgr, int padding);

    /**
    \brief Filters selected interest regions by distance from the image center.

    Given a list \c regions of interest regions returned by the given selector, the
    <tt>(int) (regions.size() * ratio)</tt> interest regions most distant from the
    image's center are selected and returned.
    */
    clarus::List<FeaturePoint> selectBorders(Selector selector, float ratio, const cv::Mat &bgr, int padding);

    /**
    \brief Select interest regions based on strong corners.
    */
    clarus::List<FeaturePoint> selectGoodFeatures(
        cv::GoodFeaturesToTrackDetector &detector,
        const cv::Mat &bgr,
        int padding
    );

    /**
    \brief Select interest regions based on saturation (the S channel of the HLS color
           space) outliers over the image.
    */
    clarus::List<FeaturePoint> selectSaturation(const cv::Mat &bgr, int padding);
}

#endif
