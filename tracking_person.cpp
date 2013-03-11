#include "tracking_person.h"

#include <iostream>

TrackingPerson::TrackingPerson()
{
}

void TrackingPerson::JustifyFeaturesPoint(const cv::Point &from_point, const cv::Point &to_point, TP_JUSTIFY_ENUM select)
{
    if (select == TP_JUSTIFY_BOTH || select == TP_JUSTIFY_PREV) {
        JustifySelectedFeaturesPoint(track_points[0], from_point, to_point);
    }
    if (select == TP_JUSTIFY_BOTH || select == TP_JUSTIFY_NEXT) {
        JustifySelectedFeaturesPoint(track_points[1], from_point, to_point);
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


    for (index = 0; index < (int)track_points[0].size(); index++) {
        if (!lk_status[index]) {
            continue;
        }
        move_x = track_points[1][index].x - track_points[0][index].x;
        move_y = track_points[1][index].y - track_points[0][index].y;

        std::cout << "(" << move_x << ", " << move_y << "), " << std::flush;

        if (std::abs(move_x) < MINIMUM_FEATURE_MOVE || std::abs(move_y) < MINIMUM_FEATURE_MOVE) {
            continue;
        }

        sum_move_x += move_x;
        sum_move_y += move_y;
        count++;
    }
    std::cout << std::endl;
    if (0 < count) {
        ave_move_x = sum_move_x / count;
        ave_move_y = sum_move_y / count;
    }
    else {
        move_x = 0;
        move_y = 0;
    }

    bounding_rect[1].x = cvRound(bounding_rect[0].x + move_x);
    bounding_rect[1].y = cvRound(bounding_rect[0].y + move_y);
    bounding_rect[1].width = bounding_rect[0].width;
    bounding_rect[1].height = bounding_rect[0].height;
}

void TrackingPerson::OverwriteLog()
{
    bounding_rect[0] = bounding_rect[1];

    int count = 0;

    for (int index = 0; index < (int)track_points[1].size(); index++) {
        if (!lk_status[index]) {
            continue;
        }
        track_points[0][count] = track_points[1][index];
        count++;
    }
    track_points[0].resize(count);
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
    cv::Rect rect = bounding_rect[0];
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
