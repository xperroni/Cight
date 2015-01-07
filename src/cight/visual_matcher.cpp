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

#include <clarus/core/math.hpp>
#include <clarus/vision/colors.hpp>
#include <clarus/vision/filters.hpp>

#ifdef DIAGNOSTICS
    #include <clarus/core/types.hpp>
    #include <clarus/io/viewer.hpp>
    #include <clarus/vision/depths.hpp>
    #include <clarus/vision/images.hpp>

    #include <iostream>
    #include <fstream>

    static void displayImagePairs(const List<cv::Mat> &images) {
        viewer::show("Replay", images[0]);
        viewer::show("Teach", images[1]);
        char key = cv::waitKey(500);
        if (key == 'p') {
            do {
                key = cv::waitKey(1000);
            }
            while (key != 'p');
        }
    }

    static void displayMatches(const cv::Mat &matches) {
        static int index = 0;

        cv::Mat bgr = images::scale(depths::bgr(matches), cv::Size(300, 300), cv::INTER_NEAREST);
        viewer::show("Matches", bgr);
        images::save(bgr, "matches-" + types::to_string(index++) + ".png");
        cv::waitKey(WAIT_KEY_MS);
    }

    static void displaySimilarities(const cv::Mat &similarities, const cv::Point3f &line) {
        std::cerr << line << std::endl;

        cv::Mat bgr = depths::bgr(similarities);
        cv::Point p0 = cight::lineP0(line);
        cv::Point p1 = cight::linePn(line, bgr.size());

        cv::line(bgr, p0, p1, cv::Scalar(0, 0, 0));
        viewer::show("Similarities", images::scale(bgr, cv::Size(300, 300), cv::INTER_NEAREST));
        cv::waitKey(WAIT_KEY_MS);
    }

    static void recordResponses(const cv::Mat &responses) {
        std::ofstream similarities("similarities.txt", std::ios_base::out | std::ios_base::app);
        similarities << responses.t() << std::endl;
    }

    static void recordLines(const cv::Point3f &line) {
        std::ofstream lines("lines.txt", std::ios_base::out | std::ios_base::app);
        lines << line << std::endl;
    }
#else
    #define displayImagePairs(PAIR)

    #define displayMatches(A)

    #define displaySimilarities(A, B)

    #define recordResponses(A)

    #define recordLines(A, B)
#endif

VisualMatcher::VisualMatcher() {
    // Nothing to do.
}

VisualMatcher::VisualMatcher(
    SensorStream teach,
    SensorStream replay,
    const cv::Size &window,
    Selector _selector,
    int _padding_a,
    int _padding_b,
    Interpolator _interpolator
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
    interpolator(_interpolator),
    similarities(window, CV_32F, cv::Scalar(0)),
    index(-1),
    teach0(0),
    replay0(0),
    slope(0)
{
    // Nothing to do.
}

inline cv::Mat color_diff(const cv::Mat &bgr) {
    List<cv::Mat> channels = colors::channels(bgr);
    cv::Mat b = images::convert(channels[0], CV_16S);
    cv::Mat g = images::convert(channels[1], CV_16S);
    cv::Mat r = images::convert(channels[2], CV_16S);

    return images::convert(cv::abs(b - g) + cv::abs(g - r) + cv::abs(r - b), CV_8U);
}

bool VisualMatcher::readTeachStream() {
    cv::Mat frame = teachStream();
    if (frame.empty()) {
        return false;
    }

    cv::Mat grays = color_diff(frame);
    teachFrames.append(grays);

    cv::Mat edges = filter::sobel(grays);
    teachEdges.append(edges);

    if (teachFrames.size() > similarities.rows) {
        teachFrames.remove(0);
        teachEdges.remove(0);
    }

    return true;
}

bool VisualMatcher::readReplayStream() {
    cv::Mat frame = replayStream();
    if (frame.empty()) {
        return false;
    }

    cv::Mat grays = color_diff(frame);
    replayFrames.append(grays);

    FeatureMap features(selector, grays, padding_a);
    replayMaps.append(features);

    if (replayFrames.size() > similarities.cols) {
        replayFrames.remove(0);
        replayMaps.remove(0);
    }

    return true;
}

#define teachIndex(INDEX) (teach0 + ((INDEX) - replay0) * slope)

clarus::List<cv::Mat> VisualMatcher::operator() () {
    if (index == -1 && computeMatching() == false) {
        return List<cv::Mat>();
    }

    if (index >= similarities.cols) {
        teachFrames.remove(0);
        replayFrames.remove(0);
        teachEdges.remove(0);
        replayMaps.remove(0);

        if (computeMatching() == false) {
            return List<cv::Mat>();
        }

        index = similarities.cols - 1;
    }

    int matched = teachIndex(index);

    List<cv::Mat> frames;
    frames.append(replayFrames.at(index));
    frames.append(teachFrames.at(matched));

    index++;

    displayImagePairs(frames);

    return frames;
}

bool VisualMatcher::computeMatching() {
    int rows = similarities.rows;
    int cols = similarities.cols;

    int row0 = teachFrames.size();
    int col0 = replayFrames.size();

    // Shift the similarity matrix to make room for new match estimations
    clarus::shift(similarities, row0 - rows, col0 - cols);

    // Fill teach buffer to capacity
    for (int i = row0; i < rows; i++) {
        if (!readTeachStream()) {
            return false;
        }
    }

    // Collect replay images, filling the similarity matrix along the way
    for (int j = col0; j < cols; j++) {
        if (!readReplayStream()) {
            return false;
        }

        const FeatureMap &features = replayMaps.at(j);
        List<cv::Mat> results = features(teachEdges, padding_b);
        const cv::Mat &responses = results[0];
        recordResponses(responses);
        displayMatches(results[1]);
        cv::Rect roi(j, 0, 1, rows);
        cv::Mat column(similarities, roi);
        responses.copyTo(column);
    }

    cv::Point3f line = interpolator(similarities);
    recordLines(line);

    index = line.x;
    replay0 = line.x;
    teach0 = line.y;
    slope = line.z;

    displaySimilarities(similarities, line);

    return true;
}
