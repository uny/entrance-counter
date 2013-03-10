#include "people_tracker.h"

#include <iostream>

#include <opencv2/video/tracking.hpp>

PeopleTracker::PeopleTracker()
{
}

void PeopleTracker::Track(const cv::Mat &frame, std::vector<TrackingPerson> &tracking_people)
{
    std::vector<float> lk_error;

    for (int index = 0; index < tracking_people.size(); index++) {
        if (!tracking_people[index].track_points[0].empty()) {
            if (prev_frame_.empty()) {
                frame.copyTo(prev_frame_);
            }
            cv::Rect larger_rect = ExpandRectToTrack(tracking_people[index].bounding_rect[0], frame);
            cv::calcOpticalFlowPyrLK(prev_frame_(larger_rect),
                                     frame(larger_rect),
                                     tracking_people[index].track_points[0],
                                     tracking_people[index].track_points[1],
                                     tracking_people[index].lk_status,
                                     lk_error);
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
    if (frame.cols < rect.x + rect.width) {
        larger.width = frame.cols - rect.x;
    }
    if (frame.rows < rect.y + rect.height) {
        larger.height = frame.rows - rect.y;
    }

    return larger;
}

