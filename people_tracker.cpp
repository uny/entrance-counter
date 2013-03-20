#include "people_tracker.h"

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>

PeopleTracker::PeopleTracker()
{
}

void PeopleTracker::Track(const ImageHolder &image_holder, std::vector<TrackingPerson> &tracking_people)
{
    // shallow copy for naming
    cv::Mat gray = image_holder.gray;

    std::vector<float> lk_error;
    cv::Rect larger_rect;
    std::vector<uchar> reverse_lk_status;
    std::vector<cv::Point2f> reverse_track_points;

    // person iterator
    std::vector<TrackingPerson>::iterator p_iter = tracking_people.begin();
    // feature index for loop
    int f_index = 0;

    cv::Mat roi_mat;
    cv::FastFeatureDetector feature_detector;
    std::vector<cv::KeyPoint> fast_keypoints;
    cv::FREAK freak;
    cv::Mat descriptors;
    cv::BruteForceMatcher<cv::Hamming> matcher;
    std::vector<cv::DMatch> matches;

    while (p_iter != tracking_people.end()) {
        // now tracking successfully
        if (p_iter->missing_count == 0) {
            // when first
            if (prev_frame_.empty()) {
                gray.copyTo(prev_frame_);
            }
            larger_rect = p_iter->ExpandRectToTrack(gray.size());
            // set features points in the bounding rect
            p_iter->JustifyFeaturesPoint(larger_rect.tl(), cv::Point(0, 0), TP_TRANSITION_PREV);
            // TODO: abort
            cv::calcOpticalFlowPyrLK(prev_frame_(larger_rect),
                                     gray(larger_rect),
                                     p_iter->track_points[0],
                                     p_iter->track_points[1],
                                     p_iter->lk_status,
                                     lk_error);
            // check by reverse
            cv::calcOpticalFlowPyrLK(gray(larger_rect),
                                     prev_frame_(larger_rect),
                                     p_iter->track_points[1],
                                     reverse_track_points,
                                     reverse_lk_status,
                                     lk_error);
            // if reverse check is error, see it as an error
            for (f_index = 0; f_index < (int)reverse_lk_status.size(); f_index++) {
                if (!reverse_lk_status[f_index]) {
                    p_iter->lk_status[f_index] = 0;
                }
            }
            // reset features points for the whole image
            p_iter->JustifyFeaturesPoint(cv::Point(0, 0), larger_rect.tl(), TP_TRANSITION_BOTH);
            p_iter->MoveRect();
        }
        else {
//            for (const cv::Rect &roi_rect : image_holder.diff_rects) {
//                roi_mat = image_holder.gray(roi_rect);
//                feature_detector.detect(roi_mat, fast_keypoints);
//                freak.compute(roi_mat, fast_keypoints, descriptors);
//                matcher.match(p_iter->descriptors, descriptors, matches);
//                cv::Mat draw;
//                std::vector<cv::KeyPoint> tmp_keypoints;
//                cv::KeyPoint::convert(p_iter->debug_points, tmp_keypoints);
//                cv::drawMatches(p_iter->debug, tmp_keypoints, roi_mat, fast_keypoints, matches, draw);
//                double tmp = 0;
//                for (const cv::DMatch &dmatch : matches) {
//                    tmp += dmatch.distance;
//                }
//                std::cout << tmp / matches.size() << ", " << std::flush;
//                cv::imshow("debug", p_iter->debug);
//                cv::imshow("match", draw);
//                cv::waitKey(0);
//            }
//            std::cout << std::endl;
            if (MAXIMUM_MISS_COUNT < p_iter->missing_count) {
                p_iter = tracking_people.erase(p_iter);
                continue;
            }
        }
        ++p_iter;
    }
    // TODO: retrack after miss

    // TODO: it should be clone ? or copyTo?
    gray.copyTo(prev_frame_);
}
