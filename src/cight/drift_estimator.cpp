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

#include <cight/drift_estimator.hpp>
using cight::Estimator;

#include <cight/transforms.hpp>

#include <clarus/core/list.hpp>
using clarus::List;

#include <clarus/vision/colors.hpp>
#include <clarus/vision/fourier.hpp>
#include <clarus/vision/images.hpp>

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

inline cv::Mat preprocess(const cv::Mat &image) {
    //return colors::grayscale(cight::upper_half(image));
/*
    cv::Mat grad;
    cv::Sobel(cight::upper_half(image), grad, CV_8U, 1, 0, CV_SCHARR);
    return grad;
*/
    return cight::upper_half(image);
}

Estimator::Estimator(int _bins, int _window, size_t range, StreamMatcher _matcher):
    teach(range),
    replay(range),
    bins(_bins),
    window(_window),
    matcher(_matcher)
{
    // Nothing to do.
}

Estimator::~Estimator() {
    // Nothing to do.
}

cv::Mat Estimator::operator () () {
    //List<cv::Mat> matched = matcher();
    //return (matched.empty() ? cv::Mat() : matched[2]);

    do {
        List<cv::Mat> matched = matcher();
        if (matched.empty()) {
            return cv::Mat();
        }

        replay.record(preprocess(matched[0]));
        teach.record(preprocess(matched[1]));

        //displayMemory("Memory (Replay)", replay.last());
        //displayMemory("Memory (Teach)", teach.last());
    }
    while (teach.idle() > 0);

    cv::Mat teachMap = change_average(teach);
    cv::Mat replayMap = change_average(replay);

    displayDiVS(teachMap, replayMap);

    List<cv::Mat> matched = matcher();
    if (matched.empty()) {
        return cv::Mat();
    }

    cv::Mat teachVector = column_histogram(teachMap, bins);
    cv::Mat replayVector = column_histogram(replayMap, bins);

    int n = teachVector.cols;
    cv::Mat responses(1, n * 2, CV_64F, cv::Scalar::all(0));
    for (int i = 0, m = 1 + n - window; i < m; i++) {
        cv::Mat b(teachVector, cv::Rect(i, 0, window, 1));
        cv::Mat c = fourier::correlate(replayVector, b, false);
        cv::Mat r(responses, cv::Rect(n - i, 0, n, 1));
        r += c;
    }

    return responses;
}

void Estimator::reset() {
    teach.clear();
    replay.clear();
}
