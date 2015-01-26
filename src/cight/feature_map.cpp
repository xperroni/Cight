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

#include <clarus/core/math.hpp>

#include <map>

#ifdef DIAGNOSTICS
    #include <clarus/core/types.hpp>
    #include <clarus/io/viewer.hpp>
    #include <clarus/vision/colors.hpp>
    #include <clarus/vision/filters.hpp>
    #include <clarus/vision/images.hpp>
    #include <iostream>

    static void display(const cv::Mat &bgr, const List<cight::FeaturePoint> &regions) {
        static cv::Scalar RED(0, 0, 255);

        static int index = 0;

        cv::Mat canvas = colors::convert(filter::sobel(colors::grayscale(bgr)), CV_GRAY2BGR);
        for (int i = 0, n = regions.size(); i < n; i++) {
            cv::rectangle(canvas, regions[i].bounds(), RED);
        }

        images::save(canvas, "regions-" + types::to_string(index++) + ".png");
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

inline void update_shifts(cv::Mat &shifts, int i, int rows, cv::Mat &responses) {
    if (shifts.empty()) {
        shifts = cv::Mat(rows, responses.cols, responses.type(), cv::Scalar(0));
    }

    cv::Mat reduced;
    reduced = responses - clarus::min(responses);
    //viewer::plot("Responses", reduced);

    cv::reduce(reduced, reduced, 0, CV_REDUCE_MAX);
    cv::flip(reduced, reduced, 1);

    cv::Mat row(shifts, cv::Rect(0, i, reduced.cols, 1));
    row += reduced - clarus::min(reduced);
}
/*
inline void update_shifts(cv::Mat &shifts, int i, int rows, cv::Mat &responses) {
    int cols = responses.cols;
    if (shifts.empty()) {
        shifts = cv::Mat(rows, cols, responses.type(), cv::Scalar(0));
    }

    cv::Mat reduced(1, cols, CV_64F, cv::Scalar(0));
    for (int j = 0, n = responses.rows; j < n; j++) {
        cv::Mat row(responses, cv::Rect(0, j, cols, 1));
        cv::Point point = clarus::argmax(row);
        reduced.at<double>(0, point.x) += 1.0;
    }
    cv::flip(reduced, reduced, 1);

    cv::Mat row(shifts, cv::Rect(0, i, cols, 1));
    row += reduced;
}

inline void update_shifts(cv::Mat &shifts, int i, int rows, cv::Mat &responses) {
    //viewer::plot("Responses", responses);

    int cols = responses.cols;
    if (shifts.empty()) {
        shifts = cv::Mat(rows, cols, responses.type(), cv::Scalar(0));
    }

    cv::Point point = clarus::argmax(responses);
    cv::Mat maxed(responses, cv::Rect(0, point.y, cols, 1));

    maxed = maxed.clone();
    cv::flip(maxed, maxed, 1);
    maxed -= clarus::min(maxed);

    cv::Mat row(shifts, cv::Rect(0, i, cols, 1));
    row += maxed;
}
*/
List<cv::Mat> FeatureMap::operator () (const List<cv::Mat> &images, int padding, int i0, int n) const {
    int rows = images.size();
    int cols = features.size();
    if (n == 0) {
        n = rows;
    }

    cv::Mat similarities(rows, cols, CV_32F, cv::Scalar(0));
    cv::Mat maxima(1, cols, CV_32S, cv::Scalar(0));
    //cv::Mat shifts;

    for (int i = i0; i < n; i++) {
        for (int j = 0; j < cols; j++) {
            const cv::Mat &image = images.at(i);
            const FeaturePoint &point = features[j];
            cv::Mat responses = point(image, padding);
            //update_shifts(shifts, i, rows, responses);
            float value = clarus::max(responses);
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

    return (List<cv::Mat>(), responses, similarities);
}
