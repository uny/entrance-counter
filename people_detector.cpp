#include "people_detector.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

PeopleDetector::PeopleDetector()
{
}

void PeopleDetector::Init()
{
    // prepare hog
    hog_.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

void PeopleDetector::Detect(const ImageHolder &image_holder, std::vector<TrackingPerson> &tracking_people)
{
    cv::Mat roi_mat;
    // normalized mat for good features to track
    cv::Mat roi_diff;

    // HoGed rects
    std::vector<cv::Rect> person_rects;

    cv::Rect intersect_rect;

    int centroid_x;
    int centroid_y;

    cv::Mat unsharp_mask = (cv::Mat_<double>(3, 3) <<
                            -1.0 / 9, -1.0 / 9, -1.0 / 9,
                            -1.0 / 9, 17.0 / 9, -1.0 / 9,
                            -1.0 / 9, -1.0 / 9, -1.0 / 9);

    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, TERMCRIT_MAX_COUNT, TERMCRIT_EPSILON / 10.0);

    for (const cv::Rect &roi_rect : image_holder.diff_rects) {
        roi_mat = ResizeFrameForHoG(image_holder.gray, roi_rect);
        cv::filter2D(roi_mat, roi_mat, -1, unsharp_mask);

        // TODO: should set parameters
        hog_.detectMultiScale(roi_mat, person_rects);

        for (cv::Rect &person_rect : person_rects) {
            JustifyPersonRect(person_rect, roi_rect);
            // TODO: ?check image should not out of the image

            bool overlapped = false;
            for (TrackingPerson &tracking_person : tracking_people) {
                intersect_rect = person_rect & tracking_person.bounding_rect[1];
                if (roi_rect.area() * (OVERLAP_THRESHOLD / 10.0) < intersect_rect.area()) {
                    tracking_person.bounding_rect[1] = person_rect;
                    overlapped = true;
                    break;
                }
            }
            if (overlapped) {
                continue;
            }

            roi_diff = image_holder.diff(person_rect);

            TrackingPerson tracking_person;
            tracking_person.bounding_rect[TP_TRANSITION_NEXT] = person_rect;

            // TODO: tune this parameter
            cv::goodFeaturesToTrack(roi_diff,
                                    tracking_person.track_points[TP_TRANSITION_NEXT],
                                    FEATURE_MAXIMUM_NUM,
                                    (FEATURE_QUALITY / 100.0),
                                    FEATURE_MINIMUM_DISTANCE);
            cv::cornerSubPix(roi_diff,
                             tracking_person.track_points[TP_TRANSITION_NEXT],
                             cv::Size(5, 5),
                             cv::Size(-1, -1),
                             termcrit);
            tracking_person.JustifyFeaturesPoint(cv::Point(0, 0), person_rect.tl(), TP_TRANSITION_NEXT);
            tracking_person.track_confidence = 1.0;
            // should call this method after justified
            tracking_person.AppendCentroid();
            tracking_people.push_back(tracking_person);
        }
    }
}

void PeopleDetector::ExpandRoIRectForHoG(cv::Rect &rect, const cv::Mat &frame)
{
    rect.x -= cvRound(rect.width * (HOG_MARGIN_WIDTH * 0.01));
    rect.width = cvRound(rect.width * (1.0 + (HOG_MARGIN_WIDTH * 0.01)));
    rect.y -= cvRound(rect.height * (HOG_MARGIN_WIDTH * 0.01));
    rect.height = cvRound(rect.height * (1.0 + (HOG_MARGIN_WIDTH * 0.01)));

    rect.x = std::max(rect.x, 0);
    rect.y = std::max(rect.y, 0);
    if (frame.cols < rect.x + rect.width) {
        rect.width = frame.cols - rect.x;
    }
    if (frame.rows < rect.y + rect.height) {
        rect.height = frame.rows - rect.y;
    }
}

cv::Mat PeopleDetector::ResizeFrameForHoG(const cv::Mat &image, const cv::Rect &rect)
{
    cv::Rect larger = rect;
    ExpandRoIRectForHoG(larger, image);
    cv::Mat resized = image(larger);
    cv::resize(resized, resized, cv::Size(HOG_IMAGE_WIDTH, HOG_IMAGE_WIDTH * 2));
    return resized;
}

void PeopleDetector::JustifyPersonRect(cv::Rect &rect, const cv::Rect &roi_rect)
{
    // TODO: it should be a bit larger
    rect.width *= (double)roi_rect.width / HOG_IMAGE_WIDTH;
    rect.height *= (double)roi_rect.height / (HOG_IMAGE_WIDTH * 2);

    rect.x += roi_rect.x;
    rect.y += roi_rect.y;
}
