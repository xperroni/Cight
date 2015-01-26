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

#include <clarus/core/list.hpp>
using clarus::List;

#include <clarus/core/math.hpp>
#include <clarus/vision/images.hpp>

cv::Point cight::lineP0(float x, float y, float t) {
    float yd = y - t * x;
    if (yd >= 0) {
        return cv::Point(0, yd);
    }
    else {
        return cv::Point(-yd / t, 0);
    }
}

cv::Point cight::lineP0(const cv::Point3f &line) {
    return lineP0(line.x, line.y, line.z);
}

cv::Point cight::linePn(float x, float y, float t, const cv::Size &size) {
    float xn = size.width - 1;
    float yn = size.height - 1;
    float yd = yn - y;
    float xd = x + yd / t;
    if (xd <= xn) {
        return cv::Point(xd, yn);
    }
    else {
        return cv::Point(xn, y + (xn - x) * t);
    }
}

cv::Point cight::linePn(const cv::Point3f &line, const cv::Size &size) {
    return linePn(line.x, line.y, line.z, size);
}

static List<float> bestSlide(const cv::Mat &costs) {
    int rows = costs.rows;
    int cols = costs.cols;
    float yn = rows;

    float a = 0.0;
    float b = cols - 1;
    float d = FLT_MAX;
    float n = std::min(rows, cols);
    float x1n = n / 2;
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

    List<float> result;
    result.append(a);
    result.append(b);
    result.append(d);
    return result;
}

cv::Point3f cight::interpolateSlide(const cv::Mat &similarities) {
    cv::Mat costs = clarus::max(similarities) - similarities;
    List<float> bestX = bestSlide(costs);
    List<float> bestY = bestSlide(costs.t());

    List<cv::Point> points;
    if (bestX[2] < bestY[2]) {
        float x0 = bestX[0];
        float xn = bestX[1];
        float yd = similarities.rows;
        float t = yd / (xn - x0);
        return cv::Point3f(x0, 0, t);
    }
    else {
        float xd = similarities.cols;
        float y0 = bestY[0];
        float yn = bestY[1];
        float t = (yn - y0) / xd;
        int xn = similarities.cols;
        return cv::Point3f(0, y0, t);
    }
}

cv::Point3f cight::interpolateHough(const cv::Mat &similarities) {
    int rows = similarities.rows;
    int cols = similarities.cols;

    List<cv::Vec4i> lines;
    cv::Mat votes = images::convert(similarities, CV_8U);
    cv::HoughLinesP(votes, *lines, 1, CV_PI / 180.0, 5, 10, 10);

    // If no lines found, fall back to the brute force approach.
    if (lines.size() == 0) {
        return interpolateSlide(similarities);
    }

    cv::Point p0;
    float t = 0;
    float best = 0;
    for (int i = 0, n = lines.size(); i < n; i++) {
        const cv::Vec4i &line = lines[i];
        float x1 = line[0];
        float y1 = line[1];
        float x2 = line[2];
        float y2 = line[3];

        if (x2 - x1 <= 0 || y2 - y1 <= 0) {
            continue;
        }

        float tan = (y2 - y1) / (x2 - x1);
        cv::Point a = lineP0(x2, y2, tan);
        cv::Point b = linePn(x2, y2, tan, similarities.size());
        cv::Mat mask(rows, cols, CV_8U, cv::Scalar(0));
        cv::line(mask, a, b, cv::Scalar(255));

        cv::Mat selected(rows, cols, CV_32F, cv::Scalar(0));
        similarities.copyTo(selected, mask);
        float total = cv::sum(selected)[0];
        if (total > best) {
            best = total;
            t = tan;
            p0 = a;
        }
    }

    return cv::Point3f(p0.x, p0.y, t);
}
