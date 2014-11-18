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

#include <cight/interest_region.hpp>
using cight::InterestRegion;

#include <clarus/core/math.hpp>
#include <clarus/vision/fourier.hpp>

InterestRegion::InterestRegion():
    roi(0, 0, 0, 0),
    poi(0, 0),
    patch()
{
    // Nothing to do.
}

InterestRegion::InterestRegion(const cv::Mat &image, int x, int y, int w, int h):
    roi(x, y, w, h),
    poi(x + w / 2, y + h / 2),
    patch(image, roi)
{
    // Nothing to do.
}

inline cv::Rect regionOfInterest(const cv::Mat &image, int xf, int yf, int padding) {
    int side = 2 * padding + 1;
    int x = std::min(std::max(0, xf - padding), image.cols - side);
    int y = std::min(std::max(0, yf - padding), image.rows - side);
    return cv::Rect(x, y, side, side);
}

InterestRegion::InterestRegion(const cv::Mat &image, int x, int y, int padding):
    roi(regionOfInterest(image, x, y, padding)),
    poi(roi.x + roi.width / 2, roi.y + roi.height / 2),
    patch(image, roi)
{
    // Nothing to do.
}

float InterestRegion::operator () (const cv::Mat &image, int range) const {
    int w = roi.width + 2 * range;
    int h = roi.height + 2 * range;
    int x = std::min(std::max(0, roi.x - range), image.cols - w);
    int y = std::min(std::max(0, roi.y - range), image.rows - h);

    cv::Mat neighborhood(image, cv::Rect(x, y, w, h));
    cv::Mat responses = fourier::correlate(neighborhood, patch);

    return clarus::max(responses);
}

const cv::Rect &InterestRegion::bounds() const {
    return roi;
}

const cv::Point &InterestRegion::center() const {
    return poi;
}

bool InterestRegion::empty() const {
    return (cv::sum(patch)[0] == 0);
}
