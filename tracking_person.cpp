#include "tracking_person.h"

#include <iostream>

TrackingPerson::TrackingPerson()
{
    lk_status = std::vector<uchar>();
}

void TrackingPerson::JustifyFeaturesPoint(const cv::Point &from_point, const cv::Point &to_point, TP_TRANSITION_ENUM select)
{
    if (select == TP_TRANSITION_BOTH || select == TP_TRANSITION_PREV) {
        JustifySelectedFeaturesPoint(track_points[TP_TRANSITION_PREV], from_point, to_point);
    }
    if (select == TP_TRANSITION_BOTH || select == TP_TRANSITION_NEXT) {
        JustifySelectedFeaturesPoint(track_points[TP_TRANSITION_NEXT], from_point, to_point);
    }
}

void TrackingPerson::MoveRect()
{
    int sum_move_x = 0;
    int sum_move_y = 0;
    int count = 0;

    int index = 0;

    int move_x;
    int move_y;

    int ave_move_x;
    int ave_move_y;


    for (index = 0; index < (int)track_points[TP_TRANSITION_PREV].size(); index++) {
        if (!lk_status[index]) {
            continue;
        }
        move_x = track_points[TP_TRANSITION_NEXT][index].x - track_points[TP_TRANSITION_PREV][index].x;
        move_y = track_points[TP_TRANSITION_NEXT][index].y - track_points[TP_TRANSITION_PREV][index].y;

        if (std::abs(move_x) < MINIMUM_FEATURE_MOVE || std::abs(move_y) < MINIMUM_FEATURE_MOVE) {
            continue;
        }

        sum_move_x += move_x;
        sum_move_y += move_y;
        count++;
    }
    if (0 < count) {
        ave_move_x = sum_move_x / count;
        ave_move_y = sum_move_y / count;
    }
    else {
        move_x = 0;
        move_y = 0;
    }
    bounding_rect[TP_TRANSITION_NEXT].x = cvRound(bounding_rect[TP_TRANSITION_PREV].x + move_x);
    bounding_rect[TP_TRANSITION_NEXT].y = cvRound(bounding_rect[TP_TRANSITION_PREV].y + move_y);
    bounding_rect[TP_TRANSITION_NEXT].width = bounding_rect[TP_TRANSITION_PREV].width;
    bounding_rect[TP_TRANSITION_NEXT].height = bounding_rect[TP_TRANSITION_PREV].height;
}

void TrackingPerson::OverwriteLog()
{
    std::vector<cv::Point2f> tmp_points;

    for (int index = 0; index < (int)lk_status.size(); index++) {
        if (!lk_status[index]) {
            continue;
        }
        if (!track_points[TP_TRANSITION_NEXT][index].inside(bounding_rect[TP_TRANSITION_NEXT])) {
            continue;
        }
        tmp_points.push_back(track_points[TP_TRANSITION_NEXT][index]);
    }

    // for debug
    if (lk_status.size() != tmp_points.size()) {
        std::cout << lk_status.size() << " > " << tmp_points.size() << std::endl;
    }

    // update confidence
    track_confidence *= (double)tmp_points.size() / lk_status.size();
    std::cout << track_confidence << std::endl;
    if (track_confidence < MINIMUM_TRACK_CONFIDENCE) {
        missing_count = 1;
        return;
    }

    bounding_rect[TP_TRANSITION_PREV] = bounding_rect[TP_TRANSITION_NEXT];
    track_points[TP_TRANSITION_PREV].swap(tmp_points);
}

void TrackingPerson::InitializeForDetection()
{
    // all features are valid, so should set lk_status all true (1)
    lk_status = std::vector<uchar>(track_points[1].size(), 1);
    track_confidence = 1.0;
}

void TrackingPerson::JustifySelectedFeaturesPoint(std::vector<cv::Point2f> &features, const cv::Point &from_point, const cv::Point &to_point)
{
    for (int index = 0; index < (int)features.size(); index++) {
        features[index].x += to_point.x - from_point.x;
        features[index].y += to_point.y - from_point.y;
    }
}

cv::Rect TrackingPerson::ExpandRectToTrack(cv::Size frame_size)
{
    cv::Rect rect = bounding_rect[TP_TRANSITION_PREV];
    cv::Rect larger;
    larger.x = std::max(rect.x - MARGIN_WIDTH, 0);
    larger.y = std::max(rect.y - MARGIN_WIDTH, 0);
    larger.width = rect.width + MARGIN_WIDTH * 2;
    larger.height = rect.height + MARGIN_WIDTH * 2;
    if (frame_size.width < larger.x + larger.width) {
        larger.width = frame_size.width - larger.x;
    }
    if (frame_size.height < larger.y + larger.height) {
        larger.height = frame_size.height - larger.y;
    }
    return larger;
}
