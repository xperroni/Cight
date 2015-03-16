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

#include <clarus/vision/fourier.hpp>

FeaturePoint::FeaturePoint():
    bounds(0, 0, 0, 0),
    center(0, 0),
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

inline float standardDeviation(const cv::Mat &patch) {
    cv::Mat mean, stddev;
    cv::meanStdDev(patch, mean, stddev);
    return stddev.at<double>(0, 0);
}

FeaturePoint::FeaturePoint(int x, int y, const cv::Mat &image, int padding):
    bounds(regionOfInterest(x, y, image, padding)),
    center(bounds.x + bounds.width / 2, bounds.y + bounds.height / 2),
    patch(image, bounds),
    strength(standardDeviation(patch))
{
    // Nothing to do.
}

FeaturePoint::FeaturePoint(const cv::KeyPoint &point, const cv::Mat &image, int padding):
    bounds(regionOfInterest(point.pt.x, point.pt.y, image, padding)),
    center(bounds.x + bounds.width / 2, bounds.y + bounds.height / 2),
    patch(image, bounds),
    strength(point.response)
{
    // Nothing to do.
}

cv::Mat FeaturePoint::operator () (const cv::Mat &image, int padding) const {
    int w = bounds.width + 2 * padding;
    int h = bounds.height + 2 * padding;
    int x = std::min(std::max(0, bounds.x - padding), image.cols - w);
    int y = std::min(std::max(0, bounds.y - padding), image.rows - h);

    cv::Mat neighborhood(image, cv::Rect(x, y, w, h));
    return fourier::correlate(neighborhood, patch);
}
