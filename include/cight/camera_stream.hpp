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

#ifndef CIGHT_CAMERA_STREAM_HPP
#define CIGHT_CAMERA_STREAM_HPP

#include <cight/image_stream.hpp>
#include <cight/sensor_stream.hpp>

#include <clarus/io/camera.hpp>

#include <fstream>
#include <string>

namespace cight {
    class CameraStream;
}

/**
\brief Sensor stream that replays a recorded video file.
*/
class cight::CameraStream: public SensorStream {
    /** \brief Object used to replay a video file. */
    clarus::Camera camera;

    /** \brief How many frames to discard for every frame returned. */
    const int sampling;

public:
    /**
    \brief Creates a new sensor stream connected to a video camera.

    \param index Index of the underlying camera. Default is 0 (the first camera found in the USB hub).

    \param fps Frames per second setting for the camera. Default is 20 FPS.

    \param spacing How many camera frames are discarded for each frame returned.

    \param recording If given, a path to a video file where camera input will be recorded.
    */
    CameraStream(int index = 0, double fps = 20, int spacing = 0, const std::string &recording = "");

    /**
    \brief Virtual destructor. Enforces polymorphism. Do not remove.
    */
    virtual ~CameraStream();

    // See cight::SensorStream::operator () ()
    virtual cv::Mat operator () ();

    // See cight::SensorStream::more()
    virtual bool more() const;
};

#endif
