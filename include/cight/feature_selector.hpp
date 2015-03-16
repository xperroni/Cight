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

#include <boost/bind.hpp>
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
    \brief Returns a non-overlapping subset of feature points selected by an upstream selector.
    */
    clarus::List<FeaturePoint> selectDisjoint(Selector selector, const cv::Mat &bgr, int padding);

    /**
    \brief Select feature points that are above the image's average intensity.

    The image is assumed to be of type <tt>CV_8UC1</tt>.
    */
    clarus::List<FeaturePoint> selectAboveMean(const cv::Mat &image, int padding);

    /**
    \brief Select feature points based on the difference between a given image and a previous one.

    When using this selector, bind it to an empty cv::Mat object, e.g.:

    Selector selector = boost::bind(selectDifference, boost::ref(cv::Mat()), 20.0, _1, _2);

    The first call will result on an empty list, but the following ones will produce feature points
    based on the differences between successive inputs. (You could of course also bind it to the
    first image in a sequence and start feeding the selector the second image onwards, in which case
    the first output won't necessarily be empty.)
    */
    clarus::List<FeaturePoint> selectDifference(cv::Mat &previous, float t, const cv::Mat &image, int padding);

    /**
    \brief Select feature points based on FAST features.
    */
    clarus::List<FeaturePoint> selectFAST(cv::FastFeatureDetector &detector, const cv::Mat &bgr, int padding);

    /**
    \brief Select feature points based on strong corners.
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
