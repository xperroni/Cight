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

#include <cight/interpolator.hpp>
using clarus::List;

#include <clarus/core/math.hpp>
#include <clarus/vision/images.hpp>

static List<float> bestSlide(const cv::Mat &costs) {
    int rows = costs.rows;
    int cols = costs.cols;
    float yn = rows;

    float a = 0.0;
    float b = cols - 1;
    float d = FLT_MAX;
    float n = std::min(rows, cols);
    float x1n = n / 3;
    float x2n = x1n * 2;
    for (float x2 = std::min(n + x1n, (float) cols - 1); x2 >= x2n; x2--) {
        for (float x1 = 0; x1 < x1n; x1++) {
            float tan = (x2 - x1) / yn;
            float e = 0;
            for (float i = 0; i < yn && e < d; i++) {
                float j = x1 + tan * i;
                e += costs.at<float>(i, j);
            }

            if (e < d) {
                d = e;
                a = x1;
                b = x2;
            }
        }
    }

    List<float> results;
    results.append(a);
    results.append(b);
    results.append(d);
    return results;
}

List<cv::Point> cight::interpolateSlide(const cv::Mat &similarities) {
    cv::Mat costs = clarus::max(similarities) - similarities;
    List<float> bestX = bestSlide(costs);
    List<float> bestY = bestSlide(costs.t());

    List<cv::Point> points;
    if (bestX[2] < bestY[2]) {
        int yn = similarities.rows - 1;
        points.append(cv::Point(bestX[0], 0));
        points.append(cv::Point(bestX[1], yn));
    }
    else {
        int xn = similarities.cols;
        points.append(cv::Point(0, bestY[0]));
        points.append(cv::Point(xn, bestY[1]));
    }

    return points;
}

List<cv::Point> cight::interpolateHough(const cv::Mat &similarities) {
    int rows = similarities.rows;
    int cols = similarities.cols;

    List<cv::Vec4i> lines;
    cv::Mat votes = images::convert(similarities, CV_8U);
    int minLineLength = sqrt(pow(rows * 0.5, 2.0) + pow(cols * 0.5, 2.0));
    cv::HoughLinesP(votes, *lines, 1, CV_PI / 180.0, 50, minLineLength, 10);

    cv::Point p0;
    cv::Point pn;
    float best = 0;
    for (int i = 0, n = lines.size(); i < n; i++) {
        const cv::Vec4i &line = lines[i];
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];

        if (x2 - x1 <= 0 || y2 - y1 <= 0) {
            continue;
        }

        cv::Point a(x1, y1);
        cv::Point b(x2, y2);
        cv::Mat mask(rows, cols, CV_8U, cv::Scalar(0));
        cv::line(mask, a, b, cv::Scalar(255));

        cv::Mat selected(rows, cols, CV_32F, cv::Scalar(0));
        similarities.copyTo(selected, mask);
        float total = cv::sum(selected)[0];
        if (total > best) {
            best = total;
            p0 = a;
            pn = b;
        }
    }

    List<cv::Point> points;
    points.append(p0);
    points.append(pn);
    return points;
}
