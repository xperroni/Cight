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

#include <cight/interest_map.hpp>
using clarus::List;
using cight::InterestMap;

#include <map>

InterestMap::InterestMap(Selector selector, const cv::Mat &bgr, int padding):
    regions(selector(bgr, padding))
{
    // Nothing to do.
}

List<cv::Mat> InterestMap::operator () (const List<cv::Mat> &images, int range) const {
    int rows = regions.size();
    int cols = images.size();

    cv::Mat similarities(rows, cols, CV_32F, cv::Scalar(0));
    cv::Mat responses(1, cols, CV_32F, cv::Scalar(0));
    for (int i = 0; i < rows; i++) {
        int index = 0;
        float highest = 0;
        for (int j = 0; j < cols; j++) {
            const InterestRegion &region = regions[i];
            float value = region(images[j], range);
            similarities.at<float>(i, j) = value;
            if (highest < value) {
                highest = value;
                index = j;
            }
        }

        responses.at<float>(0, index) += 1.0f;
    }

    List<cv::Mat> results;
    results.append(responses);
    results.append(similarities);

    return results;
}
