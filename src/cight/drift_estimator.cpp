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

inline cv::Mat preprocess(const cv::Mat &image) {
    return colors::grayscale(cight::upper_half(image));
}

Estimator::Estimator(int _bins, int _window, size_t range, StreamMatcher::P _matcher):
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
    do {
        if (!more()) {
            break;
        }

        List<cv::Mat> matched = matcher();
        teach.record(preprocess(matched[0]));
        replay.record(preprocess(matched[1]));
    }
    while (teach.idle() > 0);

    cv::Mat teachVector = column_histogram(change_average(teach), bins);
    cv::Mat replayVector = column_histogram(change_average(replay), bins);

    int n = teachVector.cols;
    cv::Mat responses(1, n * 2, CV_64F, cv::Scalar::all(0));
    for (int i = 0, m = 1 + n - window; i < m; i++) {
        cv::Mat b(teachVector, cv::Rect(i, 0, window, 1));
        cv::Mat c = fourier::correlate(replayVector, b);
        cv::Mat r(responses, cv::Rect(n - i, 0, n, 1));
        r += c;
    }

    return responses;
}

void Estimator::reset() {
    teach.clear();
    replay.clear();
}

bool Estimator::more() const {
    return matcher->more();
}
