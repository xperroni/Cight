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

#ifndef CIGHT_VIDEO_STREAM_HPP
#define CIGHT_VIDEO_STREAM_HPP

#include <cight/image_stream.hpp>
#include <cight/sensor_stream.hpp>

#include <clarus/io/video.hpp>

#include <fstream>
#include <string>

namespace cight {
    class VideoStream;
}

/**
\brief Sensor stream that replays a recorded video file.
*/
class cight::VideoStream {
    /** \brief Object used to replay a video file. */
    clarus::Video recording;

    /** \brief How many frames to discard for every frame returned. */
    int sampling;

public:
    /**
    \brief Default constructor.
    */
    VideoStream();

    /**
    \brief Creates a new sensor stream replay from the video file at the given path.

    \param path Path to the desired video file.

    \param spacing Number of discarded frames between returned frames.
    */
    VideoStream(const std::string &path, int spacing = 0);

    /**
    \brief Virtual destructor. Enforces polymorphism. Do not remove.
    */
    virtual ~VideoStream();

    // See cight::SensorStream::operator () ()
    virtual cv::Mat operator () ();
};

#endif
