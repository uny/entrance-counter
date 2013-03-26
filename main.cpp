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
#include "people_counter.h"

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

        std::vector<TrackingPerson> tracking_people;

        PeopleDetector people_detector;
        people_detector.Init();

        PeopleTracker people_tracker;

        PeopleCounter people_counter;

        int64 tick_cnt = cv::getTickCount();
        int64 start_time;
        int64 diff_time;
        int frame_cnt = 0;
        int prev_frame_cnt = 0;

        while (true) {
            capture >> frame;
            if (frame.empty()) {
                break;
            }
            static int draw_flag = -1;
            if (draw_flag++ == 0) {
                cv::waitKey(0);
            }


            start_time = cv::getTickCount();
            // image_holder has color, gray, diff images (it takes too much time!)
            image_holder.Update(frame);
//            std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << ", " << std::flush;

            start_time = cv::getTickCount();
            // move rects of people with lucas-kanade optical flow
            people_tracker.Track(image_holder, tracking_people);
//            std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << ", " << std::flush;

            start_time = cv::getTickCount();
            // add new people to tracking_people
            people_detector.Detect(image_holder, tracking_people);
//            std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << ", " << std::endl;
            people_counter.Count(tracking_people);

            // for debug
            frame.copyTo(draw_mat);

//            for (const TrackingPerson &tracking_person : tracking_people) {
//                cv::rectangle(draw_mat, tracking_person.bounding_rect[TP_TRANSITION_NEXT], cv::Scalar(0, 255, 0), 3);
//                for (int index = 0; index < (int)tracking_person.track_points[TP_TRANSITION_PREV].size(); index++) {
//                    cv::line(draw_mat, tracking_person.track_points[TP_TRANSITION_PREV][index], tracking_person.track_points[TP_TRANSITION_NEXT][index], cv::Scalar(0, 0, 255), 2);
//                }
//                for (const cv::Point &centroid : tracking_person.centroid) {
//                    cv::circle(draw_mat, centroid, 1, cv::Scalar(255, 0, 0), 3);
//                }
//            }
            people_counter.DrawForDebug(draw_mat, tracking_people);

            cv::imshow("debug", draw_mat);

            // overwrite next -> prev
            TrackingPerson::OverwriteLog(tracking_people);

//            if (cv::waitKey(0) == 'q') {
//                break;
//            }
            if (0 < cv::waitKey(1)) {
                break;
            }

            diff_time = (cv::getTickCount() - tick_cnt) * 1000 / cv::getTickFrequency();
            if (1000 <= diff_time) {
                std::cout << "fps: " << frame_cnt - prev_frame_cnt << std::endl;
                tick_cnt = cv::getTickCount();
                prev_frame_cnt = frame_cnt;
            }
            frame_cnt++;
        }
    }
};

int main()
{
    App app;
    app.Run();

    return 0;
}

