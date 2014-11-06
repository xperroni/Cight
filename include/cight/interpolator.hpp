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

#ifndef CIGHT_INTERPOLATOR_HPP
#define CIGHT_INTERPOLATOR_HPP

#include <clarus/core/list.hpp>

#include <boost/function.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    /**
    \brief Type of functions that interpolate a match line over a similarity map.

    The line is returned as a pair of points.
    */
    typedef boost::function<clarus::List<cv::Point>(const cv::Mat&)> Interpolator;

    /**
    \brief Brute-force interpolator.
    */
    clarus::List<cv::Point> interpolateSlide(const cv::Mat &similarities);

    /**
    \brief Hough transform-based interpolator.
    */
    clarus::List<cv::Point> interpolateHough(const cv::Mat &similarities);
}

#endif
