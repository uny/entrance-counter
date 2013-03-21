#include "tracking_person.h"

#include <iostream>

TrackingPerson::TrackingPerson()
{
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

bool TrackingPerson::MoveRect(const std::vector<uchar> &lk_status)
{
    int sum_move_x = 0;
    int sum_move_y = 0;
    int count = 0;

    int index = 0;

    int move_x;
    int move_y;

    int ave_move_x;
    int ave_move_y;

    std::vector<cv::Point2f>::iterator prev_points_iter = track_points[TP_TRANSITION_PREV].begin();
    std::vector<cv::Point2f>::iterator next_points_iter = track_points[TP_TRANSITION_NEXT].begin();

    // TODO: here, remove uncertain points
    while (prev_points_iter != track_points[TP_TRANSITION_PREV].end()
           || next_points_iter != track_points[TP_TRANSITION_NEXT].end()) {
        if (!lk_status[index]) {
            prev_points_iter = track_points[TP_TRANSITION_PREV].erase(prev_points_iter);
            next_points_iter = track_points[TP_TRANSITION_NEXT].erase(next_points_iter);
            continue;
        }
        // TODO: whether next points are too far
        move_x = track_points[TP_TRANSITION_NEXT][index].x - track_points[TP_TRANSITION_PREV][index].x;
        move_y = track_points[TP_TRANSITION_NEXT][index].y - track_points[TP_TRANSITION_PREV][index].y;

        sum_move_x += move_x;
        sum_move_y += move_y;
        count++;

        ++prev_points_iter;
        ++next_points_iter;
    }
    if (0 < count) {
        ave_move_x = sum_move_x / count;
        ave_move_y = sum_move_y / count;
    }
    else {
        move_x = 0;
        move_y = 0;
    }

    // update confidence
    // TODO: after checking bounding region
    track_confidence *= (double)count / lk_status.size();
    if (track_confidence < (MINIMUM_TRACK_CONFIDENCE / 10.0)) {
        return false;
    }
    // TODO: check points are inside rect

    bounding_rect[TP_TRANSITION_NEXT].x = cvRound(bounding_rect[TP_TRANSITION_PREV].x + move_x);
    bounding_rect[TP_TRANSITION_NEXT].y = cvRound(bounding_rect[TP_TRANSITION_PREV].y + move_y);
    bounding_rect[TP_TRANSITION_NEXT].width = bounding_rect[TP_TRANSITION_PREV].width;
    bounding_rect[TP_TRANSITION_NEXT].height = bounding_rect[TP_TRANSITION_PREV].height;

    return true;
}

void TrackingPerson::InitializeForDetection()
{
    // all features are valid, so should set lk_status all true (1)
    // TODO: remove though unchecked, doutful
//    lk_status = std::vector<uchar>(track_points[1].size(), 1);
    track_confidence = 1.0;
}

void TrackingPerson::OverwriteLog(std::vector<TrackingPerson> &tracking_people)
{
    std::vector<cv::Point2f> tmp_points;
    // TODO: will thin out on MoveRect, so the method does not need status filter
    for (TrackingPerson &tracking_person : tracking_people) {
        if (!tracking_person.track_points[TP_TRANSITION_NEXT].size()) {
            continue;
        }
        for (int index = 0; index < (int)tracking_person.track_points[TP_TRANSITION_NEXT].size(); index++) {
//            if (!tracking_person.lk_status[index]) {
//                continue;
//            }
//            if (!tracking_person.track_points[TP_TRANSITION_NEXT][index].inside(tracking_person.bounding_rect[TP_TRANSITION_NEXT])) {
//                continue;
//            }
            tmp_points.push_back(tracking_person.track_points[TP_TRANSITION_NEXT][index]);
        }
        tracking_person.bounding_rect[TP_TRANSITION_PREV] = tracking_person.bounding_rect[TP_TRANSITION_NEXT];
        tracking_person.track_points[TP_TRANSITION_PREV].swap(tmp_points);
    }
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
