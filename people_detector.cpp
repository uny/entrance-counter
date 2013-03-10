#include "people_detector.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

PeopleDetector::PeopleDetector()
{
}

void PeopleDetector::Init()
{
    // prepare for background substraction, ref. bgfg_gmg.cpp
    fgbg_ = cv::Algorithm::create<cv::BackgroundSubtractorGMG>("BackgroundSubtractor.GMG");
    if (fgbg_.empty()) {
        std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
        return;
    }
    // TODO: maybe need to tune these parameters
    fgbg_->set("initializationFrames", GMG_INIT_FRAME_NUM);
    fgbg_->set("decisionThreshold", GMG_THRESHOLD);

    // HoG
    hog_.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

void PeopleDetector::Detect(const cv::Mat &frame, std::vector<TrackingPerson> &tracking_people)
{
    cv::Mat fgmask;
    cv::Mat roi_mat;

    // diff and labeled rect
    cv::Rect roi_rect;
    // HoGed rects
    std::vector<cv::Rect> person_rects;

    cv::Rect intersect_rect;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> contours_poly;

    (*fgbg_)(frame, fgmask);
    cv::erode(fgmask, fgmask, cv::Mat());
    cv::dilate(fgmask, fgmask, cv::Mat());
    cv::dilate(fgmask, fgmask, cv::Mat());
    cv::erode(fgmask, fgmask, cv::Mat());

    cv::Mat unsharp_mask = (cv::Mat_<double>(3, 3) << -1.0 / 9, -1.0 / 9, -1.0 / 9,
                                                      -1.0 / 9, 17.0 / 9, -1.0 / 9,
                                                      -1.0 / 9, -1.0 / 9, -1.0 / 9);

    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);

    cv::findContours(fgmask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (std::vector<cv::Point> contour : contours) {
        if (cv::contourArea(contour) < MINIMUM_AREA) {
            continue;
        }
        cv::approxPolyDP(cv::Mat(contour), contours_poly, cv::arcLength(cv::Mat(contour), true) * 0.02, true);
        roi_rect = cv::boundingRect(cv::Mat(contours_poly));

        roi_mat = ResizeFrameForHoG(frame, roi_rect);
        cv::filter2D(roi_mat, roi_mat, -1, unsharp_mask);

        hog_.detectMultiScale(roi_mat, person_rects);

        for (cv::Rect person_rect : person_rects) {
            JustifyPersonRect(person_rect, roi_rect);

            bool overlapped = false;
            for (int index = 0; index < tracking_people.size(); index++) {
                intersect_rect = person_rect & tracking_people[index].bounding_rect[1];
                if (roi_rect.area() * OVERLAP_THRESHOLD < intersect_rect.area()) {
//                    std::cout << "interset: " << intersect_rect << std::endl;
                    tracking_people[index].bounding_rect[1] = person_rect;
                    overlapped = true;
                    break;
                }
            }
            if (overlapped) {
                continue;
            }

            TrackingPerson tracking_person;
            tracking_person.bounding_rect[0] = person_rect;
            tracking_person.missing_count = 0;
            cv::goodFeaturesToTrack(frame(person_rect),
                                    tracking_person.track_points[0],
                                    FEATURE_MAXIMUM_NUM,
                                    FEATURE_QUALITY,
                                    FEATURE_MINIMUM_DISTANCE);
            cv::cornerSubPix(frame(person_rect), tracking_person.track_points[0], cv::Size(10, 10), cv::Size(-1, -1), termcrit);
            tracking_person.JustifyFeaturesPoint(cv::Point(0, 0), person_rect.tl(), tracking_person.TP_JUSTIFY_PREV);
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

cv::Mat PeopleDetector::ResizeFrameForHoG(const cv::Mat &image, cv::Rect &rect)
{
    ExpandRoIRectForHoG(rect, image);
    cv::Mat resized = image(rect);
    cv::resize(resized, resized, cv::Size(HOG_IMAGE_WIDTH, HOG_IMAGE_WIDTH * 2));
    return resized;
}

void PeopleDetector::JustifyPersonRect(cv::Rect &rect, const cv::Rect &roi_rect)
{
    rect.width *= (double)roi_rect.width / HOG_IMAGE_WIDTH;
    rect.height *= (double)roi_rect.height / (HOG_IMAGE_WIDTH * 2);

    rect.x += roi_rect.x;
    rect.y += roi_rect.y;
}

void PeopleDetector::JustifyFeaturePoint(cv::Point &point, const cv::Rect &roi_rect)
{
    point.x += roi_rect.x;
    point.y += roi_rect.y;
}
