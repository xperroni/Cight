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

#include <cight/memory.hpp>
using cight::Memory;

#include <cight/transforms.hpp>

#include <clarus/core/list.hpp>
using clarus::List;
using clarus::ListIteratorConst;

#include <clarus/model/point.hpp>
using clarus::Point;
using clarus::Point2D;

#include <clarus/vision/depths.hpp>
#include <clarus/vision/filters.hpp>
#include <clarus/vision/images.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;
using std::cout;
using std::endl;

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <stdexcept>

Memory::Memory(size_t _range):
    List<cv::Mat>(),
    range(_range)
{
    // Nothing to do.
}

Memory::~Memory() {
    // Nothing to do.
}

size_t Memory::idle() const {
    return range - size();
}

void Memory::record(const cv::Mat &image) {
    append(image);
    if (size() > range) {
        remove(0);
    }
}

cv::Mat cight::drift_color(const Memory &memory) {
    if (memory.size() < 2) {
        throw std::runtime_error("Working memory underrun");
    }

    cv::Mat u = filter::tantriggs(memory.at(0));
    cv::Mat v = filter::tantriggs(memory.at(1));

    int n = memory.size();
    cv::Mat total(u.size(), CV_64FC3, cv::Scalar::all(0));
    for (int j = 1;;) {
        total += images::convert(images::absdiff(u, v), CV_64FC3);

        if (++j >= n) {
            break;
        }

        std::swap(u, v);
        v = filter::tantriggs(memory.at(j));
    }

    return total / n;
}

cv::Mat cight::change_count(const Memory &memory) {
    if (memory.size() == 0) {
        throw std::runtime_error("Eye buffer is empty");
    }

    cv::Mat u = memory.at(0);
    cv::Mat v = memory.at(1);

    int n = memory.size();
    cv::Mat total(u.size(), CV_32F, cv::Scalar::all(0));
    for (int j = 1;;) {
        total += images::convert(filter::otsu(images::absdiff(u, v)), CV_32F);

        if (++j >= n) {
            break;
        }

        std::swap(u, v);
        v = memory.at(j);
    }

    return total;
}

cv::Mat cight::change_average(const Memory &memory) {
    return change_count(memory) / (double) (memory.size() - 1);
}

cv::Mat cight::drift_edges(const Memory &memory) {
    if (memory.size() == 0) {
        throw std::runtime_error("Eye buffer is empty");
    }

    cv::Mat u = binary_edges(memory.at(0));
    cv::Mat v = binary_edges(memory.at(1));

    int n = memory.size();
    cv::Mat total(u.size(), CV_32F, cv::Scalar::all(0));
    for (int j = 1;;) {
        total += images::convert(images::absdiff(u, v), CV_32F);

        if (++j >= n) {
            break;
        }

        std::swap(u, v);
        v = binary_edges(memory.at(j));
    }

    return total;
}

cv::Mat cight::average_edges(const Memory &memory) {
    return drift_edges(memory) / (double) (memory.size() - 1);
}
