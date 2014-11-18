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

#ifndef CIGHT_INTEREST_REGION_HPP
#define CIGHT_INTEREST_REGION_HPP

#include <opencv2/opencv.hpp>

namespace cight {
    class InterestRegion;
}

class cight::InterestRegion {
    /** \brief Original view field coordinates from which this region was extracted. */
    cv::Rect roi;

    /** \brief Center of the interest region. */
    cv::Point poi;

    /** Contents of the interest region. */
    cv::Mat patch;

public:
    /**
    \brief Creates a new, empty interest region.
    */
    InterestRegion();

    /**
    \brief Creates a new interest region from the region defined by the given coordinates.

    \param image Image from which the contents of the interest region will be extracted.

    \param x Horizontal coordinate of the interest region's top-left corner.

    \param y Vertical coordinate of the interest region's top-left corner.

    \param w Width of the interest region.

    \param h Height of the interest region.
    */
    InterestRegion(const cv::Mat &image, int x, int y, int w, int h);

    /**
    \brief Creates a new interest region from the region defined by the given coordinates and padding.

    The region's center may be moved towards the center of the image, if part of it
    would otherwise fall outside image borders.

    \param image Image from which the contents of the interest region will be extracted.

    \param x Horizontal coordinate of the interest region's center.

    \param y Vertical coordinate of the interest region's center.

    \param padding Padding around the region's center.
    */
    InterestRegion(const cv::Mat &image, int x, int y, int padding);

    /**
    \brief Evaluates the similarity between this interest region an the given image.

    Returns the maximum similarity response between this interest regions and the given
    image, within the given range of the interest region's original coordinates.
    */
    float operator () (const cv::Mat &image, int range) const;

    /**
    \brief Returns the boundaries of the interest region.
    */
    const cv::Rect &bounds() const;

    /**
    \brief Returns the coordinates for the center of the interest region.
    */
    const cv::Point &center() const;

    /**
    \brief Returns whether all pixels within the interest region have zero values.
    */
    bool empty() const;
};

#endif
