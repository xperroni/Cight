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
    const int window,
    Interpolator _interpolator,
    Selector _selector,
    int _padding_a,
    int _padding_b,
    int _padding_c
):
    teachStream(teach),
    replayStream(replay),
    teachFrames(),
    teachEdges(),
    replayFrames(),
    replayMaps(),
    selector(_selector),
    padding_a(_padding_a),
    padding_b(_padding_b),
    padding_c(_padding_c),
    interpolator(_interpolator),
    similarities(window, window, CV_32F, cv::Scalar(0)),
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

    if (teachFrames.size() > similarities.rows) {
        teachFrames.remove(0);
        teachEdges.remove(0);
    }
}

void VisualMatcher::readReplayStream() {
    if (!replayStream->more()) {
        return;
    }

    cv::Mat frame = replayStream();
    FeatureMap features(selector, frame, padding_a);
    replayFrames.append(frame);
    replayMaps.append(features);

    if (replayFrames.size() > similarities.cols) {
        replayFrames.remove(0);
        replayMaps.remove(0);
    }
}

clarus::List<cv::Mat> VisualMatcher::operator() () {
    if (index == -1) {
        computeMatching();
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

    int rows = similarities.rows;
    int matched = std::min(rows - 1, (int) (index * tan));
    while (matched >= teachFrames.size()) {
        readReplayStream();
        readTeachStream();
        matched--;
        index--;
    }

    viewer::show("Teach", teachFrames.at(matched), 0, 0);
    viewer::show("Replay", replayFrames.at(index), 650, 0);
    cv::waitKey(200);

    List<cv::Mat> frames;
    frames.append(teachFrames.at(matched));
    frames.append(replayFrames.at(index));
    return frames;
}

void VisualMatcher::fillTeachBuffer() {
    int rows = similarities.rows;
    for (int i = teachFrames.size(); i < rows; i++) {
        readTeachStream();
    }
}

void VisualMatcher::fillReplayBuffer() {
    int cols = similarities.cols;
    for (int j = replayFrames.size(); j < cols; j++) {
        readReplayStream();
    }
}

void VisualMatcher::computeMatching() {
    fillTeachBuffer();
    fillReplayBuffer();

    int rows = similarities.rows;
    int cols = similarities.cols;
    for (int j = 0; j < cols; j++) {
        std::cerr << "Processing replay image #" << j << std::endl;
        const FeatureMap &features = replayMaps.at(j);
        int i0 = (padding_c != 0 ? std::max(j - padding_c, 0) : 0);
        int in = (padding_c != 0 ? std::min(j + padding_c, rows) : 0);

        List<cv::Mat> results = features(teachEdges, padding_b, i0, in);
        const cv::Mat &responses = results[0];
        cv::Rect roi(j, 0, 1, rows);
        cv::Mat column(similarities, roi);
        responses.copyTo(column);
    }

    List<cv::Point> line = interpolator(similarities);
    std::cerr << line << std::endl;

    cv::Point &p1 = line[1];
    p0 = line[0];
    yn = p1.y;

    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    tan = dy / dx;
    index = p0.y;

    std::ofstream file("similarities.txt", std::ios_base::out | std::ios_base::app);
    cv::Mat bgr = depths::bgr(similarities);
    cv::line(bgr, p0, p1, cv::Scalar(0, 0, 0));
    viewer::show("Similarities", images::scale(bgr, cv::Size(300, 300)));
    depths::save(similarities, file);
    file << std::endl;
    cv::waitKey(200);
}

bool VisualMatcher::more() const {
    return (teachStream->more() && replayStream->more());
}
