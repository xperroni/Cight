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

#include <cight/shift_estimator.hpp>
using cight::ShiftEstimator;

#include <clarus/core/list.hpp>
using clarus::List;

#include <clarus/vision/fourier.hpp>

#ifdef DIAGNOSTICS
    #include <clarus/core/types.hpp>
    #include <clarus/io/viewer.hpp>
    #include <clarus/vision/colors.hpp>
    #include <clarus/vision/depths.hpp>
    #include <clarus/vision/filters.hpp>
    #include <clarus/vision/images.hpp>
    #include <iostream>

    static void displayDiVS(const cv::Mat &teach, cv::Mat &replay) {
        static cv::Scalar RED(0, 0, 255);

        static int index = 0;

        cv::Mat bgr_teach = depths::bgr(images::convert(teach, CV_32F));
        cv::Mat bgr_replay = depths::bgr(images::convert(replay, CV_32F));
        images::save(bgr_teach, "divs-teach-" + types::to_string(index) + ".png");
        images::save(bgr_replay, "divs-replay-" + types::to_string(index++) + ".png");
        viewer::show("DiVS (Teach)", bgr_teach);
        viewer::show("DiVS (Replay)", bgr_replay);
        cv::waitKey(WAIT_KEY_MS);
    }

    static void displayMemory(const std::string &title, const cv::Mat &image) {
        viewer::show(title, image);
        cv::waitKey(WAIT_KEY_MS);
    }
#else
    #define displayDiVS(A, B)

    #define displayMemory(A, B)
#endif

static cv::Mat columnHistogram(const cv::Mat &data, size_t bins) {
    size_t cols = data.cols / bins;
    size_t rows = data.rows;
    cv::Mat totals(1, bins, CV_32F, cv::Scalar::all(0));
    for (size_t i = 0; i < bins; i++) {
        cv::Rect bounds(cols * i, 0, cols, rows);
        cv::Mat section(data, bounds);
        totals.at<float>(0, i) = cv::sum(section)[0];
    }

    return totals;
}

static cv::Mat upperHalf(const cv::Mat &image) {
    cv::Rect roi(0, 0, image.cols, image.rows / 2);
    cv::Mat half(image, roi);
    return half;
}

ShiftEstimator::ShiftEstimator(int _bins, int _width, StreamMatcher _matcher):
    bins(_bins),
    width(_width),
    matcher(_matcher)
{
    // Nothing to do.
}

ShiftEstimator::~ShiftEstimator() {
    // Nothing to do.
}

cv::Mat ShiftEstimator::operator () () {
    List<cv::Mat> matched = matcher();
    if (matched.empty()) {
        return cv::Mat();
    }

    displayDiVS(matched[0], matched[1]);

    cv::Mat replayMap = upperHalf(matched[0]);
    cv::Mat teachMap = upperHalf(matched[1]);

    //cv::Mat teachVector = columnHistogram(teachMap, bins);
    //cv::Mat replayVector = columnHistogram(replayMap, bins);

    int rows = teachMap.rows;
    int cols = teachMap.cols;

    int window = width * (cols / bins);

    int slits = cols - window;

    cv::Mat responses(1, slits * 2, CV_64F, cv::Scalar::all(0));
    for (int i = 0; i <= slits; i+= window) {
        cv::Mat slice(teachMap, cv::Rect(i, 0, window, rows));
        cv::Mat correlated = fourier::correlate(replayMap, slice);
        cv::Mat shifts(correlated, cv::Rect(0, 0, slits, 1));

        cv::Mat range(responses, cv::Rect(slits - i, 0, slits, 1));
        range += shifts;
    }

    return responses(cv::Rect(slits / 2, 0, slits, 1));
}
