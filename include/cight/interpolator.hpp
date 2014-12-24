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

#include <boost/function.hpp>

#include <opencv2/opencv.hpp>

namespace cight {
    /**
    \brief Type of functions that interpolate a match line over a similarity map.

    The line is returned as a 3D point <tt>(x0, y0, t)</tt>, where
    <tt>y = (x - x0)t + y0 &forall; x &ge; x0</tt> .
    */
    typedef boost::function<cv::Point3f(const cv::Mat&)> Interpolator;

    /**
    \brief Returns the first point for the given line such that <tt>x &ge; 0</tt> and <tt>y &ge; 0</tt> .
    */
    cv::Point lineP0(float x, float y, float t);

    /**
    \brief Returns the first point for the given line such that <tt>x &ge; 0</tt> and <tt>y &ge; 0</tt> .
    */
    cv::Point lineP0(const cv::Point3f &line);

    /**
    \brief Returns the last point for the given line that falls within the given size.
    */
    cv::Point linePn(float x, float y, float t, const cv::Size &size);

    /**
    \brief Returns the last point for the given line that falls within the given size.
    */
    cv::Point linePn(const cv::Point3f &line, const cv::Size &size);

    /**
    \brief Brute-force interpolator.
    */
    cv::Point3f interpolateSlide(const cv::Mat &similarities);

    /**
    \brief Hough transform-based interpolator.
    */
    cv::Point3f interpolateHough(const cv::Mat &similarities);
}

#endif
