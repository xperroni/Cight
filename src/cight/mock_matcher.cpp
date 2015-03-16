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

#include <cight/mock_matcher.hpp>
using cight::MockMatcher;
using clarus::List;

#include <stdexcept>

MockMatcher::MockMatcher() {
    // Nothing to do.
}

MockMatcher::MockMatcher(const std::string &path, const StreamTeach &_teach, const StreamReplay &_replay):
    pairings(new std::ifstream(path.c_str())),
    teach(_teach),
    replay(_replay),
    offset(0)
{
    while (teach.diffs.size() < teach.size) {
        teach.read();
    }
}

List<cv::Mat> MockMatcher::operator () () {
    cv::Mat replayed = replay();
    if (replayed.empty()) {
        return List<cv::Mat>();
    }

    List<int> indices;
    *pairings >> indices;
    int k = indices[1];

    for (;; offset++) {
        int matched = k - offset;
        if (matched < 0) {
            throw std::runtime_error("Teach buffer underrun");
        }

        if (matched < teach.diffs.size()) {
            return (List<cv::Mat>(), replayed, teach.diffs[matched]);
        }

        if (!teach.read()) {
            return List<cv::Mat>();
        }
    }
}
