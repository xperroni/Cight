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

#include <cight/feature_map.hpp>
using clarus::List;
using cight::FeatureMap;

#include <map>

#ifdef DIAGNOSTICS
    #include <clarus/io/viewer.hpp>
    #include <iostream>

    static void display(const cv::Mat &bgr, const List<cight::FeaturePoint> &regions) {
        static cv::Scalar RED(0, 0, 255);

        cv::Mat canvas = bgr.clone();
        for (int i = 0, n = regions.size(); i < n; i++) {
            cv::rectangle(canvas, regions[i].bounds(), RED);
        }

        viewer::show("Interest Regions", canvas);
        cv::waitKey(WAIT_KEY_MS);
    }
#else
    #define display(A, B)
#endif

FeatureMap::FeatureMap(Selector selector, const cv::Mat &bgr, int padding):
    features(selector(bgr, padding))
{
    display(bgr, features);
}

List<cv::Mat> FeatureMap::operator () (const List<cv::Mat> &images, int padding, int i0, int n) const {
    int rows = images.size();
    int cols = features.size();
    if (n == 0) {
        n = rows;
    }

    cv::Mat similarities(rows, cols, CV_32F, cv::Scalar(0));
    cv::Mat maxima(1, cols, CV_32S, cv::Scalar(0));

    for (int i = i0; i < n; i++) {
        for (int j = 0; j < cols; j++) {
            const FeaturePoint &point = features[j];
            float value = point(images[i], padding);
            similarities.at<float>(i, j) = value;

            int l = maxima.at<int>(0, j);
            if (similarities.at<float>(l, j) < value) {
                maxima.at<int>(0, j) = i;
            }
        }
    }

    cv::Mat responses(rows, 1, CV_32F, cv::Scalar(0));
    for (int j = 0; j < cols; j++) {
        int index = maxima.at<int>(0, j);
        responses.at<float>(index, 0) += 1.0f;
    }

    List<cv::Mat> results;
    results.append(responses);
    results.append(similarities);

    return results;
}
