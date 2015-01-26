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
using cight::StreamBuffer;
using cight::StreamTeach;
using cight::StreamReplay;
using cight::SimilarityMap;
using cight::VisualMatcher;
using clarus::List;

#include <clarus/core/math.hpp>
#include <clarus/vision/colors.hpp>
#include <clarus/vision/filters.hpp>
#include <clarus/vision/images.hpp>

#ifdef DIAGNOSTICS
    #include <clarus/core/types.hpp>
    #include <clarus/io/viewer.hpp>
    #include <clarus/vision/depths.hpp>

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
    }

    static void displaySimilarities(const cv::Mat &similarities, const cv::Point3f &line) {
        std::cerr << line << std::endl;

        cv::Mat bgr = depths::bgr(similarities);
        cv::Point p0 = cight::lineP0(line);
        cv::Point p1 = cight::linePn(line, bgr.size());

        cv::line(bgr, p0, p1, cv::Scalar(0, 0, 0));
        viewer::show("Similarities", images::scale(bgr, cv::Size(300, 300), cv::INTER_NEAREST));
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

    #define recordLines(A)
#endif

StreamTeach::StreamTeach():
    StreamBuffer()
{
    // Nothing to do.
}

StreamTeach::StreamTeach(SensorStream _stream, size_t _size, int padding_b):
    StreamBuffer(_stream, _size),
    padding(padding_b)
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

inline cv::Mat preprocess2(const cv::Mat &bgr) {
    return colors::grayscale(bgr);
/*
    cv::Mat grad;
    cv::Sobel(colors::grayscale(bgr), grad, CV_8U, 1, 0, CV_SCHARR);
    return grad;
*/
}

void StreamTeach::pop() {
    frames.remove(0);
    edges.remove(0);
}

bool StreamTeach::read() {
    cv::Mat frame = stream();
    if (frame.empty()) {
        return false;
    }

    cv::Mat grays = preprocess2(frame);
    frames.append(grays);

    cv::Mat sobeld = filter::sobel(grays);
    edges.append(sobeld);

    if (frames.size() > size) {
        pop();
    }

    return true;
}

StreamReplay::StreamReplay():
    StreamBuffer()
{
    // Nothing to do.
}

StreamReplay::StreamReplay(SensorStream _stream, size_t _size, Selector _selector, int padding_a):
    StreamBuffer(_stream, _size),
    selector(_selector),
    padding(padding_a)
{
    // Nothing to do.
}

List<cv::Mat> StreamReplay::operator () (int j, StreamTeach &teach) {
    const FeatureMap &features = maps.at(j);
    List<cv::Mat> results = features(teach.edges, teach.padding);
    //shifts.at(j) = results[1];
    return results;
}

void StreamReplay::pop() {
    frames.remove(0);
    maps.remove(0);
    //shifts.remove(0);
}

bool StreamReplay::read() {
    cv::Mat frame = stream();
    if (frame.empty()) {
        return false;
    }

    cv::Mat grays = preprocess2(frame);
    frames.append(grays);

    FeatureMap features(selector, grays, padding);
    maps.append(features);

    //shifts.append();

    if (frames.size() > size) {
        pop();
    }

    return true;
}

cv::Mat StreamReplay::shifted(int i, int j) const {
    cv::Mat matrix = shifts.at(i);
    cv::Mat row(matrix, cv::Rect(0, i, matrix.cols, 1));
    return row;
}

SimilarityMap::SimilarityMap() {
    // Nothing to do.
}

SimilarityMap::SimilarityMap(const cv::Size &size):
    cv::Mat(size, CV_32F, cv::Scalar(0))
{
    // Nothing to do.
}

bool SimilarityMap::update(StreamTeach &teach, StreamReplay &replay) {
    int row0 = teach.frames.size();
    int col0 = replay.frames.size();

    // Shift the similarity matrix to make room for new match estimations
    clarus::shift(*this, row0 - rows, col0 - cols);

    // Fill teach buffer to capacity
    for (int i = row0; i < rows; i++) {
        if (!teach.read()) {
            return false;
        }
    }

    // Collect replay images, filling the similarity matrix along the way
    for (int j = col0; j < cols; j++) {
        if (!replay.read()) {
            return false;
        }

        List<cv::Mat> results = replay(j, teach);
        const cv::Mat &responses = results[0];
        recordResponses(responses);
        displayMatches(results[1]);
        cv::Rect roi(j, 0, 1, rows);
        cv::Mat column(*this, roi);
        responses.copyTo(column);
    }

    return true;
}

VisualMatcher::VisualMatcher() {
    // Nothing to do.
}

VisualMatcher::VisualMatcher(
    SensorStream teachStream,
    SensorStream replayStream,
    const cv::Size &window,
    Selector selector,
    int _padding_a,
    int _padding_b,
    Interpolator _interpolator
):
    teach(teachStream, window.height, _padding_b),
    replay(replayStream, window.width, selector, _padding_a),
    similarities(window),
    interpolator(_interpolator),
    line(0, 0, 0),
    index(-1)
{
    // Nothing to do.
}

//#define teachIndex(INDEX) (teach0 + ((INDEX) - replay0) * slope)

inline float teachIndex(const cv::Point3f &line, float index) {
    return line.y + (index - line.x) * line.z;
}

#define INDEX_N (similarities.cols - 1)

clarus::List<cv::Mat> VisualMatcher::operator() () {
    if (index == -1 && computeMatching() == false) {
        return List<cv::Mat>();
    }

    if (index >= similarities.cols) {
        //if (teach.frames.size() - teachIndex(line, INDEX_N) < similarities.rows / 2) {
            teach.pop();
            line.y--;
        //}

        replay.pop();
        line.x--;

        if (computeMatching() == false) {
            return List<cv::Mat>();
        }

        index = INDEX_N;
    }

    int matched = teachIndex(line, index);

    List<cv::Mat> frames;
    frames.append(replay.frames.at(index));
    frames.append(teach.frames.at(matched));
    //frames.append(replay.shifted(index, matched));

    index++;

    displayImagePairs(frames);

    return frames;
}

bool VisualMatcher::computeMatching() {
    if (similarities.update(teach, replay) == false) {
        return false;
    }

    cv::Point3f line2 = interpolator(similarities);
    float y1 = teachIndex(line, INDEX_N - 1);
    float y2 = teachIndex(line2, INDEX_N);

    if (y1 > y2) {
        float x0 = line2.x;
        float y0 = line2.y;
        line2.z = (y1 - y0) / (INDEX_N - x0);
    }

    line = line2;
    recordLines(line);

    index = line.x;

    displaySimilarities(similarities, line);

    return true;
}
