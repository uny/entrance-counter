#include "people_tracker.h"

#include <iostream>

#include <opencv2/video/tracking.hpp>

PeopleTracker::PeopleTracker()
{
}

void PeopleTracker::Track(const cv::Mat &frame, std::vector<TrackingPerson> &tracking_people)
{
    std::vector<float> lk_error;

    // change track method to fast
    for (int index = 0; index < (int)tracking_people.size(); index++) {
        if (!tracking_people[index].track_points[0].empty()) {
            if (prev_frame_.empty()) {
                frame.copyTo(prev_frame_);
            }
            cv::Rect larger_rect = ExpandRectToTrack(tracking_people[index].bounding_rect[0], frame);
            tracking_people[index].JustifyFeaturesPoint(larger_rect.tl(), cv::Point(0, 0), tracking_people[index].TP_JUSTIFY_PREV);
            cv::calcOpticalFlowPyrLK(prev_frame_(larger_rect),
                                     frame(larger_rect),
                                     tracking_people[index].track_points[0],
                                     tracking_people[index].track_points[1],
                                     tracking_people[index].lk_status,
                                     lk_error);
            tracking_people[index].JustifyFeaturesPoint(cv::Point(0, 0), larger_rect.tl(), tracking_people[index].TP_JUSTIFY_BOTH);
            tracking_people[index].MoveRect();
        }
    }

    frame.copyTo(prev_frame_);
}

cv::Rect PeopleTracker::ExpandRectToTrack(const cv::Rect &rect, const cv::Mat &frame)
{
    cv::Rect larger;
    larger.x = std::max(rect.x - MARGIN_WIDTH, 0);
    larger.y = std::max(rect.y - MARGIN_WIDTH, 0);
    larger.width = rect.width + MARGIN_WIDTH * 2;
    larger.height = rect.height + MARGIN_WIDTH * 2;
    if (frame.cols < larger.x + larger.width) {
        larger.width = frame.cols - larger.x;
    }
    if (frame.rows < larger.y + larger.height) {
        larger.height = frame.rows - larger.y;
    }

    return larger;
}

