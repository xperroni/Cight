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

#include <cight/feature_point.hpp>
using cight::FeaturePoint;

#include <clarus/core/math.hpp>
#include <clarus/vision/fourier.hpp>

FeaturePoint::FeaturePoint():
    roi(0, 0, 0, 0),
    poi(0, 0),
    patch()
{
    // Nothing to do.
}

inline cv::Rect regionOfInterest(int x, int y, const cv::Mat &image, int padding) {
    int side = 2 * padding + 1;
    int xf = std::min(std::max(0, x - padding), image.cols - side);
    int yf = std::min(std::max(0, y - padding), image.rows - side);
    return cv::Rect(xf, yf, side, side);
}

FeaturePoint::FeaturePoint(int x, int y, const cv::Mat &image, int padding):
    roi(regionOfInterest(x, y, image, padding)),
    poi(roi.x + roi.width / 2, roi.y + roi.height / 2),
    patch(image, roi)
{
    // Nothing to do.
}

float FeaturePoint::operator () (const cv::Mat &image, int padding) const {
    int w = roi.width + 2 * padding;
    int h = roi.height + 2 * padding;
    int x = std::min(std::max(0, roi.x - padding), image.cols - w);
    int y = std::min(std::max(0, roi.y - padding), image.rows - h);

    cv::Mat neighborhood(image, cv::Rect(x, y, w, h));
    cv::Mat responses = fourier::correlate(neighborhood, patch);

    return clarus::max(responses);
}

const cv::Rect &FeaturePoint::bounds() const {
    return roi;
}

const cv::Point &FeaturePoint::point() const {
    return poi;
}

bool FeaturePoint::empty() const {
    return (cv::sum(patch)[0] == 0);
}
