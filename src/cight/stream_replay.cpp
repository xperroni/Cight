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

#include <cight/stream_replay.hpp>
using cight::StreamReplay;
using clarus::List;

StreamReplay::StreamReplay():
    DifferenceStream()
{
    // Nothing to do.
}

StreamReplay::StreamReplay(SensorStream stream, size_t size, double threshold, Selector _selector, int _padding):
    DifferenceStream(stream, size, threshold),
    selector(_selector),
    padding(_padding)
{
    // Nothing to do.
}

cv::Mat StreamReplay::operator () () {
    return DifferenceStream::operator () ();
}

List<cv::Mat> StreamReplay::operator () (int j, StreamTeach &teach) {
    const FeatureMap &featured = features.at(j);
    List<cv::Mat> results = featured(teach.diffs, teach.padding);
    return results;
}

void StreamReplay::pop() {
    DifferenceStream::pop();
    features.remove(0);
}

bool StreamReplay::read() {
    if (!DifferenceStream::read()) {
        return false;
    }

    FeatureMap featured(selector, diffs.at(-1), padding);

    features.append(featured);

    return true;
}
