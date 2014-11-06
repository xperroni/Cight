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

#include <cight/video_stream.hpp>
using cight::VideoStream;

VideoStream::VideoStream(const std::string &path, int spacing):
    recording(path),
    sampling(spacing)
{
    // Nothing to do.
}

VideoStream::~VideoStream() {
    // Nothing to do.
}

cv::Mat VideoStream::operator () () {
    cv::Mat frame = recording.next();

    // Discard the appropriate number of frames as per the configured sampling.
    for (int i = 0; i < sampling && recording.more(); i++) {
        recording.next();
    }

    return frame;
}

bool VideoStream::more() const {
    return recording.more();
}
