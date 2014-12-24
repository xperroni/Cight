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

    static void displaySimilarities(const cv::Mat &similarities, const cv::Point3f &line, int slit) {
        std::ofstream file("line.txt");
        file << "(" << line << ", " << slit << ")" << std::endl;
        std::cerr << "(" << line << ", " << slit << ")" << std::endl;

        cv::Mat bgr = depths::bgr(similarities);
        cv::Point p0 = cight::lineP0(line);
        cv::Point p1 = cight::linePn(line, bgr.size());

        cv::line(bgr, p0, p1, cv::Scalar(0, 0, 0));
        viewer::show("Similarities", images::scale(bgr, cv::Size(300, 300)));
        depths::save(similarities, "similarities.txt");
        cv::waitKey(WAIT_KEY_MS);
    }

    static void displayResponses(std::ostream &out, int x0, int y0, int i, float slope, const cv::Mat &responses) {
        out
            << "("
            << "(" << x0 << ", " << y0 << ", " << i << ", " << slope << ")"
            << ", "
            << responses.t()
            << ")" << std::endl
        ;
    }

    static void displayResponses(int x0, int y0, int i, float slope, const cv::Mat &responses) {
        std::ofstream file("similarities_extra.txt", std::ios_base::out | std::ios_base::app);
        displayResponses(std::cerr, x0, y0, i, slope, responses);
        displayResponses(file, x0, y0, i, slope, responses);
    }
#else
    #define displayImagePairs(PAIR)

    #define displayMatches(A)

    #define displaySimilarities(A, B)

    #define displayResponses(X, Y, R)
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
    slope(0),
    slit(0)
{
    // Nothing to do.
}

bool VisualMatcher::readTeachStream() {
    cv::Mat frame = teachStream();
    if (frame.empty()) {
        return false;
    }

    cv::Mat edges = filter::sobel(colors::grayscale(frame));
    teachFrames.append(frame);
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

    FeatureMap features(selector, frame, padding_a);
    replayFrames.append(frame);
    replayMaps.append(features);

    if (replayFrames.size() > similarities.cols) {
        replayFrames.remove(0);
        replayMaps.remove(0);
    }

    return true;
}

#define teachIndex(INDEX) (teach0 + ((INDEX) - replay0) * slope)

clarus::List<cv::Mat> VisualMatcher::operator() () {
    if (index == -1) {
        computeMatching();
    }

    int matched = teachIndex(index);
    for (int i = 0, n = matched + slit - similarities.rows; i < n; i++) {
        if (!readTeachStream()) {
            return List<cv::Mat>();
        }

        teach0--;
        matched--;
    }

    if (index >= similarities.cols) {
        if (!readReplayStream()) {
            return List<cv::Mat>();
        }

        replay0--;

        index = replayMaps.size() - 1;
        int i0 = std::max((int) (matched - slit), 0);
        int in = std::min((int) (matched + slit), (int) teachEdges.size());

        const FeatureMap &features = replayMaps.at(index);
        List<cv::Mat> results = features(teachEdges, padding_b, i0, in);
        const cv::Mat &responses = results[0];
        displayMatches(results[1]);

        matched = std::max(clarus::argmax(responses).y, (int) ceil(teachIndex(index - 1)));
        slope = (matched - teach0) / (index - replay0);

        displayResponses(teach0, replay0, matched, slope, responses);
    }

    List<cv::Mat> frames;
    frames.append(replayFrames.at(index));
    frames.append(teachFrames.at(matched));

    index++;

    displayImagePairs(frames);

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

inline float computeSlit(const cv::Mat &similarities, const cv::Point3f &line) {
    int rows = similarities.rows;
    int cols = similarities.cols;
    float replay0 = line.x;
    float teach0 = line.y;
    float slope = line.z;

    int rl = 0;
    int rh = 0;

    for (int j = replay0; j < cols; j++) {
        int ic = teach0 + (j - replay0) * slope;

        int il = 0;
        for (int i = 1; ic - i >= 0; i++) {
            if (similarities.at<float>(ic - i, j) > 0) {
                il = i;
            }
        }

        int ih = 0;
        for (int i = 0; ic + i < rows; i++) {
            if (similarities.at<float>(ic + i, j) > 0) {
                ih = i;
            }
        }

        rl = std::max(rl, il);
        rh = std::max(rh, ih);
    }

    return (rl + rh) / 2;
}

void VisualMatcher::computeMatching() {
    fillTeachBuffer();
    fillReplayBuffer();

    int rows = similarities.rows;
    int cols = similarities.cols;
    for (int j = 0; j < cols; j++) {
        std::cerr << "Processing replay image #" << j << std::endl;
        const FeatureMap &features = replayMaps.at(j);
        List<cv::Mat> results = features(teachEdges, padding_b);
        const cv::Mat &responses = results[0];
        displayMatches(results[1]);
        cv::Rect roi(j, 0, 1, rows);
        cv::Mat column(similarities, roi);
        responses.copyTo(column);
    }

    cv::Point3f line = interpolator(similarities);

    index = line.x;
    replay0 = line.x;
    teach0 = line.y;
    slope = line.z;
    slit = computeSlit(similarities, line);

    displaySimilarities(similarities, line, slit);
}
