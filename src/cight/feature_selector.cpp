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

#include <cight/feature_selector.hpp>
using cight::FeaturePoint;
using clarus::List;

#include <clarus/model/point.hpp>
using clarus::Point;

#include <clarus/vision/colors.hpp>
#include <clarus/vision/filters.hpp>

List<FeaturePoint> cight::selectAtMost(Selector selector, int limit, const cv::Mat &bgr, int padding) {
    List<FeaturePoint> regions = selector(bgr, padding);
    return (regions.size() > limit ? regions(0, limit) : regions);
}

List<FeaturePoint> cight::selectBorders(Selector selector, float ratio, const cv::Mat &bgr, int padding) {
    int xc = bgr.cols / 2;
    int yc = bgr.rows / 2;

    List<FeaturePoint> selected = selector(bgr, padding);
    std::map<uint64_t, const FeaturePoint*> ordered;
    for (int k = 0, n = selected.size(); k < n; k++) {
        const FeaturePoint &region = selected[k];
        const cv::Point &center = region.point();
        if (!region.empty()) {
            uint64_t d = abs(xc - center.x) + abs(yc - center.y);
            uint64_t key = (d << 32) + ordered.size();
            ordered[key] = &region;
        }
    }

    List<FeaturePoint> regions;
    int tops = ordered.size() * ratio;
    std::map<uint64_t, const FeaturePoint*>::iterator k = --(ordered.end());
    for (int i = 0; i < tops; ++i, --k) {
        regions.append(*(k->second));
    }

    return regions;
}

List<FeaturePoint> cight::selectDisjoint(Selector selector, const cv::Mat &bgr, int padding) {
    static cv::Scalar BLACK(0);
    static cv::Scalar WHITE(1);

    List<FeaturePoint> selected = selector(bgr, padding);

    int side = 2 * padding + 1;
    List<FeaturePoint> disjoint;
    cv::Mat mask(bgr.size(), CV_8U, cv::Scalar(0));
    for (int k = 0, n = selected.size(); k < n; k++) {
        const FeaturePoint &feature = selected[k];
        const cv::Point &center = feature.point();
        int x = center.x - padding;
        int y = center.y - padding;

        cv::Mat roi(mask, cv::Rect(x, y, side, side));

        if (cv::sum(roi)[0] == 0) {
            disjoint.append(feature);
            roi = 1;
        }
    }

    return disjoint;
}

static bool compareResponse(cv::KeyPoint a, cv::KeyPoint b) {
    return (a.response > b.response);
}

List<FeaturePoint> cight::selectFAST(cv::FastFeatureDetector &detector, const cv::Mat &bgr, int padding) {
    cv::Mat edges = filter::sobel(colors::grayscale(bgr));
    List<cv::KeyPoint> keypoints;
    detector.detect(edges, *keypoints);
    clarus::sort(keypoints, compareResponse);

    int rows = keypoints.size();
    List<FeaturePoint> regions;
    for (int k = 0; k < rows; k++) {
        const cv::Point &point = keypoints[k].pt;
        regions.append(FeaturePoint(point.x, point.y, edges, padding));
    }

    return regions;
}

List<FeaturePoint> cight::selectGoodFeatures(
    cv::GoodFeaturesToTrackDetector &detector,
    const cv::Mat &bgr,
    int padding
) {
    cv::Mat edges = filter::sobel(colors::grayscale(bgr));
    List<cv::KeyPoint> keypoints;
    detector.detect(edges, *keypoints);

    int rows = keypoints.size();
    List<FeaturePoint> regions;
    for (int k = 0; k < rows; k++) {
        const cv::Point &point = keypoints[k].pt;
        regions.append(FeaturePoint(point.x, point.y, edges, padding));
    }

    return regions;
}

static cv::Mat saturation_mask(const cv::Mat &bgr) {
    cv::Mat grays = colors::saturation(bgr);
    cv::normalize(grays, grays, 0, 255, CV_MINMAX);

    cv::Mat binary;
    cv::threshold(grays, binary, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

    cv::Mat edges = filter::sobel(grays);
    cv::threshold(edges, edges, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

    cv::Mat centers;
    binary.copyTo(centers, edges);

    cv::Mat foreground;
    cv::erode(centers, foreground, cv::Mat::ones(5, 5, CV_8U));

    cv::Mat background;
    cv::dilate(foreground, background, cv::Mat::ones(15, 15, CV_8U));
    cv::threshold(background, background, 1, 64, cv::THRESH_BINARY_INV);

    cv::Mat markers;
    cv::Mat mask = foreground + background;
    mask.convertTo(markers, CV_32S);

    return markers;
}

static cv::Mat boundaries(const cv::Mat &bgr, const cv::Mat &markers) {
    int rows = bgr.rows;
    int cols = bgr.cols;

    cv::watershed(bgr, markers);

    // Copy the boundaries of the objetcs segmented by cv::watershed().
    // Ensure there is a minimum distance of 1 pixel between boundary
    // pixels and the image border.
    cv::Mat borders(rows + 2, cols + 2, CV_8U, cv::Scalar::all(0));
    for (int i = 0; i < rows; i++) {
        uchar *u = borders.ptr<uchar>(i + 1) + 1;
        const int *v = markers.ptr<int>(i);
        for (int j = 0; j < cols; j++, u++, v++) {
            *u = (*v == -1);
        }
    }

    return borders;
}

typedef std::vector<cv::Point> Contour;

List<FeaturePoint> cight::selectSaturation(const cv::Mat &bgr, int padding) {
    static cv::Scalar WHITE = cv::Scalar::all(255);

    cv::Mat s = saturation_mask(bgr);
    cv::Mat borders = boundaries(bgr, s);

    // Calculate contour vectors for the boundaries extracted above.
    std::vector<Contour> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(borders, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    int ceil = bgr.size().area();

    cv::Mat edges = filter::sobel(colors::grayscale(bgr));
    List<FeaturePoint> regions;
    std::set<Point> unique;
    for (int i = 0, n = contours.size(); i < n; i++) {
        // Ignores repeated contours.
        const Contour &contour = contours[i];
        cv::Rect bounds = cv::boundingRect(contour);
        Point feature = clarus::center(bounds);
        if (unique.count(feature) > 0) {
            continue;
        }

        // Ignores contours for which the bounding rectangle's
        // area equals the area of the original image, or is
        // lower than the assigned minimum.
        int area = bounds.area();
        if (area >= ceil) { // || area < a0) {
            continue;
        }

        // Records the selected feature.
        regions.append(FeaturePoint(feature[0], feature[1], edges, padding));
        unique.insert(feature);
    }

    return regions;
}
