#include <iostream>
// for debug
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "image_holder.h"
#include "tracking_person.h"
#include "people_detector.h"
#include "people_tracker.h"

using namespace std;

class App
{
public:
    App()
    {

    }

    void Run()
    {
        cv::VideoCapture capture("Resources/768x576.avi");
        cv::Mat frame;
        // for debug
        cv::Mat draw_mat;

        // i do not know why this is here, ref. bgfg_gmg.cpp
        cv::initModule_video();

        ImageHolder image_holder;
        image_holder.Init();

        std::vector<TrackingPerson> tracking_people;

        PeopleDetector people_detector;
        people_detector.Init();

        PeopleTracker people_tracker;

//        int64 start_time;

        while (true) {
//            start_time = cv::getTickCount();

            capture >> frame;
            if (frame.empty()) {
                break;
            }

            // image_holder has color, gray, diff images
            image_holder.Update(frame);

            // move rects of people with lucas-kanade optical flow
            people_tracker.Track(image_holder, tracking_people);
            // add new people to tracking_people
            people_detector.Detect(image_holder, tracking_people);

            // for debug
            frame.copyTo(draw_mat);

            for (const TrackingPerson &tracking_person : tracking_people) {
                cv::rectangle(draw_mat, tracking_person.bounding_rect[TP_TRANSITION_NEXT], cv::Scalar(0, 255, 0), 3);
                for (int index = 0; index < tracking_person.track_points[TP_TRANSITION_PREV].size(); index++) {
                    cv::circle(draw_mat, tracking_person.track_points[TP_TRANSITION_NEXT][index], 1, cv::Scalar(0, 255, 0), 3);
                    cv::line(draw_mat, tracking_person.track_points[TP_TRANSITION_PREV][index], tracking_person.track_points[TP_TRANSITION_NEXT][index], cv::Scalar(0, 0, 255), 1);
                }
                for (const cv::Point &centroid : tracking_person.centroid) {
                    cv::circle(draw_mat, centroid, 1, cv::Scalar(255, 0, 0), 3);
                }
            }

            cv::imshow("debug", draw_mat);

            // overwrite next -> prev
            TrackingPerson::OverwriteLog(tracking_people);

//            if (cv::waitKey(0) == 'q') {
//                break;
//            }
            if (0 < cv::waitKey(1)) {
                break;
            }

//            std::cout << (cv::getTickCount() - start_time) / cv::getTickFrequency() * 1000 << std::endl;
        }
    }
};

int main()
{
    App app;
    app.Run();

    return 0;
}

