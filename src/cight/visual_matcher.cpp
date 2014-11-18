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

#include <cight/visual_matcher.hpp>
using cight::VisualMatcher;
using clarus::List;

#include <clarus/io/viewer.hpp>
#include <clarus/vision/depths.hpp>
#include <clarus/vision/images.hpp>

#include <clarus/vision/colors.hpp>
#include <clarus/vision/filters.hpp>

#include <iostream>
#include <fstream>

VisualMatcher::VisualMatcher(
    SensorStream::P teach,
    SensorStream::P replay,
    const cv::Size &window,
    Interpolator _interpolator,
    Selector _selector,
    int _padding,
    int range
):
    teachStream(teach),
    replayStream(replay),
    teachFrames(),
    teachEdges(),
    replayFrames(),
    replayMaps(),
    selector(_selector),
    padding(_padding),
    searchRange(range),
    interpolator(_interpolator),
    similarities(window, CV_32F, cv::Scalar(0)),
    index(-1),
    p0(0, 0),
    tan(0),
    yn(0)
{
    // Nothing to do.
}

void VisualMatcher::readTeachStream() {
    if (!teachStream->more()) {
        return;
    }

    cv::Mat frame = teachStream();
    cv::Mat edges = filter::sobel(colors::grayscale(frame));
    teachFrames.append(frame);
    teachEdges.append(edges);

    if (teachFrames.size() > similarities.cols) {
        teachFrames.remove(0);
        teachEdges.remove(0);
    }
}

void VisualMatcher::readReplayStream() {
    if (!replayStream->more()) {
        return;
    }

    cv::Mat frame = replayStream();
    InterestMap regions(selector, frame, padding);
    replayFrames.append(frame);
    replayMaps.append(regions);

    if (replayFrames.size() > similarities.rows) {
        replayFrames.remove(0);
        replayMaps.remove(0);
    }
}

void VisualMatcher::computeSimilarityMap() {
    int rows = similarities.rows;
    int cols = similarities.cols;
    for (int i = 0; i < rows; i++) {
        std::cerr << "Processing similarity map #" << i << std::endl;
        const InterestMap &regions = replayMaps.at(i);
        List<cv::Mat> results = regions(teachEdges, searchRange);
        const cv::Mat &responses = results[0];
        cv::Rect roi(0, i, cols, 1);
        cv::Mat row(similarities, roi);
        responses.copyTo(row);
    }

    List<cv::Point> line = interpolator(similarities);
    cv::Point &p1 = line[1];
    p0 = line[0];
    yn = p1.y;

    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    tan = dx / dy;

    std::ofstream file("similarities.txt", std::ios_base::out | std::ios_base::app);
    cv::Mat bgr = depths::bgr(similarities);
    cv::line(bgr, p0, p1, cv::Scalar(0, 0, 0));
    viewer::show("Similarities", images::scale(bgr, cv::Size(300, 300)));
    depths::save(similarities, file);
    file << std::endl;
    cv::waitKey(1000);
}

clarus::List<cv::Mat> VisualMatcher::operator() () {
    int cols = similarities.cols;
    if (index == -1) {
        int rows = similarities.rows;
        for (int i = 0; i < rows; i++) {
            readReplayStream();
        }

        for (int j = 0; j < cols; j++) {
            readTeachStream();
        }

        computeSimilarityMap();
        index = p0.y;
    }
    else if (index == yn) {
        readReplayStream();
        readTeachStream();
        //computeSimilarityMap();
        index = yn;
    }
    else {
        index++;
    }

    int matched = std::min(cols - 1, (int) (index * tan));
    while (matched >= teachFrames.size()) {
        readReplayStream();
        readTeachStream();
        matched--;
        index--;
    }

    viewer::show("Teach", teachFrames.at(matched), 0, 0);
    viewer::show("Replay", replayFrames.at(index), 650, 0);
    cv::waitKey(1000);

    List<cv::Mat> frames;
    frames.append(teachFrames.at(matched));
    frames.append(replayFrames.at(index));
    return frames;
}

bool VisualMatcher::more() const {
    return replayStream->more();
}
