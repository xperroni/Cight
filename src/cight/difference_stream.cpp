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

#include <cight/difference_stream.hpp>
using cight::DifferenceStream;
using cight::StreamBuffer;

#include <clarus/io/viewer.hpp>
#include <clarus/vision/filters.hpp>
#include <clarus/core/math.hpp>
#include <clarus/vision/images.hpp>

DifferenceStream::DifferenceStream():
    StreamBuffer()
{
    threshold = 0;
}

DifferenceStream::DifferenceStream(SensorStream stream, size_t size, double _threshold):
    StreamBuffer(stream, size),
    threshold(_threshold)
{
    // Nothing to do.
}

cv::Mat DifferenceStream::operator () () {
    return (read() ? diffs[-1] : cv::Mat());
}

void DifferenceStream::pop() {
    frames.remove(0);
    indices.remove(0);
    diffs.remove(0);
}

bool DifferenceStream::read() {
    for (int i = 1;; i++) {
        cv::Mat frame = stream();
        if (frame.empty()) {
            return false;
        }

        if (frames.empty()) {
            frames.append(frame);
            indices.append(0);
            continue;
        }

        cv::Mat diff = images::difference(frames[-1], frame, CV_8U);
        double mean = clarus::mean(diff);
        if (mean < threshold) {
            continue;
        }

        //diff = filter::masked(diff, filter::otsu(diff));
        frames.append(frame);
        indices.append(i + indices[-1]);
        diffs.append(diff);

        if (diffs.size() > size) {
            pop();
        }

        return true;
    }
}
