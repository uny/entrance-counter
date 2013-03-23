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

    int index;

    std::vector<int> move_x;
    std::vector<int> move_y;

    int certain_size;

    int ave_move_x;
    int ave_move_y;

    int centroid_x;
    int centroid_y;

    std::vector<cv::Point2f>::iterator prev_points_iter = track_points[TP_TRANSITION_PREV].begin();
    std::vector<cv::Point2f>::iterator next_points_iter = track_points[TP_TRANSITION_NEXT].begin();

    // remove uncertain points
    for (index = 0; index < (int)lk_status.size(); index++) {
        // check also bounding contain for consistency with size
        if (lk_status[index]
                && bounding_rect[TP_TRANSITION_PREV].contains(*prev_points_iter)) {
            move_x.push_back(next_points_iter->x - prev_points_iter->x);
            move_y.push_back(next_points_iter->y - prev_points_iter->y);

            ++prev_points_iter;
            ++next_points_iter;
        }
        else {
            prev_points_iter = track_points[TP_TRANSITION_PREV].erase(prev_points_iter);
            next_points_iter = track_points[TP_TRANSITION_NEXT].erase(next_points_iter);
        }
    }
    // sort to exclude outliers
    std::sort(move_x.begin(), move_x.end());
    std::sort(move_y.begin(), move_y.end());
    // prev and next point vector will have the same size

    certain_size = track_points[TP_TRANSITION_PREV].size();

    // certain_size / 3 -> certain_size * 2/3
    for (index = (certain_size / EXCLUDE_OUTLINER_RATIO); index < (1 - certain_size / EXCLUDE_OUTLINER_RATIO); index++) {
        sum_move_x += track_points[TP_TRANSITION_NEXT][index].x - track_points[TP_TRANSITION_PREV][index].x;
        sum_move_y += track_points[TP_TRANSITION_NEXT][index].y - track_points[TP_TRANSITION_PREV][index].y;
        count++;
    }

    // update confidence
    track_confidence *= (double)count / lk_status.size();
    if (track_confidence < (MINIMUM_TRACK_CONFIDENCE / 10.0)) {
        return false;
    }

    // enough confidence
    if (0 < count) {
        ave_move_x = sum_move_x / count;
        ave_move_y = sum_move_y / count;
    }
    else {
        ave_move_x = 0;
        ave_move_y = 0;
    }

    bounding_rect[TP_TRANSITION_NEXT].x = cvRound(bounding_rect[TP_TRANSITION_PREV].x + ave_move_x);
    bounding_rect[TP_TRANSITION_NEXT].y = cvRound(bounding_rect[TP_TRANSITION_PREV].y + ave_move_y);
    bounding_rect[TP_TRANSITION_NEXT].width = bounding_rect[TP_TRANSITION_PREV].width;
    bounding_rect[TP_TRANSITION_NEXT].height = bounding_rect[TP_TRANSITION_PREV].height;

    prev_points_iter = track_points[TP_TRANSITION_PREV].begin();
    next_points_iter = track_points[TP_TRANSITION_NEXT].begin();

    // add centroid
    AppendCentroid();

    // do not remove next points here for consistency with prev points size

    return true;
}

void TrackingPerson::AppendCentroid()
{
    int centroid_x = cvRound(bounding_rect[TP_TRANSITION_NEXT].x + bounding_rect[TP_TRANSITION_NEXT].width / 2);
    int centroid_y = cvRound(bounding_rect[TP_TRANSITION_NEXT].y + bounding_rect[TP_TRANSITION_NEXT].height / 2);
    centroid.push_back(cv::Point2f(centroid_x, centroid_y));
}

void TrackingPerson::OverwriteLog(std::vector<TrackingPerson> &tracking_people)
{
    for (TrackingPerson &tracking_person : tracking_people) {
        if (!tracking_person.track_points[TP_TRANSITION_NEXT].size()) {
            continue;
        }
        tracking_person.bounding_rect[TP_TRANSITION_PREV] = tracking_person.bounding_rect[TP_TRANSITION_NEXT];
        tracking_person.track_points[TP_TRANSITION_PREV].swap(tracking_person.track_points[TP_TRANSITION_NEXT]);
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
