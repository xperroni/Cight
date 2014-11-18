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

#include <cight/camera_stream.hpp>
using cight::CameraStream;

CameraStream::CameraStream(int index, double fps):
    camera(index, fps)
{
    // Nothing to do.
}

CameraStream::~CameraStream() {
    // Nothing to do.
}

cv::Mat CameraStream::operator () () {
    return camera.next();
}

bool CameraStream::more() const {
    return camera.opened();
}
