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

#include <cight/similarity_map.hpp>
using cight::SimilarityMap;

#include <clarus/core/list.hpp>
using clarus::List;

#include <clarus/core/math.hpp>

SimilarityMap::SimilarityMap() {
    // Nothing to do.
}

SimilarityMap::SimilarityMap(int rows, int cols):
    cv::Mat(rows, cols, CV_32F, cv::Scalar(0))
{
    // Nothing to do.
}

SimilarityMap::SimilarityMap(const cv::Size &size):
    cv::Mat(size, CV_32F, cv::Scalar(0))
{
    // Nothing to do.
}

bool SimilarityMap::update(StreamTeach &teach, StreamReplay &replay) {
    int row0 = teach.diffs.size();
    int col0 = replay.diffs.size();

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
        cv::Rect roi(j, 0, 1, rows);
        cv::Mat column(*this, roi);
        responses.copyTo(column);
    }

    return true;
}
