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

#include <cight/difference_matcher.hpp>
using cight::DifferenceMatcher;
using clarus::List;

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
    }

    static void displaySimilarities(const cv::Mat &similarities, const cv::Point3f &line) {
        std::cerr << line << std::endl;

        cv::Mat bgr = depths::bgr(similarities);
        cv::Point p0 = cight::lineP0(line);
        cv::Point p1 = cight::linePn(line, bgr.size());

        cv::line(bgr, p0, p1, cv::Scalar(0, 0, 0));
        viewer::show("Similarities", images::scale(bgr, cv::Size(300, 300), cv::INTER_NEAREST));
    }

    static void recordResponses(const cv::Mat &similarities, int a, int n) {
        std::ofstream out("similarities.txt", std::ios_base::out | std::ios_base::app);

        int height = similarities.rows;
        for (int i = a; i < n; i++) {
            cv::Mat responses(similarities, cv::Rect(i, 0, 1, height));
            out << responses.t() << std::endl;
        }
    }

    static void recordLines(const cv::Point3f &line) {
        std::ofstream lines("lines.txt", std::ios_base::out | std::ios_base::app);
        lines << line << std::endl;
    }
#else
    #define displayImagePairs(PAIR)

    #define displayMatches(A)

    #define displaySimilarities(A, B)

    #define recordResponses(A, I_0, I_N)

    #define recordLines(A)
#endif

DifferenceMatcher::DifferenceMatcher() {
    // Nothing to do.
}

DifferenceMatcher::DifferenceMatcher(
    const Interpolator &_interpolator,
    const StreamTeach &_teach,
    const StreamReplay &_replay
):
    interpolator(_interpolator),
    teach(_teach),
    replay(_replay),
    similarities(teach.size, replay.size),
    line(0, 0, 0),
    index(-1)
{
    // Nothing to do.
}

inline float teachIndex(const cv::Point3f &line, float index) {
    return line.y + (index - line.x) * line.z;
}

#define INDEX_N (similarities.cols - 1)

List<cv::Mat> DifferenceMatcher::operator () () {
/*
    if (replay.frames.empty()) {
        replay.read();
    }

    if (replay.read()) {
        return (List<cv::Mat>(), replay.diffs[-1], replay.diffs[-2]);
    }
*/
/*
    if (teach.read() && replay.read()) {
        return (List<cv::Mat>(), replay.diffs[-1], teach.diffs[-1]);
    }

    return List<cv::Mat>();
*/

    if (index == -1) {
        bool ok = computeMatching();
        recordResponses(similarities, 0, similarities.cols);
        if (!ok) {
            return List<cv::Mat>();
        }
    }

    if (index >= similarities.cols) {
        teach.pop();
        line.y--;

        replay.pop();
        line.x--;

        if (computeMatching() == false) {
            return List<cv::Mat>();
        }

        recordResponses(similarities, INDEX_N, similarities.cols);

        index = INDEX_N;
    }

    int matched = teachIndex(line, index);

    List<cv::Mat> frames = (List<cv::Mat>(),
        replay.diffs.at(index),
        teach.diffs.at(matched)
    );

    index++;

    displayImagePairs(frames);

    return frames;
}

bool DifferenceMatcher::computeMatching() {
    if (similarities.update(teach, replay) == false) {
        return false;
    }

    cv::Point3f line2 = interpolator(similarities);
/*
    float y1 = teachIndex(line, INDEX_N - 1);
    float y2 = teachIndex(line2, INDEX_N);

    if (y1 > y2) {
        float x0 = line2.x;
        float y0 = line2.y;
        line2.z = (y1 - y0) / (INDEX_N - x0);
    }
*/
    line = line2;
    recordLines(line);

    index = line.x;

    displaySimilarities(similarities, line);

    return true;
}
